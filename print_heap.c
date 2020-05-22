/* print_heap.c - implementation of function print_heap
 *
 * Print a formatted listing on file f showing the blocks on the free list.
 * Each line of output should describe one free block and begin with two
 * hexadecimal numbers giving the address and length of that block.
 */

#include <stdlib.h>
#include "mem.h"
#include "mem_impl.h"

/* Print a formatted listing on file f showing the blocks on the free list.
 *
 * f: pointer to the file to be printed to
 */
void print_heap(FILE * f) {
  FreeNode* current = head;
  while (current) {
    fprintf(f, "Address: 0x%08" PRIxPTR "\tSize: 0x%08" PRIxPTR "\n",
        (uintptr_t) current, current->size);
    current = current->next;
  }
}
