// project from Jamiliah Eubanks and Rabia Mamo

#include "BENSCHILLIBOWL.h"
#include <assert.h>
#include <stdlib.h>
#include <time.h>

// Function prototypes for queue operations
bool IsEmpty(BENSCHILLIBOWL* bcb);
bool IsFull(BENSCHILLIBOWL* bcb);
void AddOrderToBack(Order **orders, Order *order);

// Ben's Chili Bowl's legendary menu
MenuItem BENSCHILLIBOWLMenu[] = { 
    "BensChilli", 
    "BensHalfSmoke", 
    "BensHotDog", 
    "BensChilliCheeseFries", 
    "BensShake",
    "BensHotCakes",
    "BensCake",
    "BensHamburger",
    "BensVeggieBurger",
    "BensOnionRings",
};
int BENSCHILLIBOWLMenuLength = 10;

// Let fate decide what's for dinner
MenuItem PickRandomMenuItem() {
    int menu_index = rand() % BENSCHILLIBOWLMenuLength;
    return BENSCHILLIBOWLMenu[menu_index];
}

// Grand opening of Ben's Chili Bowl
BENSCHILLIBOWL* OpenRestaurant(int max_size, int expected_num_orders) {
    BENSCHILLIBOWL* bcb = (BENSCHILLIBOWL*)malloc(sizeof(BENSCHILLIBOWL));
    if (!bcb) {
        perror("Unable to process restaurant creation");
        exit(1);
    }
    // Initialize the chili bowl's properties
    bcb->orders = NULL;
    bcb->max_size = max_size;
    bcb->current_size = 0;
    bcb->next_order_number = 1;
    bcb->orders_handled = 0;
    bcb->expected_num_orders = expected_num_orders;

    // Set up the kitchen's synchronization tools
    pthread_mutex_init(&(bcb->mutex), NULL);
    pthread_cond_init(&(bcb->can_add_orders), NULL);
    pthread_cond_init(&(bcb->can_get_orders), NULL);

    printf("Restaurant is open and ready to serve!\n");
    return bcb;
}

// Time to close up shop
void CloseRestaurant(BENSCHILLIBOWL* bcb) {
    pthread_mutex_lock(&(bcb->mutex));
    // Check if we served everyone
    if (bcb->orders_handled != bcb->expected_num_orders) {
        printf("Warning: %d hungry customers left unserved.\n", 
               bcb->expected_num_orders - bcb->orders_handled);
    }
    pthread_mutex_unlock(&(bcb->mutex));

    // Clean up our synchronization tools
    pthread_mutex_destroy(&(bcb->mutex));
    pthread_cond_destroy(&(bcb->can_add_orders));
    pthread_cond_destroy(&(bcb->can_get_orders));
    free(bcb);

    printf("Restaurant is now closed. See you tomorrow for more chili!\n");
}

// Add a new chili order to the queue
int AddOrder(BENSCHILLIBOWL* bcb, Order* order) {
    pthread_mutex_lock(&(bcb->mutex));

    // Wait if the kitchen is overwhelmed
    while (IsFull(bcb)) {
        pthread_cond_wait(&(bcb->can_add_orders), &(bcb->mutex));
    }

    // Assign an order number and add to the queue
    order->order_number = bcb->next_order_number++;
    AddOrderToBack(&(bcb->orders), order);
    bcb->current_size++;

    // Signal that a new order is available
    pthread_cond_signal(&(bcb->can_get_orders));
    pthread_mutex_unlock(&(bcb->mutex));

    return order->order_number;
}

// Grab the next order from the queue
Order* GetOrder(BENSCHILLIBOWL* bcb) {
    pthread_mutex_lock(&(bcb->mutex));

    // Wait if there are no orders to prepare
    while (IsEmpty(bcb)) {
        if (bcb->orders_handled >= bcb->expected_num_orders) {
            pthread_mutex_unlock(&(bcb->mutex));
            return NULL;  // No more orders expected
        }
        pthread_cond_wait(&(bcb->can_get_orders), &(bcb->mutex));
    }

    // Get the next order and update the queue
    Order* order = bcb->orders;
    bcb->orders = bcb->orders->next;
    bcb->current_size--;
    bcb->orders_handled++;

    // Signal that there's space for new orders
    pthread_cond_signal(&(bcb->can_add_orders));
    pthread_mutex_unlock(&(bcb->mutex));

    return order;
}

// Check if the order queue is empty
bool IsEmpty(BENSCHILLIBOWL* bcb) {
    return bcb->current_size == 0;
}

// Check if the order queue is full
bool IsFull(BENSCHILLIBOWL* bcb) {
    return bcb->current_size == bcb->max_size;
}

// Add a new order to the end of the queue
void AddOrderToBack(Order **orders, Order *order) {
    if (*orders == NULL) {
        *orders = order;
    } else {
        Order *current = *orders;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = order;
    }
    order->next = NULL;  // Ensure the new order points to NULL
}