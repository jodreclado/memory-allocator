/* mem_impl.h - interface to memory manager functions
 * CSE 374, HW6, Team 'bf'
 * Authors: Manchen Jin, Rogers Xiang
 * May 24, 2018
 */

#ifndef MEMIMPL_H
#define MEMIMPL_H

#include <inttypes.h>  // Required to use type uintptr_t

/* Free memory block */
typedef struct FreeNode {
  struct FreeNode* next;  // pointer to next block on free list
  uintptr_t size;  // block size
} FreeNode;

/* Free list head pointer (defined & initialized in getmem.c) */
extern FreeNode* head;

/* Memory manager statistic variables (defined & initialized in getmem.c) */
extern uintptr_t totalSize;
extern uintptr_t totalFree;
extern uintptr_t freeBlocks;

/* Check for possible problems with the free list data structure. */
void check_heap();

#endif
