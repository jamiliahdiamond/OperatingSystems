// project from Jamiliah Eubanks and Rabia Mamo

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include "BENSCHILLIBOWL.h"

// Restaurant configuration
#define BENSCHILLIBOWL_SIZE 100
#define NUM_CUSTOMERS 90
#define NUM_COOKS 10
#define ORDERS_PER_CUSTOMER 3
#define EXPECTED_NUM_ORDERS NUM_CUSTOMERS * ORDERS_PER_CUSTOMER

// The famous Ben's Chili Bowl
BENSCHILLIBOWL *bcb;

// Simulate a hungry customer
void* BENSCHILLIBOWLCustomer(void* tid) {
    int customer_id = *(int*)tid;
    free(tid);

    // Each customer places multiple orders
    for (int i = 0; i < ORDERS_PER_CUSTOMER; i++) {
        // Allocate space for an order
        Order* order = (Order*)malloc(sizeof(Order));
        if (!order) {
            perror("Failed to allocate memory for order");
            pthread_exit(NULL);
        }

        // Select a menu item and populate the order
        order->menu_item = PickRandomMenuItem();
        order->customer_id = customer_id;
        order->order_number = -1;  // Will be set by AddOrder
        order->next = NULL;

        // Add order to the restaurant
        int order_number = AddOrder(bcb, order);
        printf("Customer %d ordered %s (Order #%d)\n", customer_id, order->menu_item, order_number);

        // Wait a bit before next order
        usleep(rand() % 1000000);
    }

    pthread_exit(NULL);
}

// Simulate a hardworking cook
void* BENSCHILLIBOWLCook(void* tid) {
    int cook_id = *(int*)tid;
    free(tid);

    while (1) {
        // Get an order from the restaurant
        Order* order = GetOrder(bcb);
        if (!order) {
            // No more orders, time to go home
            break;
        }

        // Fulfill the order
        printf("Cook %d preparing %s for Customer %d (Order #%d)\n",
               cook_id, order->menu_item, order->customer_id, order->order_number);
        
        // Simulate cooking time
        usleep(rand() % 1000000);
        
        // Free the space taken by the order
        free(order);
    }

    pthread_exit(NULL);
}

int main() {
    srand(time(NULL));

    // Open the restaurant
    bcb = OpenRestaurant(BENSCHILLIBOWL_SIZE, EXPECTED_NUM_ORDERS);

    pthread_t customers[NUM_CUSTOMERS];
    pthread_t cooks[NUM_COOKS];

    // Create customer threads
    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        int* id = malloc(sizeof(int));
        *id = i + 1;
        if (pthread_create(&customers[i], NULL, BENSCHILLIBOWLCustomer, id) != 0) {
            perror("Failed to create customer thread");
            exit(1);
        }
    }

    // Create cook threads
    for (int i = 0; i < NUM_COOKS; i++) {
        int* id = malloc(sizeof(int));
        *id = i + 1;
        if (pthread_create(&cooks[i], NULL, BENSCHILLIBOWLCook, id) != 0) {
            perror("Failed to create cook thread");
            exit(1);
        }
    }

    // Wait for all customers to finish
    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        pthread_join(customers[i], NULL);
    }

    // Wait for all cooks to finish
    for (int i = 0; i < NUM_COOKS; i++) {
        pthread_join(cooks[i], NULL);
    }

    // Close the restaurant
    CloseRestaurant(bcb);
    return 0;
}