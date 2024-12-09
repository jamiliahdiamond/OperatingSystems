// Project by Jamiliah Eubanks and Rabia Mamo

#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>
#include <sys/wait.h>
#include <errno.h>

#define SEM_NAME "/bank_account_sem"

// Function to generate a random number between min and max (inclusive)
int random_range(int min, int max) {
    return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

void DearOldDad(sem_t *mutex, int *counter_ptr) {
    while (1) {
        sleep(random_range(0, 5)); // Sleep for 0-5 seconds
        printf("Dear Old Dad: Attempting to Check Balance\n");

        if (sem_wait(mutex) == -1) {
            perror("sem_wait failed");
            exit(1);
        }

        int localBalance = *counter_ptr;
        if (rand() % 2 == 0) { // Even number: try to deposit
            if (localBalance < 100) {
                int amount = random_range(100, 200); // Random amount between 100-200
                if (rand() % 2 == 0) { // 50% chance to deposit
                    localBalance += amount;
                    *counter_ptr = localBalance;
                    printf("Dear old Dad: Deposits $%d / Balance = $%d\n", amount, localBalance);
                } else {
                    printf("Dear old Dad: Doesn't have any money to give\n");
                }
            } else {
                printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", localBalance);
            }
        } else { // Odd number: just check balance
            printf("Dear Old Dad: Last Checking Balance = $%d\n", localBalance);
        }

        if (sem_post(mutex) == -1) {
            perror("sem_post failed");
            exit(1);
        }
    }
}

void PoorStudent(sem_t *mutex, int *counter_ptr) {
    while (1) {
        sleep(random_range(0, 5)); // Sleep for 0-5 seconds
        printf("Poor Student: Attempting to Check Balance\n");

        if (sem_wait(mutex) == -1) {
            perror("sem_wait failed");
            exit(1);
        }

        int localBalance = *counter_ptr;
        if (rand() % 2 == 0) { // Even number: try to withdraw
            int need = random_range(1, 50); // Random amount between 1-50
            printf("Poor Student needs $%d\n", need);
            if (need <= localBalance) {
                localBalance -= need;
                *counter_ptr = localBalance;
                printf("Poor Student: Withdraws $%d / Balance = $%d\n", need, localBalance);
            } else {
                printf("Poor Student: Not Enough Cash ($%d)\n", localBalance);
            }
        } else { // Odd number: just check balance
            printf("Poor Student: Last Checking Balance = $%d\n", localBalance);
        }

        if (sem_post(mutex) == -1) {
            perror("sem_post failed");
            exit(1);
        }
    }
}

int main(int argc, char **argv) {
    int fd, *counter_ptr;
    sem_t *mutex;

    // Seed the random number generator
    srand(time(NULL));

    // Create shared memory
    fd = open("log.txt", O_RDWR | O_CREAT, S_IRWXU);
    if (fd == -1) {
        perror("open failed");
        exit(1);
    }

    if (ftruncate(fd, sizeof(int)) == -1) {
        perror("ftruncate failed");
        exit(1);
    }

    counter_ptr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (counter_ptr == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }

    close(fd);

    // Create and initialize semaphore
    mutex = sem_open(SEM_NAME, O_CREAT, 0644, 1);
    if (mutex == SEM_FAILED) {
        perror("sem_open failed");
        exit(1);
    }

    // Create child process (Poor Student)
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        exit(1);
    } else if (pid == 0) {
        // Child process
        PoorStudent(mutex, counter_ptr);
        exit(0);
    } else {
        // Parent process
        DearOldDad(mutex, counter_ptr);

        // Wait for child to finish (which it never will in this case)
        wait(NULL);
    }

    // Cleanup (this part will never be reached in the current implementation)
    sem_close(mutex);
    sem_unlink(SEM_NAME);
    munmap(counter_ptr, sizeof(int));

    return 0;
}