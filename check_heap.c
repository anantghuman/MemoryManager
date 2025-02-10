#include "check_heap.h"
#include <stdio.h>
#include <stdint.h>

extern mem_block_header_t *free_head;
extern mem_block_header_t *free_heads[BIN_COUNT];

int check_heap() {
    for (int i = 0; i < BIN_COUNT; ++i) {
        int result = check_bin(free_heads[i]);
        if (result) return result;
    }
    return HEAP_SUCCESS;
}

/*
 * STUDENT TODO: set these variables according to your heap design (tests will fail if you do not
 * set these variables!)
 *    - order:      how is your free list ordered?
 *    - circular:   true if your free list is circular; false otherwise
 */
heap_order order = ORD_MEM;
bool circular = false;

/*
 * check_bin -  used to check that the heap is still in a consistent state.
 * 
 * STUDENT TODO: this function is required to be completed for checkpoint 1
 * 
 *      - Ensure that the free block list is in the order you expect it to be in
 *        (if your list is randomly ordered, this check is not required).
 * 
 *      - Check if any free blocks overlap with each other. 
 * 
 *      - Ensure that each free block is aligned.
 * 
 *      - Ensure that all blocks on the free list are free (no implicit free lists)
 *
 * Should return HEAP_SUCCESS if the heap is consistent or HEAP_FAILURE if an error 
 * is detected.
 */
int check_bin(mem_block_header_t *free_head) {
    mem_block_header_t* temp = free_head;
    void* prev_address = NULL;
    uint64_t prev_size = 0;
    while (temp != NULL) {
        size_t t = temp->block_metadata;
        if ((t & 1) != 0)
            return HEAP_FAILURE;
        if (temp < prev_address)
            return HEAP_FAILURE;
        if ((uint8_t)temp % 16 != 0)
            return HEAP_FAILURE;
        if (prev_address != NULL && (prev_address + prev_size) >= (uint8_t)temp)
            return HEAP_FAILURE;

        prev_size = 0;
        prev_size = (t >> 4) & 0xFFFFFFFFFFFFFFF;
        prev_address = (uint8_t*)temp;
        temp = temp->next;
    }
    return HEAP_SUCCESS;
}