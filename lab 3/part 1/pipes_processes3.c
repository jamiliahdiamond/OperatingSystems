#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <grep_argument>\n", argv[0]);
        exit(1);
    }

    int pipefd1[2], pipefd2[2];
    pid_t pid1, pid2;

    char *cat_args[] = {"cat", "scores", NULL};
    char *grep_args[] = {"grep", argv[1], NULL};
    char *sort_args[] = {"sort", NULL};

    // Create first pipe
    if (pipe(pipefd1) == -1) {
        perror("pipe");
        exit(1);
    }

    // First fork (parent-child)
    pid1 = fork();

    if (pid1 == -1) {
        perror("fork");
        exit(1);
    } else if (pid1 == 0) {
        // Child process (P2 - grep)

        // Create second pipe
        if (pipe(pipefd2) == -1) {
            perror("pipe");
            exit(1);
        }

        // Second fork (child-grandchild)
        pid2 = fork();

        if (pid2 == -1) {
            perror("fork");
            exit(1);
        } else if (pid2 == 0) {
            // Grandchild process (P3 - sort)

            // Replace standard input with input part of second pipe
            dup2(pipefd2[0], STDIN_FILENO);

            // Close unused half of second pipe
            close(pipefd2[1]);

            // Close unused ends of first pipe
            close(pipefd1[0]);
            close(pipefd1[1]);

            // Execute sort
            execvp("sort", sort_args);
            perror("execvp");
            exit(1);
        } else {
            // Child process (P2 - grep)

            // Replace standard input with input part of first pipe
            dup2(pipefd1[0], STDIN_FILENO);

            // Replace standard output with output part of second pipe
            dup2(pipefd2[1], STDOUT_FILENO);

            // Close unused halves of pipes
            close(pipefd1[1]);
            close(pipefd2[0]);

            // Execute grep
            execvp("grep", grep_args);
            perror("execvp");
            exit(1);
        }
    } else {
        // Parent process (P1 - cat)

        // Replace standard output with output part of first pipe
        dup2(pipefd1[1], STDOUT_FILENO);

        // Close unused input half of first pipe
        close(pipefd1[0]);

        // Execute cat
        execvp("cat", cat_args);
        perror("execvp");
        exit(1);
    }

    return 0;
}