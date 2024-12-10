#include <netinet/in.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h> 
  
#define PORT 9001
#define MAX_COMMAND_LINE_LEN 1024

// Get command from user input
char* getCommandLine(char *command_line){
    do{ 
        if ((fgets(command_line, MAX_COMMAND_LINE_LEN, stdin) == NULL) && ferror(stdin)) {
            fprintf(stderr, "fgets error");
            exit(0);
        }
    }while(command_line[0] == 0x0A);  // Repeat if just ENTER pressed
    
    command_line[strlen(command_line) - 1] = '\0';
    return command_line;
}

int main(int argc, char const* argv[]) 
{ 
    // Create socket
    int sockID = socket(AF_INET, SOCK_STREAM, 0); 
    char  *token, *cp;
    char buf[MAX_COMMAND_LINE_LEN];
    char responeData[MAX_COMMAND_LINE_LEN];

    // Set up server address
    struct sockaddr_in servAddr; 
    servAddr.sin_family = AF_INET; 
    servAddr.sin_port = htons(PORT);
    servAddr.sin_addr.s_addr = INADDR_ANY; 
  
    // Connect to server
    int connectStatus = connect(sockID, (struct sockaddr*)&servAddr, sizeof(servAddr)); 
  
    if (connectStatus == -1) { 
        printf("Error...\n"); 
    } 
    else { 
        // Main client loop
        while(1) {
            printf("Enter Command (or menu): ");
            getCommandLine(buf);

            // Send command to server
            send(sockID, buf, strlen(buf), 0);

            cp = buf;
            token = strtok(cp, " ");

            // Handle exit command
            if(strcmp(token,"exit") == 0){
                exit(1);
            }
            // Display menu
            else if(strcmp(token,"menu") == 0){
                printf("COMMANDS:\n---------\n1. print\n2. get_length\n3. add_back <value>\n4. add_front <value>\n5. add_position <index> <value>\n6. remove_back\n7. remove_front\n8. remove_position <index>\n9. get <index>\n10. exit\n");
            }
 
            // Receive server response
            recv(sockID, responeData, sizeof(responeData), 0);
  
            // Display response
            printf("\nSERVER RESPONSE: %s\n", responeData); 
            memset(buf, '\0', MAX_COMMAND_LINE_LEN);
        }
    } 
 
    return 0; 
}