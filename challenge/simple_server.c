#include <time.h>           // Necessary for clock_gettime
#include <signal.h>         // Necessary for signal handling
#include <stdlib.h>         // Necessary for exit and atoi.
#include <stdio.h>          // Necessary for fprintf, printf, etc.
#include "server_utils.h"   // Custom server functions

// Server and client socket descriptors
int server_fd, new_socket;
struct sockaddr_in address;  // Server address
int addrlen = sizeof(address);
request_packet req;   // Request packet
response_packet resp; // Response packet
HashTable *hashTable;        // Hash table
struct timespec start, end; // time variables

// Signal handler for Ctrl+C
void handle_sigint(int sig) {
    printf("Freeing resources...\n");
    close(server_fd);
    printf("Server closed.\n");
    freeHashTable(hashTable);
    printf("Hash table freed.\n");
    printf("Exiting gracefully...\n");
    exit(0);
}

int main(int argc, char *argv[]) {
    // Check command line arguments
    if(argc != 3) {
        fprintf(stderr, "Usage: %s <port> <verbose print>\n", argv[0]);
        fprintf(stderr, "  <port>: The port number to use (e.g., 8080)\n");
        fprintf(stderr, "  <verbose print>: 1 to enable verbose print, 0 to disable verbose print\n");
        exit(1);
    }

    // Get arguments from command line
    int port = atoi(argv[1]);
    int verbose = atoi(argv[2]);

    // Register signal handler for Ctrl+C
    signal(SIGINT, handle_sigint); 

    if (verbose) {
        hashTable = createHashTable();
        printf("Hash table of size %d created.\n", HASH_TABLE_SIZE);
        
        server_fd = createServerTcpSocketAndListen(port);
        printf("Server socket created and listening on port %d.\n", port);
    }
    else {
        // Initialize hash table
        hashTable = createHashTable();

        // Create server socket and start listening
        server_fd = createServerTcpSocketAndListen(port);
    }
 
    // Main loop
    while (1) {
        // Accept a new client connection and read the request
        if(acceptClientConnectionAndReadRequest(&server_fd, &new_socket, &address, &addrlen, &req) != 0) {
            //continue;
        }

        if (verbose) {
            // Print the request
            printf("Request received from client:\n");
            printf("Client address: %s\n", inet_ntoa(address.sin_addr));
            printf("Hash: ");
            for (int i = 0; i < 32; i++) {
                printf("%02x", req.hash[i]);
            }
            printf("\n");
            printf("Start: %lu\n", req.start);
            printf("End: %lu\n", req.end);
            printf("Priority: %d\n", req.priority);
        
            // time searchHashTableAndSendValueToClient if verbose
            clock_gettime(CLOCK_MONOTONIC, &start);
            if (searchHashTableAndSendValueToClient(&new_socket, &req, &resp, hashTable) == 0) {
                clock_gettime(CLOCK_MONOTONIC, &end);
                printf("Key found in hash table. Request served in %lf microseconds\n\n",((end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9) * 1e6);
                continue;
            } 

            // time reverseHashUpdateHashTableAndSendValueToClient if verbose
            reverseHashUpdateHashTableAndSendValueToClient(&new_socket, &req, &resp, hashTable);
            clock_gettime(CLOCK_MONOTONIC, &end);
            printf("Key not found in hash table. Request served in %lf microseconds\n\n",((end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9) * 1e6);
            
        }
        else {
            if (searchHashTableAndSendValueToClient(&new_socket, &req, &resp, hashTable) == 0) {
                continue;
            }
            reverseHashUpdateHashTableAndSendValueToClient(&new_socket, &req, &resp, hashTable);
        }
    }
    return 0;
}