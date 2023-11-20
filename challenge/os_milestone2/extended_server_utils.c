#include "extended_server_utils.h"

// function that creates a TCP socket and starts listening. The function returns a file descriptor for the TCP socket
int createServerTcpSocketAndListen(int port, struct sockaddr_in *address) {
    
    // Create a TCP server socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        return -1;
    }

    // Set the socket option SO_REUSEADDR to reuse the port if it's in TIME_WAIT state
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt SO_REUSEADDR failed");
        close(server_fd);
        return -1;
    }

    // Set up the server address structure
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;  // Accept connections to any IP address
    address->sin_port = htons(port);  // Convert port number to network byte order

    // Bind the server socket to the specified address and port
    if (bind(server_fd, (struct sockaddr *)address, sizeof(*address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        return -1;
    }

    // Start listening for incoming connections
    if (listen(server_fd, QUEUE_SIZE) < 0) {
        perror("Listen failed");
        close(server_fd);
        return -1;
    }

    // Set the server socket to non-blocking mode
    fcntl(server_fd, F_SETFL, O_NONBLOCK);

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
int reverseHashAndSendValueToClient(int *client_fd, request_packet *req, response_packet *resp) {
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
    if (send(*client_fd, resp, RESP_SIZE,0) != RESP_SIZE) {
        perror("Send to client failed.");
        exit(EXIT_FAILURE);
    }
    
    // Close the client socket
    close(*client_fd);
    return 0;   
}

// thread handler for accepting connections and adding them to the queue
void* threadAcceptConnectionsHandler(void* args) {
    
    struct sockaddr_in address;

    int server_fd = createServerTcpSocketAndListen(((acceptConnectionsArgs*)args)->port, &address); // Create server socket and start listening

    while (!terminate_flag) {
        // Accept a new connection and add it to the queue
        enqueue(((acceptConnectionsArgs*)args)->queue, accept(server_fd, (struct sockaddr*)&address, &(socklen_t){sizeof(address)}));
    }
    
    printf("Thread threadAcceptConnectionsHandler is shutting down gracefully...\n");

    return NULL;
}

// function that reads the request from the client in a loop until it has read REQ_SIZE bytes of data.
int readRequestFromClient(int client_fd, request_packet *req) {
    // Read the request from client and make sure we get REQ_SIZE bytes of data.
    int bytesRead = 0;
    while (bytesRead < REQ_SIZE && !terminate_flag) {
        int n = recv(client_fd, ((char*)req) + bytesRead, REQ_SIZE - bytesRead,0);
        
        if (n == -1) return 1; // Error in reading
        
        if (n == 0) return 2; // Client closed connection before sending REQ_SIZE bytes of data
        
        bytesRead += n;
    }

    return 0; // Success
}

// thread handler for processing requests
void* threadProcessRequestsHandler(void* arg) {
    request_packet req;             // Request packet
    response_packet resp;           // Response packet
    

    while (!terminate_flag) {
        int client_fd = dequeue((FIFOQueue *)arg);  // Pass the queue's address to the dequeue function
        
        if (client_fd == -1) { // Continue if queue is empty
            continue;
        }
        

        if (readRequestFromClient(client_fd, &req) != 0) {
            perror("Error reading request from client");
            close(client_fd);  // Closing the socket on error is a good idea to free up resources
            continue;
        }

        if (reverseHashAndSendValueToClient(&client_fd, &req, &resp) != 0) {
            perror("Error processing request");
            continue;
        }
    }
    printf("Thread  threadProcessRequestsHandler is shutting down gracefully...\n");
    return NULL;
}

// Signal handler for Ctrl+C
void signal_handler(int signum) {
    if (signum == SIGINT) {
        terminate_flag = 1;
        shutdown(server_fd, SHUT_RDWR);
        close(server_fd);
    }
}

