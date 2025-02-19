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
    int index = (size <= 16) ? 0
                : (size <= 64) ? 1
                : (size <= 512) ? 2
                : 3;

    mem_block_header_t *prev = NULL;
    mem_block_header_t *curr = free_heads[index];
    while (curr && get_size(curr) < size) { 
        prev = curr;
        curr = curr->next;
    }

    if (curr) {
        if (get_size(curr) >= size + sizeof(mem_block_header_t) + ALIGNMENT) {
            curr = split(curr, size);
        }
        allocate(curr);
        if (prev) {
            prev->next = curr->next;
        } else {
            free_heads[index] = curr->next;
        }
        curr->next = NULL;
        return curr;
    }

    /* Search for a block in larger bins */
    for (int i = index + 1; i < BIN_COUNT; i++) {
        prev = NULL;
        curr = free_heads[i];
        while (curr && get_size(curr) < size) {
            prev = curr;
            curr = curr->next;
        }
        if (curr) {
            if (get_size(curr) >= size + sizeof(mem_block_header_t) + ALIGNMENT) {
                curr = split(curr, size);
            }
            allocate(curr);
            if (prev) {
                prev->next = curr->next;
            } else {
                free_heads[i] = curr->next;
            }
            curr->next = NULL;
            return curr;
        }
    }
    
    /* No valid block found, extend memory */
    mem_block_header_t *new_block = extend(size + sizeof(mem_block_header_t));
    if (!new_block) 
        return NULL;
    return new_block;
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
    return (block->block_metadata & ~0xF) >> 4;
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
    mem_block_header_t *block = select_bin(payload_size);
    if (block && get_size(block) >= payload_size + sizeof(mem_block_header_t) + ALIGNMENT) {
        block = split(block, payload_size);
    }
    return block;
}

/*
 * extend - extends the heap if more memory is required.
 */
mem_block_header_t *extend(size_t size) {
    // Student TODO
    if (size == 0)
        return NULL;
    mem_block_header_t *block = (mem_block_header_t *)csbrk(size);
    if (block == (void *)-1) {
        return NULL;
    }
    set_block_metadata(block, size, false);
    block->next = NULL;
    return block;
}

/*
 * split - splits a given block in parts, one allocated, one free.
 */
mem_block_header_t *split(mem_block_header_t *block, size_t new_block_size) {
    // Student TODO
    if (!block || new_block_size == 0)
        return NULL;
    if (new_block_size % 16 != 0)
        new_block_size = ALIGN(new_block_size);
    size_t total_size = get_size(block);
    if (total_size - new_block_size < sizeof(mem_block_header_t) + ALIGNMENT)
        return NULL;
    mem_block_header_t *split = (mem_block_header_t *)((char *)block + sizeof(mem_block_header_t) + new_block_size);
    set_block_metadata(split, total_size - new_block_size - sizeof(mem_block_header_t), false);
    set_block_metadata(block, new_block_size, true);
    int bin_index;
    size_t split_size = get_size(split);
    if (split_size <= 16)
        bin_index = 0;
    else if (split_size <= 64)
        bin_index = 1;
    else if (split_size <= 512)
        bin_index = 2;
    else
        bin_index = 3;

    mem_block_header_t *prev = NULL;
    mem_block_header_t *curr = free_heads[bin_index];
    
    while (curr && (curr < split)) { 
        prev = curr;
        curr = curr->next;
    }
    
        // Insert the block in the correct position based on address
    split->next = curr;
    if (prev) {
        prev->next = split;
    } else {
        free_heads[bin_index] = split;
    }
    
    return block;
}

/*
 * coalesce - coalesces a free memory block with neighbors.
 */
mem_block_header_t *coalesce(mem_block_header_t *block) {
    // Student TODO
    if (!block)
        return NULL;

    mem_block_header_t *next_block = get_next(block);
    while (next_block && !is_allocated(next_block)) {
        size_t total_size = get_size(block) + sizeof(mem_block_header_t) + get_size(next_block);
        set_block_metadata(block, total_size, false);
        block->next = get_next(next_block); // Update the next pointer.
        next_block = get_next(block);
    }

    return block;
}


/*
 * uinit - Used initialize metadata required to manage the heap
 * along with allocating initial memory.
 */
int uinit() {
    // Student TODO
    int arr[BIN_COUNT] = {16, 64, 512, 2048};
    int t = 16;
    for (int i = 0; i < BIN_COUNT; i++) {
        free_heads[i] = extend(arr[i]);
        if (free_heads[i] == (void *) -1) {
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
    mem_block_header_t *block = find(size);
    if (!block)
        return NULL;
    return get_payload(block);
}

/**
 * @param ptr the pointer to the memory to be freed,
 * must have been called by a previous malloc call
 * @brief frees the memory space pointed to by ptr.
 */
void ufree(void *ptr) {
    if (!ptr) 
        return;

    mem_block_header_t *block = get_header(ptr);
    if (!block || !is_allocated(block)) 
        return;

    deallocate(block); 

    block = coalesce(block);

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

    mem_block_header_t *prev = NULL;
    mem_block_header_t *curr = free_heads[bin_index];

    while (curr && curr < block) {
        prev = curr;
        curr = curr->next;
    }

    block->next = curr;
    if (prev) {
        prev->next = block; 
    } else {
        free_heads[bin_index] = block;
    }
}