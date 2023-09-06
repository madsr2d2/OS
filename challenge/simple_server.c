#include <stdio.h>          // Necessary for fprintf, printf, etc.
#include <stdlib.h>         // Necessary for exit and atoi.
#include <string.h>         // Necessary for memcmp.
#include <unistd.h>         // Necessary for read, write, and close.
#include <arpa/inet.h>      // Necessary for socket operations like htons, accept, etc.
#include <signal.h>         // Necessary for signal handling.

#include "max_heap.h"       // Custom max heap functions
#include "reverse_hash.h"   // Custom reverse hash function
#include "tcpServerSetup.h" // Custom TCP server setup functions

#define REQ_SIZE 49
#define RESP_SIZE 8

// Server and client socket descriptors
int server_fd, new_socket;
struct sockaddr_in address;  // Server address
int addrlen = sizeof(address);
struct request_packet req;   // Request packet
struct response_packet resp; // Response packet

// Signal handler for Ctrl+C
void handle_sigint(int sig) {
    close(server_fd);
    printf("Server closed.\n");
    printf("Exiting gracefully...\n");
    exit(0);
}

// Response packet structure
struct response_packet {
    uint64_t answer;
};

int main(int argc, char *argv[]) {
    // Check command line arguments
    if(argc != 2) {
        fprintf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    // Register signal handler for Ctrl+C
    signal(SIGINT, handle_sigint); 


    // Create server socket and start listening
    server_fd = createServerTcpSocketAndListen(atoi(argv[1]));

    // Initialize hash table
    

    // Main loop
    while (1) {
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

    }

  

    return 0;
}