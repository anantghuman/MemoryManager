#include "umalloc.h"
#include "csbrk.h"
#include <stdio.h>
#include <assert.h>
#include "ansicolors.h"

const char author[] = ANSI_BOLD ANSI_COLOR_RED "REPLACE THIS WITH YOUR NAME AND UT EID" ANSI_RESET;

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
    // Student TODO (REQUIRED)
    return NULL;
}

/*
 * block_metadata - returns true if a block is marked as allocated.
 */
bool is_allocated(mem_block_header_t *block) {
    // Student TODO
    return 0;
}

/*
 * allocate - marks a block as allocated.
 */
void allocate(mem_block_header_t *block) {
    // Student TODO
}


/*
 * deallocate - marks a block as unallocated.
 */
void deallocate(mem_block_header_t *block) {
    // Student TODO
}

/*
 * get_size - gets the size of the block.
 */
size_t get_size(mem_block_header_t *block) {
    // Student TODO
    return 0;
}

/*
 * get_next - gets the next block.
 */
mem_block_header_t *get_next(mem_block_header_t *block) {
    // Student TODO
    return NULL;
}

/*
 * set_block_metadata
 * Optional helper method that can be used to initialize the fields for the 
 * memory block struct. 
 */
void set_block_metadata(mem_block_header_t *block, size_t size, bool alloc) {
    // Optional student todo
}

/*
 * get_payload - gets the payload of the block.
 */
void *get_payload(mem_block_header_t *block) {
    // Student TODO
    return NULL;
}

/*
 * get_header - given a payload, returns the block.
 */
mem_block_header_t *get_header(void *payload) {
    // Student TODO
    return NULL;
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
    return NULL;
}

/*
 * extend - extends the heap if more memory is required.
 */
mem_block_header_t *extend(size_t size) {
    // Student TODO
    return NULL;
}

/*
 * split - splits a given block in parts, one allocated, one free.
 */
mem_block_header_t *split(mem_block_header_t *block, size_t new_block_size) {
    // Student TODO
    return NULL;
}

/*
 * coalesce - coalesces a free memory block with neighbors.
 */
mem_block_header_t *coalesce(mem_block_header_t *block) {
    // Student TODO
    return NULL;
}


/*
 * uinit - Used initialize metadata required to manage the heap
 * along with allocating initial memory.
 */
int uinit() {
    // Student TODO
    return 0;
}

/*
 * umalloc -  allocates size bytes and returns a pointer to the allocated memory.
 */
void *umalloc(size_t size)
{
    // STUDENT TODO
	return NULL;
}

/**
 * @param ptr the pointer to the memory to be freed,
 * must have been called by a previous malloc call
 * @brief frees the memory space pointed to by ptr.
 */
void ufree(void *ptr)
{
    // STUDENT TODO
}