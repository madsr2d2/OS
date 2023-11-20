#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H

// Standard and network headers for socket programming and system operations.
#include <sys/socket.h>  // Socket-related functions: socket(), bind(), listen(), accept(), setsockopt().
#include <netinet/in.h>  // Structures and functions for internet domain addresses, e.g., sockaddr_in, htons().
#include <arpa/inet.h>   // Functions for manipulating numeric IP addresses.
#include <unistd.h>      // POSIX operating system API: close().
#include <fcntl.h>       // File control options: fcntl().
#include <signal.h>      // Signal handling utilities, e.g., signal().
#include <stdio.h>       // Standard input/output library for functions like perror(), printf().
#include <stdlib.h>      // Standard library functions: exit(), atoi().
#include <pthread.h>     // Thread functions: pthread_create(), pthread_join(), etc.
#include <stdatomic.h>   // Atomic data types for thread synchronization, e.g., atomic_bool.
#include <openssl/sha.h> // OpenSSL's SHA256 hash function.
#include <stdint.h>      // Standard integer types: uint8_t, uint64_t, etc.
#include <string.h>      // String handling functions: memcmp(), memcpy().
#include "fifo.h"           // Include the fifo header

#define QUEUE_SIZE 1000      // Number of pending connections in the connection queue
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
