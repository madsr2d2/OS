

#include "simple_server.h"



// Server and client socket descriptors
int server_fd, new_socket;
struct sockaddr_in address;  // Server address
int addrlen = sizeof(address);
struct request_packet req;   // Request packet
struct response_packet resp; // Response packet

HashTable *hashTable;        // Hash table

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

// Response packet structure
struct response_packet {
    uint64_t answer;
};

int main(int argc, char *argv[]) {
    // Check command line arguments
    if(argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    // Register signal handler for Ctrl+C
    signal(SIGINT, handle_sigint); 


    // Initialize hash table
    hashTable = createHashTable();
    printf("Hash table initialized.\n");

    // Create server socket and start listening
    server_fd = createServerTcpSocketAndListen(atoi(argv[1]));
    printf("Server is listening on port %s...\n", argv[1]);



    

    // Main loop
    while (1) {
        // Accept a new client connection
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Accept failed");
            handle_sigint(1);

        }

        // Read the request from client
        int valread = read(new_socket, &req, REQ_SIZE);
        if (valread != REQ_SIZE) {
            perror("Read failed");
            handle_sigint(1);
        }

        // check if the request is already in the hash table and send the response back to the client
        uint64_t value;
        if (search(hashTable, req.hash, &value) == 0) {
            
            // Convert to network byte order
            resp.answer = htobe64(value);

            // Send the response back to the client
            int valwrite = write(new_socket, &resp, sizeof(resp));
            if (valwrite != sizeof(resp)) {
                perror("Write failed");
                handle_sigint(1);
            }

            // Close the client socket
            close(new_socket);
            continue;
        }

        // Convert to host byte order
        req.start = be64toh(req.start);
        req.end = be64toh(req.end);

  
        uint64_t answer = reverse_hash(req.hash, req.start, req.end);

        // Insert the request and the answer into the hash table
        insert(hashTable, req.hash, answer);

        // Convert to network byte order
        resp.answer = htobe64(answer);

        // Send the response back to the client
        int valwrite = write(new_socket, &resp, sizeof(resp));
        if (valwrite != sizeof(resp)) {
            perror("Write failed");
            exit(1);
        }

        // Close the client socket
        close(new_socket);
    }

  

    return 0;
}