#include "extended_server_utils.h"

atomic_bool terminate_flag = 0; // Terminate flag 

int main(int argc, char *argv[]) {
    if(argc != 2) { // Check command line argument
        printf("Usage: %s <port>\n", argv[0]);
        printf("<port>: The port number to use (e.g., 8080)\n");
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]); // Port number to use
    struct sockaddr_in address;
    request_packet req;
    response_packet resp;
                            
    int server_fd = createServerTcpSocketAndListen(port, &address); // Create server socket and start listening
    int client_fd;

    // Register signal handler for SIGINT
    if (signal(SIGINT, signal_handler) == SIG_ERR) {
        perror("Error setting up signal handler.");
        exit(EXIT_FAILURE);
    }
    
    while(!terminate_flag) {
        
        client_fd = accept(server_fd, (struct sockaddr*)&address, &(socklen_t){sizeof(address)});
        
        if (client_fd == -1) continue;

        
        if (readRequestFromClient(client_fd, &req) != 0) {
            perror("Error reading request from client");
            close(client_fd);
            continue;
        }

        if (reverseHashAndSendValueToClient(&client_fd, &req, &resp) != 0) {
            perror("Error processing request");
            continue;
        }
    }

    close(server_fd);  // Close the server socket
    printf("Server is shutting down gracefully...\n");
    
    return 0;
}
