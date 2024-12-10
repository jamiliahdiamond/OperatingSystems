#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "util.h"

// Allocate memory based on the specified policy (First Fit, Best Fit, or Worst Fit)
void allocate_memory(list_t *freelist, list_t *alloclist, int pid, int blocksize, int policy) {
    node_t *current = freelist->head;
    node_t *previous = NULL, *chosen_previous = NULL;
    node_t *chosen_node = NULL;
    block_t *blk = NULL;
    int min_size_difference = INT_MAX;  // For Best Fit
    int max_size = 0;                   // For Worst Fit

    // Traverse the free list to find a suitable block based on the allocation policy
    while (current != NULL) {
        int block_size = current->blk->end - current->blk->start + 1;
        int size_difference = block_size - blocksize;

        if (block_size >= blocksize) {
            if (policy == FIRST_FIT) {
                chosen_node = current;
                chosen_previous = previous;
                break;
            } else if (policy == BEST_FIT && size_difference < min_size_difference) {
                min_size_difference = size_difference;
                chosen_node = current;
                chosen_previous = previous;
            } else if (policy == WORST_FIT && block_size > max_size) {
                max_size = block_size;
                chosen_node = current;
                chosen_previous = previous;
            }
        }
        previous = current;
        current = current->next;
    }

    // Handle case when no suitable block is found
    if (chosen_node == NULL) {
        printf("Error: Not Enough Memory\n");
        return;
    }

    blk = chosen_node->blk;
    // Remove the chosen block from the Free List
    if (chosen_previous == NULL) {
        freelist->head = chosen_node->next;
    } else {
        chosen_previous->next = chosen_node->next;
    }

    // Adjust the block size and update PID
    int original_end = blk->end;
    blk->end = blk->start + blocksize - 1;
    blk->pid = pid;

    // Add the allocated block to the Allocated List
    list_add_ascending_by_address(alloclist, blk);

    // Handle fragmentation if necessary
    if (original_end > blk->end) {
        block_t *fragment = malloc(sizeof(block_t));
        if (!fragment) {
            printf("Error: Memory Allocation Failure\n");
            return;
        }
        fragment->pid = 0;
        fragment->start = blk->end + 1;
        fragment->end = original_end;

        // Add the fragment back to the Free List based on the allocation policy
        if (policy == FIRST_FIT || policy == BEST_FIT) {
            list_add_to_back(freelist, fragment);
        } else if (policy == WORST_FIT) {
            list_add_to_back(freelist, fragment);
        }
    }
}

// Deallocate memory for a given process ID and update free list based on policy
void deallocate_memory(list_t *alloclist, list_t *freelist, int pid, int policy) {
    // Find the allocated block with the given PID
    node_t *current = alloclist->head;
    block_t *blk = NULL;

    while (current != NULL) {
        if (current->blk->pid == pid) {
            blk = current->blk;
            break;
        }
        current = current->next;
    }

    // Handle case when the block is not found
    if (blk == NULL) {
        printf("Error: Can't locate Memory Used by PID: %d\n", pid);
        return;
    }

    // Remove the block from the Allocated List
    list_remove_node(alloclist, current);

    // Reset the block's PID
    blk->pid = 0;

    // Add the block back to the Free List based on the allocation policy
    switch (policy) {
        case FIRST_FIT:
            list_add_ascending_by_address(freelist, blk);
            break;
        case BEST_FIT:
            list_add_ascending_by_address(freelist, blk);
            break;
        case WORST_FIT:
            list_add_descending_by_blocksize(freelist, blk);
            break;
        default:
            // Handle other policies if any
            break;
    }
}

// Merge adjacent free memory blocks to reduce fragmentation
void coalesce_memory(list_t *freelist) {
    if (freelist == NULL || freelist->head == NULL) {
        return; // No blocks to coalesce
    }

    // Sort the free list by starting addresses
    sort_freelist_by_address(freelist);

    node_t *current = freelist->head;
    while (current != NULL && current->next != NULL) {
        block_t *current_block = current->blk;
        block_t *next_block = current->next->blk;

        // Check if the current block is adjacent to the next block
        if (current_block->end + 1 == next_block->start) {
            // Merge current and next block
            current_block->end = next_block->end;

            // Remove the next block from the list
            node_t *temp = current->next;
            current->next = temp->next;
            free(temp->blk);
            free(temp);
        } else {
            current = current->next;
        }
    }
}