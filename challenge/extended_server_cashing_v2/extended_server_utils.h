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
#include "types.h"
#include "fifo.h"           // Assuming this contains the definition of FIFOQueue
#include "hashTable.h"      // Assuming this contains the definition of HashTable

extern atomic_bool terminate_flag;  // Terminate flag

// Function prototypes
int createServerTcpSocketAndListen(int port, struct sockaddr_in *address);
int reverseHash(uint8_t target_hash[32], uint64_t *start, uint64_t *end, uint64_t *answer);
int reverseHashAndSendValueToClient(request_packet *req, response_packet *resp);
int reverseHashUpdateHashTableAndSendValueToClient(request_packet *req, response_packet *resp, HashTable *hashTable);
int searchHashTableAndSendValueToClient(request_packet *req, response_packet *resp, HashTable *hashTable);
int readRequestFromClient(int client_fd, request_packet *req);
void* handleConnectionAcceptanceThread(void* arg);
void* handleRequestProcessingThread(void* arg);
void signal_handler(int signum);

#endif
