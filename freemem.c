/* freemem.c - implementation of function freemem
 *
 * Return the block of storage at location p to the pool of available free storage.
 * The pointer value p must be one that was obtained as the result of a call to getmem.
 * If p is NULL, then the call to freemem has no effect and returns immediately.
 * If p has some value other than one returned by getmem, or if the block it points to
 * has previously been released by another call to freemem, then the operation of freemem
 * is undefined.
 *
 * When freemem returns a block of storage to the pool, if the block is physically located
 * in memory adjacent to one or more other free blocks, then the free blocks involved should
 * be combined into a single larger block, rather than adding the small blocks to the
 * free list individually.
 */

#include <stdlib.h>
#include "mem.h"
#include "mem_impl.h"

/* Return the block at location p to the pool of available free storage.
 *
 * p: the address of the block of storage to be freed. 
 */
void freemem(void* p) {
  if (p) {
    check_heap();
    uintptr_t nsize = sizeof(FreeNode);  // for header space
    FreeNode* node = (FreeNode*) ((uintptr_t) p - nsize);
    uintptr_t currSize = node->size;
    totalFree += currSize + nsize;  // totalFree includes header spaces
    ++freeBlocks;
    // p is before the start of free list
    if ((uintptr_t)node < (uintptr_t)head) {
      if ((uintptr_t)p + currSize == (uintptr_t)head) {  // if they are adjacent
        node->next = head->next;
        node->size = head->size + currSize + nsize;
        head = node;
        --freeBlocks;
      } else {
        node->next = head;
        head = node;
      }
    } else {  // p is after the start of free list
      FreeNode* nextNode = (head) ? head->next : NULL;
      FreeNode* current = head;
      while (nextNode && (uintptr_t)nextNode < (uintptr_t)node) {
        current = nextNode;
        nextNode = nextNode->next;
      }  // nextNode is either null, or comes after node
      // if current touches node, merge
      if (current && (uintptr_t)current + nsize + current->size ==
            (uintptr_t)node) {
        current->size = current->size + currSize + nsize;
        --freeBlocks;
      } else if (current) {
        node->next = current->next;
        current->next = node;
      } else {
        head = node;
        node->next = NULL;
      }
      if (nextNode && (uintptr_t)node + nsize + currSize ==
            (uintptr_t)nextNode) {  // if node touches nextNode
        --freeBlocks;
        // if p is not connected with previous free block
        if (current->next == node) {
          node->size = node->size + nextNode->size + nsize;
          node->next = nextNode->next;
        } else {  // if p is merged with current
          current->next = nextNode->next;
          current->size = current->size + nextNode->size + nsize;
        }
      }
    }
    check_heap();
  }
}
