/* getmem.c - implementation of function getmem
 *
 * Return a pointer to a new block of storage with at least size bytes of memory.
 * The pointer to the returned block should be aligned on an 16-byte boundary.
 * The value size must be greater than 0. If size is not positive, or if for some
 * reason getmem cannot satisfy the request, it should return NULL.
 *
 * getmem actually allocates a block of memory that is a bit larger than the user's request,
 * stores the free list node at the very beginning of that block, and returns to the caller
 * a pointer to the storage that the caller can use, but which actually points a few
 * bytes beyond the real start of the block. Then when freemem is called, it can take
 * the pointer it is given, subtract the appropriate number of bytes to get the real start
 * address of the block, and find the size of the block there.
 */

#include <stdlib.h>
#include "mem.h"
#include "mem_impl.h"

#define MEM_CHUNK 8000  // initial block size
#define BYTE_ALIGN 16  // 16-byte alignment
#define SPLIT_CONSTANT 32  // for splitting a block

/* Function declarations. */
uintptr_t align(uintptr_t size);
FreeNode* newNode(uintptr_t size);
void* splitNode(FreeNode** prevPtr, uintptr_t size);

/* Free list head. */
FreeNode* head = NULL;

/* Memory manager statistic variables (refer to get_mem_stats.c). */
uintptr_t totalSize = 0;  // total amount of bytes acquired
uintptr_t totalFree = 0;  // total amount of bytes on free list
uintptr_t freeBlocks = 0;   // total number of blocks on free list

/* Return a pointer to a new block with at least size bytes of memory.
 * Returns NULL if getmem cannot satisfy request.
 *
 * size: requested size in bytes, must be greater than 0.
 */
void* getmem(uintptr_t size) {
  check_heap();
  FreeNode *prev, *current;  // for list traversal
  if (size <= 0) {
    return NULL;
  }
  size = align(size);  // align on 16-byte boundary
  // get chunk if head is null
  head = head ? head : newNode(size > MEM_CHUNK ? size : MEM_CHUNK);
  if (!head) {
    return NULL;  // if newNode malloc failed
  }
  prev = NULL;
  current = head;
  while (current) {
    if (current->size >= size) {
      // splitNode will check if prev is null (edge case)
      return splitNode(&prev, size);
    }
    prev = current;
    current = current->next;
  }
  // if no node's large enough
  prev->next = newNode(size > MEM_CHUNK ? size : MEM_CHUNK);
  check_heap();
  return splitNode(&prev, size);
}

/* Aligns given size to a multiple of BYTE_ALIGN. 
 *
 * size: requested size in bytes
 */
uintptr_t align(uintptr_t size) {
  uintptr_t mod = size % BYTE_ALIGN;
  if (size < BYTE_ALIGN) {
    return BYTE_ALIGN;
  }
  if (mod) {
    size += (BYTE_ALIGN - mod);
  }
  return size;
}

/* Creates a new FreeNode and updates stats.
 * The allocated block of memory is actually a bit larger than
 * the requested size; the free list node is stored at the
 * very beginning of the block.
 *
 * size: requested size in bytes
 */
FreeNode* newNode(uintptr_t size) {
  uintptr_t msize = size + sizeof(FreeNode);  // malloc size
  FreeNode* node = (FreeNode*) malloc(msize);
  if (node) {  // malloc might return null
    node->next = NULL;
    node->size = size;
    totalSize += msize;
    totalFree += msize;
    ++freeBlocks;
  } else {
    fprintf(stderr, "bench: getmem malloc failed\n");
  }
  return node;
}

/* If the node to be split is substantially larger than the storage requested,
 * divide the block and return a pointer to a portion that is large enough
 * to satisfy the request, leaving the remainder on the free list.
 * If the block is only a little bit larger than the requested size,
 * the entire block is returned.
 * 
 * prevPtr: pointer to pointer to node that comes before the node to be split
 * size: requested getmem size in bytes
 */
void* splitNode(FreeNode** prevPtr, uintptr_t size) {
  FreeNode* prev = *prevPtr;
  uintptr_t nsize = sizeof(FreeNode);  // for header space
  // check if prev is null and set accordingly
  prevPtr = prev ? &(prev->next) : &head;
  // current = prev->next (if prev isn't null), head otherwise
  FreeNode* current = *prevPtr;
  if (current->size > size + SPLIT_CONSTANT) {
    // cast for pointer arithmetic
    *prevPtr = (FreeNode*) ((uintptr_t) current + nsize + size);
    (*prevPtr)->next = current->next;
    (*prevPtr)->size = current->size - nsize - size;
    current->size = size;
  } else {
    *prevPtr = current->next;
    --freeBlocks;
  }
  totalFree -= current->size + nsize;  // includes header spaces
  // return block without header space
  current = (FreeNode*) ((uintptr_t) current + nsize);
  return (void*) current;
}
