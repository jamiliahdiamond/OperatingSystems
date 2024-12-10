// Jamiliah Eubanks

#include <stdio.h>
#include <stdlib.h>

// Process structure to hold information about each process
typedef struct {
    int pid;           // Process ID
    int arrival_time;  // Time when process arrives
    int priority;      // Priority level of the process
} Process;

// Comparison function for qsort to order processes by arrival time
int compare_arrival_time(const void* a, const void* b) {
    Process* p1 = (Process*)a;
    Process* p2 = (Process*)b;
    return p1->arrival_time - p2->arrival_time;
}

// Comparison function for qsort to order processes by priority
// If priorities are equal, arrival time is used as a tiebreaker
int compare_priority(const void* a, const void* b) {
    Process* p1 = (Process*)a;
    Process* p2 = (Process*)b;
    
    // Compare priorities (higher priority first)
    if (p1->priority != p2->priority)
        return p2->priority - p1->priority;
    
    // If priorities are equal, compare arrival times
    return p1->arrival_time - p2->arrival_time;
}

int main() {
    // Open the input file
    FILE* file = fopen("process.txt", "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    Process processes[100];  // Array to store up to 100 processes
    int count = 0;

    // Read process data from file
    while (fscanf(file, "%d %d %d", 
        &processes[count].pid, 
        &processes[count].arrival_time, 
        &processes[count].priority) == 3) {
        count++;
    }
    fclose(file);

    // Sort and display processes by arrival time
    printf("Sorted by Arrival Time (Ascending):\n");
    qsort(processes, count, sizeof(Process), compare_arrival_time);
    
    for (int i = 0; i < count; i++) {
        printf("%d %d %d\n", 
            processes[i].pid, 
            processes[i].arrival_time, 
            processes[i].priority);
    }

    // Sort and display processes by priority
    printf("\nSorted by Priority (Descending):\n");
    qsort(processes, count, sizeof(Process), compare_priority);
    
    for (int i = 0; i < count; i++) {
        printf("%d %d %d\n", 
            processes[i].pid, 
            processes[i].arrival_time, 
            processes[i].priority);
    }

    return 0;
}