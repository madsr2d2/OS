#include <stdio.h>          // Necessary for fprintf, printf, etc.
#include <stdlib.h>         // Necessary for exit and atoi.
#include <string.h>         // Necessary for memcmp.
#include <unistd.h>         // Necessary for read, write, and close.
#include <arpa/inet.h>      // Necessary for socket operations like htons, accept, etc.
#include <signal.h>         // Necessary for signal handling.

#include "max_heap.h"       // Custom max heap functions
#include "reverse_hash.h"   // Custom reverse hash function

#define REQ_SIZE 49
#define RESP_SIZE 8


// Global flag for signal handling
volatile sig_atomic_t keep_running = 1;

// Signal handler for Ctrl+C
void handle_sigint(int sig) {
    keep_running = 0;
}

// Response packet structure
struct response_packet {
    uint64_t answer;
};

int main(int argc, char *argv[]) {
    // Check command line arguments
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Parse port number
    int port = atoi(argv[1]);

    // Register signal handler for Ctrl+C
    signal(SIGINT, handle_sigint); 

    // Server and client socket descriptors
    int server_fd, new_socket;
    struct sockaddr_in address;  // Server address
    int addrlen = sizeof(address);
    struct request_packet req;   // Request packet
    struct response_packet resp; // Response packet

    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Bind server socket to the address
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Start listening for client connections
    if (listen(server_fd, 50) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Main loop
    while (keep_running) {
        // Accept a new client connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // Read the request from client
        int valread = read(new_socket, &req, REQ_SIZE);
        if (valread != REQ_SIZE) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        // Convert to host byte order
        req.start = be64toh(req.start);
        req.end = be64toh(req.end);

        // Insert the request into max heap
        insert(req, new_socket);

        // Extract the highest priority request and process it
        struct heap_node highestPriorityNode = extractMax();
        uint64_t answer = reverse_hash(highestPriorityNode.req.hash, highestPriorityNode.req.start, highestPriorityNode.req.end);

        // Convert to network byte order
        resp.answer = htobe64(answer);

        // Send the response back to the client
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

    // Close the server socket
    close(server_fd);

    return 0;
}