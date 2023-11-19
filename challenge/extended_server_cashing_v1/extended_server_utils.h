#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H

#include <sys/socket.h>     // For socket(), bind(), listen(), accept(), and setsockopt()
#include <netinet/in.h>     // For sockaddr_in and htons()
#include <arpa/inet.h>      // For byte order conversion functions like htonl(), htons(), ntohl(), ntohs()
#include <unistd.h>         // For close()
#include <fcntl.h>          // For fcntl() to set non-blocking mode
#include <signal.h>         // For signal handling, particularly SIGINT
#include <stdio.h>          // For perror() and printf()
#include <stdlib.h>         // For exit() and atoi()
#include <pthread.h>        // For pthread_create(), pthread_join(), etc.
#include <stdatomic.h>      // For atomic_bool
#include <openssl/sha.h>    // For SHA256() function
#include <stdint.h>         // For uint8_t, uint64_t, etc.
#include <string.h>         // For memcmp() and memcpy()
#include "fifo.h"           // Assuming this contains the definition of FIFOQueue
#include "hashTable.h"      // Assuming this contains the definition of HashTable

#define QUEUE_SIZE 1000     // Number of pending connections in the connection queue
#define REQ_SIZE 49         // Size of the request packet
#define RESP_SIZE 8         // Size of the response packet

extern atomic_bool terminate_flag;  // Terminate flag

// Request packet structure
#pragma pack(push, 1) // exact fit - no padding
typedef struct request_packet {
    uint8_t hash[32];
    uint64_t start;
    uint64_t end;
    uint8_t priority;
} request_packet;
#pragma pack(pop) // back to whatever the previous packing mode was

// Response packet structure
typedef struct response_packet{
    uint64_t answer;
} response_packet;

// Arguments for the acceptConnections thread
typedef struct {
    FIFOQueue *queue;
    int port;
} acceptConnectionsArgs;

// Arguments for the processRequests thread
typedef struct {
    FIFOQueue *queue;
    HashTable *hashTable;
} processRequestsArgs;

// Function prototypes
int createServerTcpSocketAndListen(int port, struct sockaddr_in *address);
int reverseHash(uint8_t target_hash[32], uint64_t *start, uint64_t *end, uint64_t *answer);
int reverseHashAndSendValueToClient(int *client_fd, request_packet *req, response_packet *resp);
int reverseHashUpdateHashTableAndSendValueToClient(int *client_fd, request_packet *req, response_packet *resp, HashTable *hashTable);
int searchHashTableAndSendValueToClient(int *new_socket, request_packet *req, response_packet *resp, HashTable *hashTable);
int readRequestFromClient(int client_fd, request_packet *req);
void* handleConnectionAcceptanceThread(void* arg);
void* handleRequestProcessingThread(void* arg);
void signal_handler(int signum);

#endif
