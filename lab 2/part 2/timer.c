#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

int signal_received = 0;  // Global variable to track if the signal was received
int alarm_count = 0;      // Counter for number of alarms
time_t start_time;        // Start time of the program

// SIGALRM handler function
void alarm_handler(int signum) {
    printf("Hello World!\n");
    signal_received = 1;  // Set the flag when signal is received
    alarm_count++;        // Increment alarm counter
}

// SIGINT handler function (for CTRL-C)
void interrupt_handler(int signum) {
    time_t end_time = time(NULL);
    double execution_time = difftime(end_time, start_time);
    
    printf("\nProgram interrupted.\n");
    printf("Number of alarms: %d\n", alarm_count);
    printf("Execution time: %.2f seconds\n", execution_time);
    
    exit(0);
}

int main(int argc, char *argv[]) {
    signal(SIGALRM, alarm_handler);   // Register the SIGALRM handler
    signal(SIGINT, interrupt_handler);  // Register the SIGINT handler

    start_time = time(NULL);  // Record the start time

    while (1) {
        alarm(1);  // Schedule a SIGALRM every 1 second

        // Wait for the signal
        while (!signal_received) {
            // Busy wait for the signal to be delivered
        }

        // After receiving the signal
        printf("Turing was right!\n");

        // Reset the signal_received flag for the next iteration
        signal_received = 0;
    }

    return 0;  // This line will never be reached
}