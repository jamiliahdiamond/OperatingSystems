// C program for implementation of Simulation 
// Jamiliah Eubanks
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include "process.h"
#include "util.h"

// Function to find the waiting time for all processes using Round Robin scheduling
void findWaitingTimeRR(ProcessType plist[], int n, int quantum) 
{
    int *bt_rem = (int *)malloc(n * sizeof(int));  // Remaining burst time
    int t = 0;  // Current time

    // Initialize remaining burst times
    for (int i = 0; i < n; i++)
        bt_rem[i] = plist[i].bt;

    while (1) {
        int done = 1;
        for (int i = 0; i < n; i++) {
            if (bt_rem[i] > 0) {
                done = 0;  // There is a pending process

                if (bt_rem[i] > quantum) {
                    t += quantum;
                    bt_rem[i] -= quantum;
                } else {
                    t += bt_rem[i];
                    plist[i].wt = t - plist[i].bt;
                    bt_rem[i] = 0;
                }
            }
        }
        if (done == 1)
            break;
    }

    free(bt_rem);  // Free allocated memory
}

// Function to find the waiting time for all processes using Shortest Job First scheduling
void findWaitingTimeSJF(ProcessType plist[], int n)
{
    int *bt_rem = (int *)malloc(n * sizeof(int));  // Remaining burst time
    int t = 0;  // Current time
    int completed = 0;  // Number of completed processes

    // Initialize remaining burst times
    for (int i = 0; i < n; i++)
        bt_rem[i] = plist[i].bt;

    while (completed < n) {
        int shortest = -1;
        int min_bt = INT_MAX;

        // Find process with minimum remaining time
        for (int i = 0; i < n; i++) {
            if (bt_rem[i] > 0 && bt_rem[i] < min_bt) {
                min_bt = bt_rem[i];
                shortest = i;
            }
        }

        if (shortest == -1) {
            t++;
            continue;
        }

        bt_rem[shortest]--;
        t++;

        if (bt_rem[shortest] == 0) {
            completed++;
            plist[shortest].wt = t - plist[shortest].bt;
        }
    }

    free(bt_rem);  // Free allocated memory
}

// Function to find the waiting time for all processes using First Come First Serve scheduling
void findWaitingTime(ProcessType plist[], int n)
{
    plist[0].wt = 0 + plist[0].art;

    for (int i = 1; i < n; i++)
        plist[i].wt = plist[i-1].bt + plist[i-1].wt;
}

// Function to calculate turn around time
void findTurnAroundTime(ProcessType plist[], int n)
{
    for (int i = 0; i < n; i++)
        plist[i].tat = plist[i].bt + plist[i].wt;
}

// Function to sort the Process acc. to priority
int my_comparer(const void *this, const void *that)
{
    ProcessType *p1 = (ProcessType *)this;
    ProcessType *p2 = (ProcessType *)that;

    if (p1->pri < p2->pri)
        return -1;
    else if (p1->pri > p2->pri)
        return 1;
    else
        return 0;
}

// Function to calculate average time for FCFS
void findavgTimeFCFS(ProcessType plist[], int n)
{
    findWaitingTime(plist, n);
    findTurnAroundTime(plist, n);
    printf("\n*********\nFCFS\n");
}

// Function to calculate average time for SJF
void findavgTimeSJF(ProcessType plist[], int n)
{
    findWaitingTimeSJF(plist, n);
    findTurnAroundTime(plist, n);
    printf("\n*********\nSJF\n");
}

// Function to calculate average time for RR
void findavgTimeRR(ProcessType plist[], int n, int quantum)
{
    findWaitingTimeRR(plist, n, quantum);
    findTurnAroundTime(plist, n);
    printf("\n*********\nRR Quantum = %d\n", quantum);
}

// Function to calculate average time for Priority scheduling
void findavgTimePriority(ProcessType plist[], int n)
{
    qsort(plist, n, sizeof(ProcessType), my_comparer);
    findWaitingTime(plist, n);
    findTurnAroundTime(plist, n);
    printf("\n*********\nPriority\n");
}

// Function to print metrics for the processes
void printMetrics(ProcessType plist[], int n)
{
    int total_wt = 0, total_tat = 0;
    float awt, att;

    printf("\tProcesses\tBurst time\tWaiting time\tTurn around time\n");

    for (int i = 0; i < n; i++) {
        total_wt += plist[i].wt;
        total_tat += plist[i].tat;
        printf("\t%d\t\t%d\t\t%d\t\t%d\n", plist[i].pid, plist[i].bt, plist[i].wt, plist[i].tat);
    }

    awt = ((float)total_wt / (float)n);
    att = ((float)total_tat / (float)n);

    printf("\nAverage waiting time = %.2f", awt);
    printf("\nAverage turn around time = %.2f\n", att);
}

// Function to initialize processes from file
ProcessType * initProc(char *filename, int *n)
{
    FILE *input_file = fopen(filename, "r");
    if (!input_file) {
        fprintf(stderr, "Error: Invalid filepath\n");
        fflush(stdout);
        exit(0);
    }

    ProcessType *plist = parse_file(input_file, n);

    fclose(input_file);

    return plist;
}

// Main function
int main(int argc, char *argv[])
{
    int n;
    int quantum = 2;
    ProcessType *proc_list;

    if (argc < 2) {
        fprintf(stderr, "Usage: ./schedsim <input-file-path>\n");
        fflush(stdout);
        return 1;
    }

    // FCFS
    n = 0;
    proc_list = initProc(argv[1], &n);
    findavgTimeFCFS(proc_list, n);
    printMetrics(proc_list, n);
    free(proc_list);

    // SJF
    n = 0;
    proc_list = initProc(argv[1], &n);
    findavgTimeSJF(proc_list, n);
    printMetrics(proc_list, n);
    free(proc_list);

    // Priority
    n = 0;
    proc_list = initProc(argv[1], &n);
    findavgTimePriority(proc_list, n);
    printMetrics(proc_list, n);
    free(proc_list);

    // RR
    n = 0;
    proc_list = initProc(argv[1], &n);
    findavgTimeRR(proc_list, n, quantum);
    printMetrics(proc_list, n);
    free(proc_list);

    return 0;
}