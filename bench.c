/* bench.c - source code for the bench program
 *
 * This program executes a large number of calls to functions getmem and freemem
 * to allocate and free blocks of random sizes and in random order.
 * The user may specify parameters to control the test. Trailing parameters can
 * be omitted, in which case default values are used.
 *
 * Synopsis:
 * bench [ntrials] [pctget] [pctlarge] [small_limit] [large_limit] [random_seed]
 *
 * Parameters (default values in brackets):
 * ntrials: total number of getmem/freemem calls to perform [10000]
 * pctget: % of total getmem/freemem calls that should be getmem [50]
 * pctlarge: % of getmem calls for "large" blocks (size > small_limit) [10]
 * small_limit: largest size in bytes of a "small" block [200]
 * large_limit: largest size in bytes of a "large" block [20000]
 * random_seed: initial seed value for random number generator [system time]
 */

#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <math.h>  // gcc requires the -lm flag to link the math library
#include <string.h>  // for memset
#include "mem.h"

#define FILE_NAME "print_heap.txt"  // print heap to this file

/* Function declarations */
void doTest(unsigned int pctget, unsigned int pctlarge,
    unsigned int small_limit, unsigned int large_limit, void** myNodes);
int parseArguments(unsigned int par[], int argc, char *argv[]);
int isValid(char str[]);
void printTestStatistics(float totalCPUTime, uintptr_t totalStorage,
  uintptr_t totalBlocks, uintptr_t averageBytes);

/* Testbench for the memory manager.
 * Pre-condition: command line arguments are valid parameter values.
 */
int main(int argc, char *argv[]) {
  FILE *fp = fopen(FILE_NAME, "w");
  float start, current;  // start time, current time
  unsigned int par[6] = { 10000, 50, 10, 200, 20000, time(NULL) };
  uintptr_t total_size, total_free, free_blocks;  // statistics
  int j = 0;  // tracks number of times stats are printed

  // array of pointers to track getmem pointers
  void** myNodes = calloc(par[0], sizeof(void*));

  if (!myNodes) {
    fprintf(stderr, "bench: failed to calloc array of pointers\n");
    return EXIT_FAILURE;
  }
  if (parseArguments(par, argc, argv)) {
    return EXIT_FAILURE;
  }
  srand(par[5]);  // set random seed
  start = clock();

  if (par[0] < 10) {
    for (int i = 1; i <= par[0]; ++i) {
      doTest(par[1], par[2], par[3], par[4], myNodes);
      current = (float) (clock() - start) / CLOCKS_PER_SEC;
      get_mem_stats(&total_size, &total_free, &free_blocks);
      printf("Trial: %d out of %d\n", i, par[0]);
      printTestStatistics(current, total_size, free_blocks,
          (free_blocks) ? round(total_free/free_blocks) : 0);
      ++j;
    }
  } else {
    int pct = round(0.1 * par[0]);  // percent progress
    for (int i = 1; i <= par[0]; ++i) {
      doTest(par[1], par[2], par[3], par[4], myNodes);
      if (!(i % pct)) {
        current = (float) (clock() - start) / CLOCKS_PER_SEC;
        get_mem_stats(&total_size, &total_free, &free_blocks);
        printf("Trial: %d out of %d", i, par[0]);
        printf(" (%.2f%%)\n", (float) i / (float) par[0] * 100);
        printTestStatistics(current, total_size, free_blocks,
            (free_blocks) ? round(total_free/free_blocks) : 0);
        ++j;
        pct = round(0.1 * (j+1) * par[0]);  // next percent milestone
      }
    }
  }
  printf("Printed test statistics %d times\n", j);
  printf("Printed heap to %s\n", FILE_NAME);
  print_heap(fp);
  fclose(fp);
  return EXIT_SUCCESS;
}

