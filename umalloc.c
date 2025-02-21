#include "umalloc.h"
#include "csbrk.h"
#include <stdio.h>
#include <assert.h>
#include "ansicolors.h"

const char author[] = ANSI_BOLD ANSI_COLOR_RED "Anant Ghuman asg3966" ANSI_RESET;

/*
 * The following helpers can be used to interact with the mem_block_header_t
 * struct, they can be adjusted as necessary.
 */


mem_block_header_t *free_heads[BIN_COUNT];

/*
 * select_bin - selects a free list bin to use based on the 
 * block size.
 */
mem_block_header_t* select_bin(size_t size) {
    return free_heads[(size <= 16) ? 0
        : (size <= 64) ? 1
        : (size <= 512) ? 2
        : 3];
}

/*
 * block_metadata - returns true if a block is marked as allocated.
 */
bool is_allocated(mem_block_header_t *block) {
    // Student TODO
    return block->block_metadata & 1;
}

/*
 * allocate - marks a block as allocated.
 */
void allocate(mem_block_header_t *block) {
    // Student TODO
    block->block_metadata |= 1;
}


/*
 * deallocate - marks a block as unallocated.
 */
void deallocate(mem_block_header_t *block) {
    // Student TODO
    block->block_metadata &= ~1;
}

/*
 * get_size - gets the size of the block.
 */
size_t get_size(mem_block_header_t *block) {
    // Student TODO
    return block->block_metadata >> 4;
}

/*
 * get_next - gets the next block.
 */
mem_block_header_t *get_next(mem_block_header_t *block) {
    // Student TODO
    return block->next;
}

/*
 * set_block_metadata
 * Optional helper method that can be used to initialize the fields for the 
 * memory block struct. 
 */
void set_block_metadata(mem_block_header_t *block, size_t size, bool alloc) {
    // Optional student todo
    block->block_metadata = size << 4;
    if (alloc) {
        allocate(block);
    }
}

/*
 * get_payload - gets the payload of the block.
 */
 void *get_payload(mem_block_header_t *block) {
    // Student TODO
    if (!block)
        return NULL;
    return (void *)((char *)block + sizeof(mem_block_header_t));
}

/*
 * get_header - given a payload, returns the block.
 */
mem_block_header_t *get_header(void *payload) {
    // Student TODO
    if (!payload)
        return NULL;
    return (mem_block_header_t *)((char *)payload - sizeof(mem_block_header_t));
}

/*
 * The following are helper functions that can be implemented to assist in your
 * design, but they are not required. 
 */

/*
 * find - finds a free block that can satisfy the umalloc request.
 */
mem_block_header_t *find(size_t payload_size) {
    // Student TODO
    if (payload_size == 0)
        return NULL;
    int index = (payload_size <= 16) ? 0
        : (payload_size <= 64) ? 1
        : (payload_size <= 512) ? 2
        : 3;
    // loops through bins to find a block that has a big enough size to satisfy the malloc request
    for (int i = index; i < BIN_COUNT; i++) {
        mem_block_header_t *prev = NULL;
        mem_block_header_t *curr = free_heads[index];
        // loops through the current bin to get a large enough block
        while (curr && get_size(curr) < payload_size + sizeof(mem_block_header_t)) {
            prev = curr;
            curr = curr->next;
        }
        // if a block is found, allocate it, remove it from the free list, and return it, otherwise continue to the next bin
        if (curr) {
            allocate(curr);
            // remove the block from the free list
            if (prev) {
                prev->next = curr->next;
            } else {
                free_heads[i] = curr->next;
            }
            curr->next = NULL;
            return curr;
        }
    }

    // if no valid block found, extend memory
    mem_block_header_t *new_block = extend(ALIGN(payload_size + sizeof(mem_block_header_t)));
    if (!new_block) 
        return NULL;
    return new_block;
}

/*
 * extend - extends the heap if more memory is required.
 */
mem_block_header_t *extend(size_t size) {
    // Student TODO
    if (size == 0)
        return NULL;
    // extends the heap by the given size
    mem_block_header_t *block = (mem_block_header_t *)csbrk(size);
    if (block == NULL) {
        return NULL;
    }
    // sets the metadata for the block and coalesces it
    set_block_metadata(block, size, false);
    block->next = NULL;
    coalesce(block);
    return block;
}

/*
 * split - splits a given block in parts, one allocated, one free.
 */
