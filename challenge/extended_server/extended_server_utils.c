#include "extended_server_utils.h"

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

int reverseHash(uint8_t target_hash[32], uint64_t *start, uint64_t *end, uint64_t *answer) {
    unsigned char sha256_result[SHA256_DIGEST_LENGTH];

    for (uint64_t i = *start; i <= *end; i++) {
        // Perform hashing on the converted value
        SHA256((unsigned char *)&(uint64_t){htole64(i)}, sizeof(uint64_t), sha256_result);
        // Check if the hash matches the target hash
        if (memcmp(sha256_result, target_hash, 32) == 0) {
            *answer = i;
            return 0; // Success: Answer found
        }
    }
    return 1; // Fail: Answer not found
}

int reverseHashAndSendValueToClient(int *client_fd, request_packet *req, response_packet *resp) {
    uint64_t answer = 0; // Initialize answer
    
    // Convert range values from network to host byte order
    req->start = be64toh(req->start);
    req->end = be64toh(req->end);

    // Attempt to reverse the hash
    if (reverseHash(req->hash, &req->start, &req->end, &answer) != 0) {
        perror("Reverse hash not found. Sending 0 to client.");
    }
    
    // Set the response answer in network byte order
    resp->answer = htobe64(answer);

    // Send the response to the client
    if (send(*client_fd, resp, RESP_SIZE, 0) != RESP_SIZE) {
        perror("Send to client failed.");
        exit(EXIT_FAILURE);
    }
    
    // Close the client socket
    close(*client_fd);
    return 0;   
}

void* handleConnectionAcceptanceThread(void* args) {
    
    struct sockaddr_in address; // Server address structure

    // Create and set up the server socket for listening
    int server_fd = createServerTcpSocketAndListen(((acceptConnectionsArgs*)args)->port, &address);

    // Accept connections until the terminate flag is set
    while (!terminate_flag) {
        int client_fd = accept(server_fd, (struct sockaddr*)&address, &(socklen_t){sizeof(address)});
        if (client_fd != -1) {
            enqueue(((acceptConnectionsArgs*)args)->queue, client_fd);
        }
    }

    printf("Thread handleConnectionAcceptanceThread is shutting down gracefully...\n");
    close(server_fd); // Close the server socket
    return NULL; 
}

int readRequestFromClient(int client_fd, request_packet *req) {
    int bytesRead = 0;
    while (bytesRead < REQ_SIZE && !terminate_flag) {
        int n = recv(client_fd, ((char*)req) + bytesRead, REQ_SIZE - bytesRead, 0);
        if (n == -1) return 1; // Error in recv
        if (n == 0) return 2; // Socket closed before reading all data
        bytesRead += n;
    }
    return 0;
}

void* handleRequestProcessingThread(void* arg) {
    request_packet req;
    response_packet resp;
    FIFOQueue *queue = ((processRequestsArgs *)arg)->queue;

    // Process requests until the terminate flag is set
    while (!terminate_flag) {
        int client_fd = dequeue(queue);
        if (client_fd != -1) {
            if (readRequestFromClient(client_fd, &req) != 0) {
                perror("Error reading request from client");
                close(client_fd);
                continue;
            }

            //if (searchHashTableAndSendValueToClient(&client_fd, &req, &resp, hashTable) == 0) continue;

            if (reverseHashAndSendValueToClient(&client_fd, &req, &resp) != 0) {
                perror("Error processing request");
                continue;
            }
        }
    }

    printf("Thread handleRequestProcessingThread is shutting down gracefully...\n");
    return NULL;
}


void signal_handler(int signum) {
    // Check if the received signal is SIGINT (Ctrl+C)
    if (signum == SIGINT) {
        // Set the global terminate flag to 1 to indicate termination of the program
        terminate_flag = 1;
    }
}

