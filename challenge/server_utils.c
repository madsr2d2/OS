#include "server_utils.h"

// function that creates a TCP socket and starts listening. The function returns a file descriptor for the TCP socket
int createServerTcpSocketAndListen(int port) {
    int server_fd;
    struct sockaddr_in address;  // Server address

    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(1);
    }

    // Configure server address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Bind server socket to the address
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    // Start listening
    if (listen(server_fd, QUEUE_SIZE) < 0) {
        perror("Listen failed");
        exit(1);
    }

    return server_fd;
}

// function that searches the hash table for the key and sends the value to the client if the key is found. The function returns 0 if the key is found and 1 if the key is not found.
int searchHashTableAndSendValueToClient(int *new_socket, request_packet *req, response_packet *resp, HashTable *hashTable) {
    uint64_t value;

    // Search hash table for the key
    if (search(hashTable, req->hash, &value) == 0) {  
        // Convert to network byte order
         resp->answer = htobe64(value);

        // Send the response back to the client
        if (write(*new_socket, resp, RESP_SIZE) != RESP_SIZE) {
            perror("Write failed");
            exit(1);
        }

        // Close the client socket
        close(*new_socket);
        return 0;
    }
    else {
        // Key not found
        return 1;
    }
}

// function that reverses the hash, updates the hash table, and sends the value to the client. 
int reverseHashUpdateHashTableAndSendValueToClient(int *new_socket, request_packet *req, response_packet *resp, HashTable *hashTable) {
    uint64_t answer = 0; // Answer to the reverse hash problem
    
    // Convert to host byte order
    req->start = be64toh(req->start);
    req->end = be64toh(req->end);

    // new reverse hash function
    if (reverseHash(req->hash, &req->start, &req->end, &answer)!=0) {
        perror("Reverse hash not found. Sending 0 to client.");
    }

    // Insert the request and the answer into the hash table
    insert(hashTable, req->hash, answer);
    
    // Convert to network byte order
    resp->answer = htobe64(answer);

    // Send the response back to the client
    if (write(*new_socket, resp, RESP_SIZE) != RESP_SIZE) {
        perror("Write failed");
        exit(1);
    }
    
    // Close the client socket
    close(*new_socket);
    return 0;   
}

// function that accepts a new client connection and reads the request from the client. The function returns 0 if successful and 1 if unsuccessful.
int acceptClientConnectionAndReadRequest(int *server_fd, int *new_socket, struct sockaddr_in *address, int *addrlen, request_packet *req) {
    // Accept a new client connection
    if ((*new_socket = accept(*server_fd, (struct sockaddr *)address, (socklen_t *)addrlen)) < 0) {
        perror("Accept failed");
        return 1;
    }

    // Read the request from client
    int valread = read(*new_socket, req, REQ_SIZE);
    if (valread != REQ_SIZE) {
        perror("Read failed");
        return 1;
    }
    return 0;
}

// Function implementation for reverseHash
int reverseHash(uint8_t target_hash[32], uint64_t *start, uint64_t *end, uint64_t *answer) {
    unsigned char sha256_result[SHA256_DIGEST_LENGTH];
    uint64_t i;
    for (i = *start; i <= *end; i++) {
        SHA256((unsigned char *)&i, sizeof(uint64_t), sha256_result); // sha256_result is in big-endian format so we don't need to reverse it.
        if (memcmp(sha256_result, target_hash, 32) == 0) {
            *answer = i;
            return 0; // Answer Found
        }
    }
    return 1; // Answer not found
}