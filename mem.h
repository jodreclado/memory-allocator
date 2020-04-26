/* mem.h - interface to memory manager functions
 * CSE 374, HW6, Team 'bf'
 * Authors: Manchen Jin, Rogers Xiang
 * May 24, 2018
 */

#ifndef MEM_H
#define MEM_H

#include <stdio.h>  // Required to use type FILE
#include <inttypes.h>  // Required to use type uintptr_t

/* Return a pointer to a new block with at least size bytes of memory. */
void* getmem(uintptr_t size);

/* Return the block at location p to the pool of available free storage. */
void freemem(void* p);

/* Store statistics about the memory manager in the given arguments. */
void get_mem_stats(uintptr_t* total_size, uintptr_t* total_free,
  uintptr_t* n_free_blocks);

/* Print a formatted listing on file f showing the blocks on the free list. */
void print_heap(FILE * f);

#endif
