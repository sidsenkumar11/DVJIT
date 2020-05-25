#include "allocator.hpp"
#include "helpers.hpp"

#include "asmjit/asmjit.h"
#include <iostream>
#include <cstdlib>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>


using namespace std;

/**
 * The asmjit allocator
 */
static asmjit::JitAllocator *my_allocator;

void alloc_init(asmjit::JitAllocator *a)
{
    my_allocator = a;
}


/**
 * Acquire memory of size at least `size`.
 */
void *alloc_get(size_t size)
{
    void *foo;
    void *out;
    my_allocator->alloc(&foo, &out, size);
    return out;
}


void alloc_free(void *ptr)
{
    my_allocator->release(ptr);
}
