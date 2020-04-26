/* get_mem_stats.c - implementation of function get_mem_stats
 * CSE 374, HW6, Team 'bf'
 * Authors: Manchen Jin, Rogers Xiang
 * May 24, 2018
 *
 * Store statistics about the current state of the memory manager in the
 * three integer variables whose addresses are given as arguments.
 *
 * total_size: total amount of storage in bytes acquired by the memory manager.
 * total_free: the total amount of storage in bytes that is currently stored on
 *             the free list, including any space occupied by header information or links.
 * n_free_blocks: the total number of individual blocks currently stored on the free list.
 */

#include <stdlib.h>
#include "mem.h"
#include "mem_impl.h"

/* Store statistics about the memory manager in the given arguments
 *
 * total_size: total storage in bytes
 * total_free: total storage on free list
 * n_free_blocks: number of blocks on free list
 */
void get_mem_stats(uintptr_t* total_size, uintptr_t* total_free,
  uintptr_t* n_free_blocks) {
  *total_size = totalSize;
  *total_free = totalFree;
  *n_free_blocks = freeBlocks;
}