/* Call freemem or getmem randomly according to parameters.
 *
 * If the choice is to allocate a block, then if the pointer returned
 * by getmem is not NULL, the program should store the value 0xFE in
 * each of the first 16 bytes of the allocated block starting at
 * the pointer address requested by getmem.
 *
 * If the decision is to allocate a small block, request a block
 * whose size is a random number between 1 and small_limit.
 * If the decision is to allocate a large block, request a block
 * whose size is is a random number between small_limit and large_limit.
 *
 * If the choice is to free a block, one of the previously allocated blocks
 * should be picked randomly to be freed.
 *
 * pctget: % of total getmem/freemem calls that should be getmem
 * pctlarge: % of getmem calls for "large" blocks (size > small_limit)
 * small_limit: largest size in bytes of a "small" block
 * large_limit: largest size in bytes of a "large" block
 */
void doTest(unsigned int pctget, unsigned int pctlarge,
    unsigned int small_limit, unsigned int large_limit, void** myNodes) {
  static int totalPtrs = 0;  // tracks number of pointers
  if (rand() % 100 < pctget) {  // getmem call
    void* ptr;
    if (rand() % 100 < pctlarge) {  // large block
      ptr = getmem((uintptr_t) rand() % (large_limit + 1 - small_limit)
          + small_limit);
    } else {  // small block
      ptr = getmem((uintptr_t) rand() % small_limit + 1);
    }
    if (ptr) {  // getmem doesn't return null
      myNodes[totalPtrs] = ptr;
      totalPtrs++;
      memset(ptr, 254, 16);  // 0xFE = 254 - magic number
    }
  } else {  // freemem call
    if (totalPtrs) {
      int freedNode = rand() % totalPtrs;  // 0 to (totalPtrs - 1)
      freemem(myNodes[freedNode]);
      myNodes[freedNode] = myNodes[totalPtrs - 1];
      myNodes[totalPtrs - 1] = NULL;
      --totalPtrs;
    }
  }
}

/* Parses command line arguments and prints parameters. 
 * Returns 1 for any error, 0 otherwise.
 *
 * par: stores parameter values
 * argc: number of command line arguments
 * argv: array of command line arguments
 */
int parseArguments(unsigned int par[], int argc, char *argv[]) {
  int ret = 0;  // return value
  char *prnt[6] = { "ntrials", "pctget", "pctlarge", "small_limit",
    "large_limit", "random_seed" };  // parameter names
  if (argc > 7) {
    fprintf(stderr, "bench: cannot take more than 6 parameters\n");
    ret = 1;
  }
  for (int i = 1; i < argc; ++i) {
    if (isValid(argv[i])) {
      par[i-1] = atoi(argv[i]);
    } else {
      fprintf(stderr, "bench: \"%s\" is not a valid parameter value\n",
                argv[i]);
      ret = 1;
    }
  }
  if (par[1] > 100 || par[2] > 100) {
    fprintf(stderr, "bench: percent parameter should not exceed 100%%\n");
    ret = 1;
  }
  if (!ret) {
    printf("Parameters:\n");
    for (int i = 0; i < 6; ++i) {
      printf("   %-15s %u\n", prnt[i], par[i]);
    }
    printf("\n");
  }
  return ret;
}

/* Checks if a char array is a positive integer. Returns 1 if true, 0 otherwise.
 *
 * str: input char array
 */
int isValid(char str[]) {
  for (int i = 0; str[i] != '\0'; ++i) {
    if (!isdigit(str[i])) {
      return 0;
    }
  }
  return 1;
}

/* Prints out relevent statistics.
 *
 * totalCPUTime: total CPU time used so far
 * totalStorage: total amount of storage acquired so far
 * totalBlocks: total number of blocks on the free storage list 
 *    at this point
 * averageBytes: average number of bytes in the free storage 
 *    blocks at this point
 */
void printTestStatistics(float totalCPUTime, uintptr_t totalStorage,
                              uintptr_t totalBlocks, uintptr_t averageBytes) {
  printf("Time: %-15f", totalCPUTime);
  printf("Total storage: %-10" PRIuPTR "", totalStorage);
  printf("Free blocks: %-5" PRIuPTR "", totalBlocks);
  printf("Average bytes: %" PRIuPTR "\n\n", averageBytes);
}
