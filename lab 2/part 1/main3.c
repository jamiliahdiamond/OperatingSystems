#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
// <Author, Jamiliah Eubanks>
// Child process behavior
void childProcess() {
    int iterations = random() % 30 + 1;  // Random iterations: 1 to 30
    int i;

    for (i = 0; i < iterations; i++) {
        printf("Child Pid: %d is going to sleep!\n", getpid());

        int sleepTime = random() % 10 + 1;  // Random sleep: 1 to 10 seconds
        sleep(sleepTime);

        printf("Child Pid: %d is awake! Where is my Parent: %d?\n", getpid(), getppid());
    }
}

int main() {
    pid_t pid1, pid2;   // Process IDs for two children
    int status;         // Status for child process waiting

    // Initialize random seed
    srandom(time(NULL));

    // Create first child
    pid1 = fork();
    if (pid1 == 0) {
        childProcess();
        exit(0);
    }

    // Create second child
    pid2 = fork();
    if (pid2 == 0) {
        childProcess();
        exit(0);
    }

    // Parent waits for children
    waitpid(pid1, &status, 0);
    printf("Child Pid: %d has completed\n", pid1);

    waitpid(pid2, &status, 0);
    printf("Child Pid: %d has completed\n", pid2);

    return 0;
}