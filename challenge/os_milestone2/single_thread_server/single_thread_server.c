#include "extended_server_utils.h"

atomic_bool terminate_flag = false; 
int server_fd;

int main(int argc, char *argv[]) {
    // Check command line argument
    if(argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        printf("<port>: The port number to use (e.g., 8080)\n");
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);          // Port number to use
    struct sockaddr_in address;
    request_packet req;
    response_packet resp;

    // Register signal handler for SIGINT
    if (signal(SIGINT, signal_handler) == SIG_ERR) {
        perror("Error setting up signal handler.");
        exit(EXIT_FAILURE);
    }

    server_fd = createServerTcpSocketAndListen(port, &address); // Create server socket and start listening

    while(!terminate_flag) {
        int client_fd = accept(server_fd, (struct sockaddr*)&address, &(socklen_t){sizeof(address)});
        
        if (client_fd == -1) {
            if (terminate_flag) {
                break; // Server was interrupted, exit loop
            }
            perror("Error accepting connection.");
            continue;
        }
        
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

    printf("Server is shutting down gracefully...\n");
    close(server_fd);  // Close the server socket

    return 0;
}
