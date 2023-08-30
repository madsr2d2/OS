#include <stdio.h>          // Necessary for fprintf, printf, etc.
#include <stdlib.h>         // Necessary for exit and atoi.
#include <string.h>         // Necessary for memcmp.
#include <unistd.h>         // Necessary for read, write, and close.
#include <arpa/inet.h>      // Necessary for socket operations like htons, accept, etc.
#include <signal.h>         // Necessary for signal handling.

#include "max_heap.h"       // Importing custom max-heap data structure and functions
#include "reverse_hash.h"   // Importing custom reverse-hash function

// Constants for the sizes of request and response packets
#define REQ_SIZE 49
#define RESP_SIZE 8

// Global flag for signal handling, used to safely exit the server loop
volatile sig_atomic_t keep_running = 1;

// Function to handle Ctrl+C signals, sets the keep_running flag to 0
void handle_sigint(int sig) {
    keep_running = 0;
}

// Structure to hold the response packet data
struct response_packet {
    uint64_t answer;
};

// Function to validate command line arguments
void validate_args(int argc, char *argv[]) {
    // Check for the correct number of arguments
    if(argc != 3) {
        fprintf(stderr, "Usage: %s <port> <mode>\n", argv[0]);
        fprintf(stderr, "Mode can be 'sequential' or 'max_heap'\n");
        exit(EXIT_FAILURE);
    }
    
    // Validate the port number
    int port = atoi(argv[1]);
    if (port <= 0 || port > 65535) {    
        fprintf(stderr, "Invalid port number: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    // Validate the mode, can be 'sequential' or 'max_heap'
    if (strcmp(argv[2], "sequential") != 0 && strcmp(argv[2], "max_heap") != 0) {
        fprintf(stderr, "Invalid mode: %s\n", argv[2]);
        fprintf(stderr, "Mode can be 'sequential' or 'max_heap'\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    // Validate the command-line arguments
    validate_args(argc, argv);

    // Parse port and mode from co½mmand-line arguments
    int port = atoi(argv[1]);
    char *mode = argv[2];

    // Register the Ctrl+C signal handler
    signal(SIGINT, handle_sigint);

    // Server and client socket descriptors
    int server_fd, new_socket;
    struct sockaddr_in address;  // Server address structure
    int addrlen = sizeof(address); // Address length
    struct request_packet req;   // Request packet structure
    struct response_packet resp; // Response packet structure

    // Create the server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address and port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Bind the socket to the address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming client connections
    if (listen(server_fd, 50) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Main server loop
    while (keep_running) {
        // Accept a new client connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // Read request packet from client
        int valread = read(new_socket, &req, REQ_SIZE);
        if (valread != REQ_SIZE) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        // Convert byte order of the received data
        req.start = be64toh(req.start);
        req.end = be64toh(req.end);

        // Mode-based handling: sequential or max_heap
        if (strcmp(mode, "sequential") == 0) {
            uint64_t answer = reverse_hash(req.hash, req.start, req.end);
            resp.answer = htobe64(answer);
        } else {
            // Insert request to the max heap and extract the highest-priority request
            insert(req);
            struct heap_node highestPriorityNode = extractMax();
            uint64_t answer = reverse_hash(highestPriorityNode.req.hash, highestPriorityNode.req.start, highestPriorityNode.req.end);
            resp.answer = htobe64(answer);
        }

        // Write the response back to the client
        int valwrite = write(new_socket, &resp, sizeof(resp));
        if (valwrite != sizeof(resp)) {
            perror("write");
            exit(EXIT_FAILURE);
        }

        // Close the client socket
        close(new_socket);

        // Check if a signal to stop was received
        if (!keep_running) {
            printf("Received Ctrl+C. Shutting down.\n");
            break;
        }
    }

    // Close the server socket and exit
    close(server_fd);
    return 0;
}
