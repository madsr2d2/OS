#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H

#include <stdint.h>         // Necessary for uint8_t, uint16_t, etc.
#include <unistd.h>         // Necessary for read, write, and close.
#include <arpa/inet.h>      // Necessary for socket operations like htons, accept, etc.
#include <openssl/sha.h>    // Necessary for SHA256
#include <string.h>         // Necessary for memcpy
#include "hashTable.h"      // Custom hash table functi 

#define QUEUE_SIZE 50       // Number of pending connections in the connection queue
#define REQ_SIZE 49         // Size of the request packet
#define RESP_SIZE 8         // Size of the response packet

extern int server_fd, new_socket; // Server socket and new client socket
extern HashTable *hashTable;      // Hash table

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

int createServerTcpSocketAndListen(int port);    
int searchHashTableAndSendValueToClient(int *new_socket, request_packet *req, response_packet *resp, HashTable *hashTable);
int reverseHashUpdateHashTableAndSendValueToClient(int *new_socket, request_packet *req, response_packet *resp, HashTable *hashTable);
int acceptClientConnectionAndReadRequest(int *server_fd, int *new_socket, struct sockaddr_in *address, int *addrlen, request_packet *req);
int reverseHash(uint8_t target_hash[32], uint64_t *start, uint64_t *end, uint64_t *answer);
void handle_sigint(int sig);
#endif
