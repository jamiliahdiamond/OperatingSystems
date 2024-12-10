#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include "list.h"

#define PORT 9001
#define ACK "ACK:" // Prefix for server responses

// Global variables for socket cleanup
static int servSockD = -1;
static int clientSocket = -1;
static list_t *mylist = NULL;

// Handles graceful shutdown on Ctrl-C
void handle_sigint(int sig) {
    printf("\nReceived Ctrl-C. Cleaning up...\n");
    
    // Free resources
    if (mylist != NULL) {
        list_free(mylist);
        mylist = NULL;
    }
    if (clientSocket != -1) {
        close(clientSocket);
        clientSocket = -1;
    }
    if (servSockD != -1) {
        close(servSockD);
        servSockD = -1;
    }
    
    printf("Server shutdown complete.\n");
    exit(0);
}

int main(int argc, char const* argv[]) {
    int n, val, idx;
    
    // Set up signal handler for Ctrl-C
    signal(SIGINT, handle_sigint);

    // Create server socket
    servSockD = socket(AF_INET, SOCK_STREAM, 0);
    
    char buf[1024];
    char sbuf[1024];
    char* token;
    
    struct sockaddr_in servAddr;
    
    // Set up server address
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(PORT);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to IP and port
    if (bind(servSockD, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    // Start listening for connections
    listen(servSockD, 1);
    printf("Server listening on port %d...\n", PORT);

    // Accept a client connection
    clientSocket = accept(servSockD, NULL, NULL);
    printf("Client connected!\n");

    mylist = list_alloc();  // Create the list

    while(1) {
        // Receive message from client
        n = recv(clientSocket, buf, sizeof(buf), 0);
        buf[n] = '\0';

        if (n > 0) {
            token = strtok(buf, " ");

            // Handle different commands
            if (strcmp(token, "exit") == 0) {
                // Exit command
            }
            else if (strcmp(token, "get_length") == 0) {
                // Get length
            }
            else if (strcmp(token, "add_front") == 0) {
                // Add to front
            }
            else if (strcmp(token, "add_back") == 0) {
                // Add to back
            }
            else if (strcmp(token, "add_position") == 0) {
                // Add at index
            }
            else if (strcmp(token, "remove_front") == 0) {
                // Remove front
            }
            else if (strcmp(token, "remove_back") == 0) {
                // Remove back
            }
            else if (strcmp(token, "remove_position") == 0) {
                // Remove at index
            }
            else if (strcmp(token, "get") == 0) {
                // Get element at index
            }
            else if (strcmp(token, "print") == 0) {
                // Print list
            }
            else {
                // Invalid command
            }

            // Send response to client
            send(clientSocket, sbuf, sizeof(sbuf), 0);
        }
        memset(buf, '\0', 1024);
    }

    return 0;
}