#include "extended_server_utils.h"

// function that creates a TCP socket and starts listening. The function returns a file descriptor for the TCP socket
int createServerTcpSocketAndListen(int port, struct sockaddr_in *address) {
    
    // Create server socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        return -1;
    }

    // Set the SO_REUSEADDR option
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt SO_REUSEADDR failed");
        close(server_fd);
        return -1;
    }

    // Configure server address
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(port);

    // Bind server socket to the address
    if (bind(server_fd, (struct sockaddr *)address, sizeof(*address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        return -1;
    }

    // Start listening
    if (listen(server_fd, QUEUE_SIZE) < 0) {
        perror("Listen failed");
        close(server_fd);
        return -1;
    }

    fcntl(server_fd, F_SETFL, O_NONBLOCK); // Make the server socket non-blocking

    return server_fd;
}

// Function implementation for reverse Hash
int reverseHash(uint8_t target_hash[32], uint64_t *start, uint64_t *end, uint64_t *answer) {
    unsigned char sha256_result[SHA256_DIGEST_LENGTH];

    for (uint64_t i = *start; i <= *end; i++) {
        //Handle little endian conversion before reverse hashing..
        SHA256((unsigned char *)&(uint64_t){htole64(i)}, sizeof(uint64_t), sha256_result); // sha256_result is in big-endian format so we don't need to reverse it.
        if (memcmp(sha256_result, target_hash, 32) == 0) {
            *answer = i;
            return 0; // Answer Found
        }

    }
    return 1; // Answer not found
}

// function that reverses the hash, updates the hash table, and sends the value to the client. 
int reverseHashAndSendValueToClient(request_packet *req, response_packet *resp) {
    uint64_t answer = 0; // Answer to the reverse hash problem
    
    // Convert to host byte order
    req->start = be64toh(req->start);
    req->end = be64toh(req->end);

    if (reverseHash(req->hash, &req->start, &req->end, &answer)!=0) {
        perror("Reverse hash not found. Sending 0 to client.");
    }
    
    // Convert to network byte order
    resp->answer = htobe64(answer);

    // Send the response back to the client
    if (send(req->client_fd, resp, RESP_SIZE,0) != RESP_SIZE) {
        perror("Send to client failed.");
        exit(EXIT_FAILURE);
    }
    
    // Close the client socket
    close(req->client_fd);
    return 0;   
}

// Thread handler for accepting connections and adding them to the queue
void* handleConnectionAcceptanceThread(void* args) {
    
    struct sockaddr_in address; // Server address

    int server_fd = createServerTcpSocketAndListen(((acceptConnectionsArgs*)args)->port, &address); // Create server socket and start listening

    request_packet req; // Request packet
    response_packet resp; // Response packet
    FIFOQueue *queue = ((processRequestsArgs *)args)->queue; // FIFO queue
    HashTable *hashTable = ((processRequestsArgs *)args)->hashTable; // Hash table


    while (!terminate_flag) {
        int client_fd = accept(server_fd, (struct sockaddr*)&address, &(socklen_t){sizeof(address)});

        if (client_fd == -1) continue; // Continue if accept() returns -1
        else {
            if (readRequestFromClient(client_fd, &req) != 0) {
                perror("Error reading request from client");
                close(client_fd);  // Closing the socket 
                continue;
            }

            if (searchHashTableAndSendValueToClient(&req, &resp, hashTable) == 0) { 
                continue; // Continue if the key is found
            }
            
            enqueue(queue, req); // Add the request to the queue
        }
    }

    printf("Thread handleConnectionAcceptanceThread is shutting down gracefully...\n");
    close(server_fd);
    return NULL; 
}


// function that reads the request from the client in a loop until it has read REQ_SIZE bytes of data.
int readRequestFromClient(int client_fd, request_packet *req) {
    // Read the request from client and make sure we get REQ_SIZE bytes of data.
    int bytesRead = 0;
    while (bytesRead < REQ_SIZE && !terminate_flag) {
        int n = recv(client_fd, ((char*)req) + bytesRead, REQ_SIZE - bytesRead,0);
        
        if (n == -1) return 1; // Error in recv
        if (n == 0) return 2; // Socket closed before reading all the data
        
        bytesRead += n;
    }
    req->client_fd = client_fd;

    return 0;
}

// thread handler for processing requests
void* handleRequestProcessingThread(void* arg) {
    request_packet req;             // Request packet
    response_packet resp;           // Response packet
    FIFOQueue *queue = ((processRequestsArgs *)arg)->queue;      // FIFO queue
    HashTable *hashTable = ((processRequestsArgs *)arg)->hashTable; // Hash table

    while (!terminate_flag) {
        if(dequeue(queue, &req) == -1) continue; // Continue if queue is empty

        // Reverse the hash, update the hash table, and send the value to the client
        if (reverseHashUpdateHashTableAndSendValueToClient(&req, &resp, hashTable) != 0) {
            perror("Error processing request");
            continue;
        }

    }

    close(req.client_fd);
    printf("Thread handleRequestProcessingThread is shutting down gracefully...\n");
    return NULL;
}


// function that searches the hash table for the key and sends the value to the client if the key is found. The function returns 0 if the key is found and 1 if the key is not found.
int searchHashTableAndSendValueToClient(request_packet *req, response_packet *resp, HashTable *hashTable) {
    uint64_t value;

    // Search hash table for the key
    if (search(hashTable, req->hash, &value) == 0) {  
        // Convert to network byte order
         resp->answer = htobe64(value);

        // Send the response back to the client
        if (send(req->client_fd, resp, RESP_SIZE, 0) != RESP_SIZE) {
            perror("Write failed");
            return 2;
        }

        // Close the client socket
        close(req->client_fd);
        return 0;
    }
    else {
        // Key not found
        return 1;
    }
}


// function that reverses the hash, updates the hash table, and sends the value to the client. 
int reverseHashUpdateHashTableAndSendValueToClient(request_packet *req, response_packet *resp, HashTable *hashTable) {
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
    if (send(req->client_fd, resp, RESP_SIZE,0) != RESP_SIZE) {
        perror("Write failed");
        return 1;
    }
    
    // Close the client socket
    close(req->client_fd);
    return 0;   
}


// Signal handler for Ctrl+C
void signal_handler(int signum) {
    if (signum == SIGINT) {
        terminate_flag = 1;
    }
}