mem_block_header_t *split(mem_block_header_t *block, size_t new_block_size) {
    size_t block_size = get_size(block);
    new_block_size = ALIGN(new_block_size + sizeof(mem_block_header_t));

    // returns if the block is too small to split
    if (block_size < new_block_size + sizeof(mem_block_header_t) + ALIGNMENT) {
        return block;
    }

    // splits the block into two parts, one allocated and one free
    set_block_metadata(block, new_block_size, true);
    mem_block_header_t *free_block = (mem_block_header_t *)((uintptr_t)block + new_block_size + sizeof(mem_block_header_t));
    set_block_metadata(free_block, block_size - new_block_size - sizeof(mem_block_header_t),false);

    // adds the free block to the free list
    add_to_free_list(free_block);
    return block;
}

/*
 * add_to_free_list - adds a block to the free list.
 */  
void add_to_free_list(mem_block_header_t *block) {
    // finds the bin index for the block
    size_t size = get_size(block);
    int bin_index;
    if (size <= 16)
        bin_index = 0;
    else if (size <= 64)
        bin_index = 1;
    else if (size <= 512)
        bin_index = 2;
    else
        bin_index = 3;
    // adds the block to the free list
    block->next = free_heads[bin_index];
    free_heads[bin_index] = block;
}

/*
 * coalesce - coalesces a free memory block with neighbors.
 */
mem_block_header_t *coalesce(mem_block_header_t *block) {
    if (!block) 
        return NULL;
    // finds the previous and next blocks in memory if they are unallocated
    mem_block_header_t *n = NULL;
    mem_block_header_t *p = NULL;
    // loops through the bins to find the previous and next blocks
    for (int i = 0; i < BIN_COUNT; i++) {
        mem_block_header_t *prev = NULL;
        mem_block_header_t *curr = free_heads[i];
        // loops through all elements in the current bin
        while (curr) {
            mem_block_header_t *next = curr->next; 
            // if the block is the previous block, remove it from the free list
            if ((char *)curr + get_size(curr) + sizeof(mem_block_header_t) == (char *)block) {  
                n = curr;
                if (prev) 
                    prev->next = curr->next;
                else 
                    free_heads[i] = curr->next;
            } 
            // if the block is the next block, remove it from the free list
            else if ((char *)block + get_size(block) + sizeof(mem_block_header_t) == (char *)curr) {  
                p = curr;
                if (prev) 
                    prev->next = curr->next;
                else 
                    free_heads[i] = curr->next;
            } // update prev and current
            else {
                prev = curr;
            }
                curr = next;  
        }
    }
    // if there is no unallocated block before or after the current block, return the block
    if (!n && !p) {
        return block;  
    }
    // if there is no next allocated block, merge the current block with the previous block
    if (!n) {
        set_block_metadata(block, get_size(block) + get_size(p) + sizeof(mem_block_header_t), false);
        return block;
    }
    // if there is no previous allocated block, merge the current block with the previous block
    if (!p) {  
        set_block_metadata(n, get_size(n) + get_size(block) + sizeof(mem_block_header_t), false);
        return n;
    }
    set_block_metadata(n, get_size(n) + get_size(block) + get_size(p) + 2 * sizeof(mem_block_header_t), false);
    return n;
}


/*
 * uinit - Used initialize metadata required to manage the heap
 * along with allocating initial memory.
 */
int uinit() {
    // Student TODO
    // initializes the free list bins
    int t = 16;
    for (int i = 0; i < BIN_COUNT; i++) {
        free_heads[i] = extend(t);
        if (!free_heads[i]) {
            return -1;
        }
        t *= 4;
    }
    return 0;
}

/*
 * umalloc -  allocates size bytes and returns a pointer to the allocated memory.
 */
void *umalloc(size_t size)
{
    // STUDENT TODO
    if (size == 0)
        return NULL;
    if (size % ALIGNMENT != 0) {
        size = ALIGN(size);
    }
    // finds a block that can satisfy the malloc request
    mem_block_header_t *block = find(size);
    if (!block) {
        return NULL;
    }
    // splits the block if it is too large
    block = split(block, size);
    allocate(block);
    return get_payload(block);
}

/**
 * @param ptr the pointer to the memory to be freed,
 * must have been called by a previous malloc call
 * @brief frees the memory space pointed to by ptr.
 */
void ufree(void *ptr) {
    // STUDENT TODO
    if (!ptr) 
        return;

    // gets the block from the pointer and deallocates it
    mem_block_header_t *block = get_header(ptr);
    if (!block || !is_allocated(block)) 
        return;

    deallocate(block); 

    // coalesces the block and adds it to the free list
    block = coalesce(block);
    add_to_free_list(block);
}