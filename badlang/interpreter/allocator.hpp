/**
 * allocator.hpp
 * 
 * Contains an _extremely_ basic heap memory manager.
 * 
 * Entries in the allocator represent either free or
 * occupied segments of memory. The entries form a
 * doubly linked-list pointing forward in memory.
 * `alloc_page_start` points to the first entry.
 * 
 * 
 * This is NOT thread-safe.
 */

#pragma once

#include "asmjit/asmjit.h"
#include <cstdlib>
#include <stdint.h>


#define ALLOC_FLAG_OCCUPIED   (0b001)

// this entry is the beginning of a page
#define ALLOC_FLAG_PAGE_START (0b010)

// this entry has no successors
#define ALLOC_FLAG_END        (0b100)


struct alloc_entry_header
{
    alloc_entry_header  *next;
    alloc_entry_header  *prev;
    size_t               size;
    bool                 in_use;
};

#define DATA_SECTION(e) (void *)((char *)e + sizeof(alloc_entry_header))


/**
 * Initialize the allocator with a hint address for
 * placement of the first page of memory
 */
void alloc_init(asmjit::JitAllocator *a);


/**
 * Acquire memory of size at least `size`.
 */
void *alloc_get(size_t size);


/**
 * Mark the given memory segment as free for reuse.
 */
void alloc_free(void *ptr);


/**
 * (internal) Allocate a new page of memory, add
 * the entry header, and return it
 */
alloc_entry_header *alloc_new_page(void *hint);
