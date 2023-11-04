#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H

#include <stdint.h>         // Necessary for uint8_t, uint16_t, etc.
#include <arpa/inet.h>      // Necessary for socket operations like htons, accept, etc.
#include <openssl/sha.h>    // Necessary for SHA256
#include <string.h>         // Necessary for memcpy
#include <errno.h>          // Necessary for errno
#include <stdbool.h>        // Necessary for bool, true, false
#include <stdatomic.h>      // Necessary for atomic_bool
#include <signal.h>         // Necessary for signal handling
#include <stdlib.h>         // Necessary for exit and atoi.
#include <stdio.h>          // Necessary for fprintf, printf, etc.
#include <pthread.h>        // Necessary for pthread_create, pthread_join, etc.
#include <unistd.h>         // Necessary for read, write, and close.
#include <sys/socket.h>     // Necessary for socket, bind, listen, accept, etc.
#include <netinet/in.h>     // Necessary for sockaddr_in, htons, etc.
#include "fifo.h"           // Include the fifo header

#define QUEUE_SIZE 50       // Number of pending connections in the connection queue
#define REQ_SIZE 49         // Size of the request packet
#define RESP_SIZE 8         // Size of the response packet

extern atomic_bool terminate_flag;
extern int server_fd;

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

// Function prototypes
int createServerTcpSocketAndListen(int port, struct sockaddr_in *address);
int reverseHash(uint8_t target_hash[32], uint64_t *start, uint64_t *end, uint64_t *answer);
int reverseHashAndSendValueToClient(int *client_fd, request_packet *req, response_packet *resp);
int readRequestFromClient(int client_fd, request_packet *req);
void* threadAcceptConnectionsHandler(void* arg);
void* threadProcessRequestsHandler(void* arg);
void signal_handler(int signum);

#endif
