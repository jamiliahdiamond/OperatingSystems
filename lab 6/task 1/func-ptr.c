#include <stdio.h>
#include <stdlib.h>

// Define maximum number of processes
#define MAX_PROCESSES 100

// Structure to represent a process
typedef struct {
    int pid;           // Process ID
    int arrival_time;  // Time when the process arrives
    int priority;      // Priority of the process (higher number = higher priority)
} Process;

// Comparison function for sorting processes by arrival time (ascending order)
int compare_arrival_time(const void* a, const void* b) {
    Process* p1 = (Process*)a;
    Process* p2 = (Process*)b;
    return p1->arrival_time - p2->arrival_time;
}

// Comparison function for sorting processes by priority (descending order)
// If priorities are equal, sort by arrival time (ascending order)
int compare_priority(const void* a, const void* b) {
    Process* p1 = (Process*)a;
    Process* p2 = (Process*)b;
    
    // First, compare priority in descending order
    if (p1->priority != p2->priority)
        return p2->priority - p1->priority;
    
    // If priorities are equal, compare arrival time in ascending order
    return p1->arrival_time - p2->arrival_time;
}

int main() {
    FILE* file = fopen("process.txt", "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    Process processes[MAX_PROCESSES];
    int count = 0;

    // Read processes from file
    while (fscanf(file, "%d %d %d", 
        &processes[count].pid, 
        &processes[count].arrival_time, 
        &processes[count].priority) == 3 && count < MAX_PROCESSES) {
        count++;
    }
    fclose(file);

    // Sort and display processes by arrival time
    printf("Sorted by Arrival Time (Ascending):\n");
    printf("PID Arrival Priority\n");
    printf("--- ------- --------\n");
    qsort(processes, count, sizeof(Process), compare_arrival_time);
    
    for (int i = 0; i < count; i++) {
        printf("%3d %7d %8d\n", 
            processes[i].pid, 
            processes[i].arrival_time, 
            processes[i].priority);
    }

    // Sort and display processes by priority
    printf("\nSorted by Priority (Descending, then by Arrival Time):\n");
    printf("PID Arrival Priority\n");
    printf("--- ------- --------\n");
    qsort(processes, count, sizeof(Process), compare_priority);
    
    for (int i = 0; i < count; i++) {
        printf("%3d %7d %8d\n", 
            processes[i].pid, 
            processes[i].arrival_time, 
            processes[i].priority);
    }

    return 0;
}