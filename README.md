# Memory Management Package

The original GitLab repo (identifier: bf, May 2018) was deleted.

* Rogers Xiang - getmem, print_heap, get_mem_stats
* Manchen Jin - freemem, mem_utils

----
### Organization and Algorithms:
The free list data structure is organized as a linked list of FreeNode
data structures. Each node stores informations about an available block:
* size (the amount of storage in bytes)
* next (pointer to the next node on the list)

Blocks on the free list are sorted in order of ascending memory address.
To keep memory aligned, the sizes and addresses of all blocks are required to
be multiples of 16 bytes.

When getmem is called, it searches the free list for a block that can satisfy
the requested amount, deletes the block from the list, and returns the block
to the caller. If the block is large enough, it is split and the remainder
is left on the free list. If no block can satisfy the request, getmem will
acquire a large chunk of storage from the underlying system.

Each block that getmem allocates is actually a bit larger than the requested
size: the free list node is stored at the beginning, and the address returned
to the user comes after the node. 

When freemem is called, it subtracts the appropriate number of bytes from the
given block address to get the actual start address (which contains the size
of the block). When freemem returns the block to the free list, this value is
used to merge it with any immediately adjacent blocks on the list. Addresses
are cast to the type "uintptr_t" when performing address arithmetic.


### Results:
After running the testbench several times with the default parameters, the
following observations were made:
* The number of free blocks at the end was generally in the 20's to 40's
* The average block size (in bytes) was generally in the mid-thousands.
* The ratio of total storage to storage on the free list had a relatively 
wide range. But after changing pctget to a value in favor of freemem (e.g.,
40%), the majority of storage acquired was returned to the free list.
* The elapsed time (in seconds) had the same value before and after the test.
This is likely because the test was too fast (even with 100,000 trials),
since the clock is updated infrequently on many Linux systems.

