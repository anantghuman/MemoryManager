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
mem_block_header_t* head;

/*
 * select_bin - selects a free list bin to use based on the 
 * block size.
 */
mem_block_header_t* select_bin(size_t size) {
    // Student TODO (REQUIRED)
    if(size <= 16) {
        if (free_heads[0] == NULL) {
            return extend(size + 16);
        }
        mem_block_header_t *block = free_heads[0];
        allocate(block);
        free_heads[0] = free_heads[0]->next;
        block->next = NULL;
        return block;
    } else if(size <= 64) {
        if (free_heads[1] == NULL) {
            return extend(size + 16);
        }
        mem_block_header_t *block = free_heads[1];
        allocate(block);
        free_heads[1] = free_heads[1]->next;
        block->next = NULL;
        return block;
    } else if (size <= 512) {
        if (free_heads[2] == NULL) {
            return extend(size + 16);
        }
        mem_block_header_t *block = free_heads[2];
        allocate(block);
        free_heads[2] = free_heads[2]->next;
        block->next = NULL;
        return block;
    } else {
        if (free_heads[3] == NULL) {
            return extend(size + 16);
        }
        mem_block_header_t *block = free_heads[3];
        allocate(block);
        free_heads[3] = free_heads[3]->next;
        block->next = NULL;
        return block;
    }
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
    return select_bin(payload_size);
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
    return NULL;
    
}

/*
 * coalesce - coalesces a free memory block with neighbors.
 */
mem_block_header_t *coalesce(mem_block_header_t *block) {
    // Student TODO
    if (!block)
        return NULL;
    // mem_block_header_t *next_block = get_next(block);
    // while (next_block && !is_allocated(next_block)) {
    //     block->block_metadata += get_size(next_block) + sizeof(mem_block_header_t);
    //     block->next = get_next(next_block);
    //     next_block = get_next(block);
    // }
    return block;
}


/*
 * uinit - Used initialize metadata required to manage the heap
 * along with allocating initial memory.
 */
int uinit() {
    // Student TODO
    for (int i = 0; i < BIN_COUNT; i++) {
        free_heads[i] = NULL;
    }
    void *initial_heap = extend(PAGESIZE);
    if (initial_heap == (void *) -1) {
        return -1;
    }
    mem_block_header_t *block = (mem_block_header_t *) initial_heap;
    block->block_metadata = PAGESIZE - sizeof(mem_block_header_t);
    block->next = NULL;

    free_heads[3] = block;
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
void ufree(void *ptr)
{
    // STUDENT TODO
    if (!ptr)
        return;

    mem_block_header_t *block = get_header(ptr);
    if (!block)
        return;
    if (!is_allocated(block))
        return;
    deallocate(block);

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
    mem_block_header_t* temp = free_heads[bin_index];
    if (temp != NULL) {
        free_heads[bin_index] = block;
        return;
    }
    if (temp < block) {
        block->next = free_heads[bin_index];
        free_heads[bin_index] = block;
        return;
    }
    mem_block_header_t* prev = NULL;
    while (temp != NULL && temp > block) {
        prev = temp;
        temp = temp->next;
    }
    prev->next = block;
}