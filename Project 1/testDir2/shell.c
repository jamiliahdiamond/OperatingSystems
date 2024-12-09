#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>

#define MAX_CMD_LEN 1024
#define MAX_ARGS 128

// Structure to hold command information
typedef struct {
    char *args[MAX_ARGS];
    int argc;
    bool background;
} Command;

extern char **environ;
volatile sig_atomic_t foreground_pid = -1;

// Function prototypes
void display_prompt(void);
void parse_input(char *input, Command *cmd);
bool execute_builtin(Command *cmd);
void execute_external(Command *cmd);
void handle_sigint(int signum);
void handle_sigalrm(int signum);

int main(void) {
    char input[MAX_CMD_LEN];
    Command cmd;
    
    // Set up signal handlers
    signal(SIGINT, handle_sigint);
    signal(SIGALRM, handle_sigalrm);

    while (true) {
        display_prompt();
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n");
            break;
        }
        
        parse_input(input, &cmd);
        if (cmd.argc == 0) continue;

        // Execute built-in or external command
        if (!execute_builtin(&cmd)) {
            execute_external(&cmd);
        }
    }

    return 0;
}

// Display the shell prompt with current working directory
void display_prompt(void) {
    char cwd[MAX_CMD_LEN];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s> ", cwd);
    } else {
        printf("> ");
    }
    fflush(stdout);
}

// Parse input string into Command structure
void parse_input(char *input, Command *cmd) {
    cmd->argc = 0;
    cmd->background = false;

    char *token = strtok(input, " \t\n");
    while (token != NULL && cmd->argc < MAX_ARGS - 1) {
        // Handle environment variables
        if (token[0] == '$') {
            char *env_val = getenv(token + 1);
            cmd->args[cmd->argc++] = env_val ? strdup(env_val) : strdup("");
        } else {
            cmd->args[cmd->argc++] = strdup(token);
        }
        token = strtok(NULL, " \t\n");
    }

    // Check for background process
    if (cmd->argc > 0 && strcmp(cmd->args[cmd->argc - 1], "&") == 0) {
        cmd->background = true;
        free(cmd->args[--cmd->argc]);
    }

    cmd->args[cmd->argc] = NULL;
}

// Execute built-in commands
bool execute_builtin(Command *cmd) {
    if (strcmp(cmd->args[0], "cd") == 0) {
        if (cmd->argc < 2) {
            fprintf(stderr, "cd: missing argument\n");
        } else if (chdir(cmd->args[1]) != 0) {
            perror("cd");
        }
    } else if (strcmp(cmd->args[0], "pwd") == 0) {
        char cwd[MAX_CMD_LEN];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        } else {
            perror("pwd");
        }
    } else if (strcmp(cmd->args[0], "echo") == 0) {
        for (int i = 1; i < cmd->argc; i++) {
            printf("%s ", cmd->args[i]);
        }
        printf("\n");
    } else if (strcmp(cmd->args[0], "exit") == 0) {
        exit(0);
    } else if (strcmp(cmd->args[0], "env") == 0) {
        for (char **env = environ; *env != NULL; env++) {
            printf("%s\n", *env);
        }
    } else if (strcmp(cmd->args[0], "setenv") == 0) {
        if (cmd->argc != 3) {
            fprintf(stderr, "Usage: setenv VARIABLE VALUE\n");
        } else if (setenv(cmd->args[1], cmd->args[2], 1) != 0) {
            perror("setenv");
        }
    } else {
        return false;
    }
    return true;
}

// Execute external commands
void execute_external(Command *cmd) {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        signal(SIGINT, SIG_DFL);
        execvp(cmd->args[0], cmd->args);
        perror("execvp");
        exit(1);
    } else if (pid > 0) {
        // Parent process
        if (!cmd->background) {
            foreground_pid = pid;
            alarm(10);  // Set 10-second timer
            int status;
            waitpid(pid, &status, 0);
            alarm(0);  // Cancel timer
            foreground_pid = -1;
        }
    } else {
        perror("fork");
    }
}

// Handle SIGINT (Ctrl+C)
void handle_sigint(int signum) {
    if (foreground_pid != -1) {
        kill(foreground_pid, SIGINT);
    } else {
        printf("\n");
        display_prompt();
        fflush(stdout);
    }
}

// Handle SIGALRM (timer expiration)
void handle_sigalrm(int signum) {
    if (foreground_pid != -1) {
        printf("\nProcess timed out after 10 seconds. Terminating...\n");
        kill(foreground_pid, SIGTERM);
    }
}