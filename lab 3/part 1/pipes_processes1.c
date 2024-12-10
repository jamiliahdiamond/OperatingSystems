// C program to demonstrate use of fork() and pipe() 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

int main()
{
    int fd1[2];  // Parent to Child pipe
    int fd2[2];  // Child to Parent pipe
    char fixed_str1[] = "howard.edu";
    char fixed_str2[] = "gobison.org";
    char input_str[100];
    pid_t p;

    if (pipe(fd1) == -1 || pipe(fd2) == -1)
    {
        fprintf(stderr, "Pipe Failed");
        return 1;
    }

    p = fork();

    if (p < 0)
    {
        fprintf(stderr, "fork Failed");
        return 1;
    }
    else if (p > 0)  // Parent process
    {
        close(fd1[0]);  // Close reading end of Parent to Child pipe
        close(fd2[1]);  // Close writing end of Child to Parent pipe

        printf("Enter a string to concatenate: ");
        scanf("%s", input_str);

        // Write input string to Child
        write(fd1[1], input_str, strlen(input_str) + 1);

        // Read concatenated string from Child
        char concat_str[200];
        read(fd2[0], concat_str, 200);
        printf("Concatenated string: %s\n", concat_str);

        // Prompt for second input
        printf("Input: ");
        scanf("%s", input_str);

        // Write second input to Child
        write(fd1[1], input_str, strlen(input_str) + 1);

        // Read final concatenated string from Child
        read(fd2[0], concat_str, 200);
        printf("Output: %s%s\n", concat_str, fixed_str2);

        close(fd1[1]);
        close(fd2[0]);
    }
    else  // Child process
    {
        close(fd1[1]);  // Close writing end of Parent to Child pipe
        close(fd2[0]);  // Close reading end of Child to Parent pipe

        char concat_str[200];
        
        // First concatenation
        read(fd1[0], concat_str, 100);
        strcat(concat_str, fixed_str1);
        write(fd2[1], concat_str, strlen(concat_str) + 1);

        // Second concatenation
        read(fd1[0], input_str, 100);
        strcat(concat_str, input_str);
        write(fd2[1], concat_str, strlen(concat_str) + 1);

        close(fd1[0]);
        close(fd2[1]);
        exit(0);
    }

    return 0;
}