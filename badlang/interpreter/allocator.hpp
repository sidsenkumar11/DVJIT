/**
 * allocator.hpp
 * 
 * wrap asmjit's allocator for use by executing jitcode
 */

#pragma once

#include "asmjit/asmjit.h"
#include <cstdlib>
#include <stdint.h>

/**
 * Initialize the allocator with an asmjit instance
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
