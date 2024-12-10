// list/list.c
// Implementation for a linked list specifically designed for memory block management

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

// Allocate memory for a new list and initialize it
list_t *list_alloc() {
    list_t* list = (list_t*)malloc(sizeof(list_t));
    list->head = NULL;
    return list;
}

// Allocate memory for a new node and initialize it with the given block
node_t *node_alloc(block_t *blk) {
    node_t* node = malloc(sizeof(node_t));
    node->next = NULL;
    node->blk = blk;
    return node;
}

// Free the memory allocated for the list
void list_free(list_t *l){
    free(l);
    return;
}

// Free the memory allocated for a node
void node_free(node_t *node){
    free(node);
    return;
}

// Print the contents of the list
void list_print(list_t *l) {
    node_t *current = l->head;
    block_t *b;
    if (current == NULL){
        printf("list is empty\n");
    }
    while (current != NULL){
        b = current->blk;