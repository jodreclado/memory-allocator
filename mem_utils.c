/* mem_utils.c - implementation of function check_heap
 * CSE 374, HW6, Team 'bf'
 * Authors: Manchen Jin, Rogers Xiang
 * May 24, 2018
 *
 * Check for possible problems with the free list data structure.
 *
 * Uses asserts to verify that the free list has the following properties:
 * - Blocks are ordered with strictly increasing memory addresses.
 * - Block sizes are positive numbers and no smaller than the minimum size.
 * - Blocks do not overlap (start + length of a block is not an address in
 *   the middle of a later block).
 * - Blocks are not touching (start + length of a block should not be the
 *   same address as the next block).
 */

#include <stdlib.h>
#include <assert.h>
#include "mem.h"
#include "mem_impl.h"

/* Check for possible problems with the free list data structure. */
void check_heap() {
    FreeNode* current = head;
    while (current) {
        // check that the size is no smaller than the allowed minimum size
        assert(current->size >= 16);
        if (current->next) {
            // this checks if the blocks order correctly, if they overlapped,
            // and if they touched
            assert((uintptr_t)current + sizeof(FreeNode) + current->size <
                    (uintptr_t)current->next);
        }
        current = current->next;
    }
}
