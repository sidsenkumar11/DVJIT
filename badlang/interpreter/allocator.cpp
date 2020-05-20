#include "allocator.hpp"
#include "helpers.hpp"

#include <iostream>
#include <cstdlib>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>


using namespace std;

/**
 * A pointer to the first mmap'd page in the allocator
 */
static void *alloc_page_start = nullptr;


/**
 * Acquire memory of size at least `size`.
 */
void *alloc_get(size_t size)
{
    if (is_debug())
    {
        cout << "Allocating memory" << endl;
    }

    // Round size /up/ to nearest multiple of 8 (bytes)
    size = (size + 7) >> 3 << 3;

    if (size >= (uint64_t) getpagesize())
    {
        throw "alloc size too large";
    }

    if (size == 0)
    {
        throw "cannot alloc size 0";
    }

    if (alloc_page_start == nullptr)
    {
        alloc_page_start = alloc_new_page(NULL);
    }

    // cursor: current entry being examined
    alloc_entry_header *entry_curr = (alloc_entry_header *)alloc_page_start;

    for (;;)
    {
        if (is_debug())
        {
            cout << "Checking next entry " << std::hex << (uint64_t)entry_curr << endl;
        }
        if (!entry_curr->in_use)
        {
            // This is a free entry. If it's large enough, take it
            if (entry_curr->size >= size)
            {
                if (is_debug())
                {
                    cout << "Found entry to use" << endl;
                }
                // It's large enough. There are 2 cases:
                // 1) entry_size <= size + sizeof(alloc_entry_header).
                //    Simply set in-use flag.
                // 3) entry_size > size + sizeof(alloc_entry_header).
                //    Break the entry into 2.

                // cases 1, 2, 3 require:
                entry_curr->in_use = true;

                if (entry_curr->size > size + sizeof(alloc_entry_header))
                {
                    if (is_debug())
                    {
                        cout << "Breaking up entry" << endl;
                    }
                    // case 3: break into 2 entries
                    alloc_entry_header *next =
                        (alloc_entry_header *)(
                            ((char *)entry_curr) +
                            sizeof(alloc_entry_header) +
                            size
                        );
                    next->size = (entry_curr->size - size - sizeof(alloc_entry_header));

                    next->next = entry_curr->next;
                    next->prev = entry_curr;
                    if (entry_curr->next != nullptr)
                    {
                        entry_curr->next->prev = next;
                    }
                    entry_curr->next = next;
                    entry_curr->size = size;
                }

                // Done. Return a pointer to the entry's data section.

                if (is_debug())
                {
                    cout << "Returning data from entry " << std::hex << (uint64_t)entry_curr << endl;
                }
                return DATA_SECTION(entry_curr);
            }
        } // end (is free entry)

        // Advance entry pointer
        
        if (entry_curr->next == nullptr)
        {
            // we need more space, allocate it
            entry_curr->next = alloc_new_page(entry_curr);
            entry_curr->next->prev = entry_curr;
        }

        entry_curr = entry_curr->next;
    }
}


void alloc_free(void *ptr)
{
    // for convenience -- bitmask to determine the page of a pointer
    const uintptr_t page_mask = ~((uintptr_t)getpagesize() - 1);

    if (ptr == nullptr)
    {
        return;
    }

    alloc_entry_header *entry = (alloc_entry_header *)((char *)ptr - sizeof(alloc_entry_header));

    if (is_debug())
    {
        cout << "Freeing memory from entry " << std::hex << (uint64_t)entry << endl;
    }

    entry->in_use = false;

    // attempt to merge with previous entry
    if (entry->prev != nullptr &&
        entry->prev->in_use == false &&
        // ensure theyre on the same page
        ((uintptr_t)entry->prev & page_mask) == ((uintptr_t)entry & page_mask))
    {
        entry->prev->size = entry->prev->size + entry->size + sizeof(alloc_entry_header);
        entry->prev->next = entry->next;
        if (entry->next != nullptr)
        {
            entry->next->prev = entry->prev;
        }
        entry = entry->prev;
    }

    // attempt to merge with next entry
    if (entry->next != nullptr &&
        entry->next->in_use == false &&
        // ensure theyre on the same page
        ((uintptr_t)entry->next & page_mask) == ((uintptr_t)entry & page_mask))
    {
        entry->size = entry->size + entry->next->size + sizeof(alloc_entry_header);
        entry->next = entry->next->next;
        if (entry->next != nullptr)
        {
            entry->next->prev = entry;
        }
    }

    if (is_debug())
    {
        cout << "Finished freeing" << endl;
    }
}


alloc_entry_header *alloc_new_page(void *hint)
{
    if (is_debug())
    {
        cout << "Allocating new memory page" << endl;
    }

    size_t pagesize = getpagesize();
    void *page = mmap(
        hint,
        pagesize,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANON,
        0,
        0
    );
    if (page == MAP_FAILED)
    {
        throw errno;
    }

    alloc_entry_header *header = (alloc_entry_header *)page;
    header->in_use = false;
    header->next   = nullptr;
    header->prev   = nullptr;
    header->size   = pagesize - sizeof(alloc_entry_header);

    return header;
}
