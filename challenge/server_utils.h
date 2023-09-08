#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H

#include <stdint.h>         // Necessary for uint8_t, uint16_t, etc.
#include <unistd.h>         // Necessary for read, write, and close.
#include <arpa/inet.h>      // Necessary for socket operations like htons, accept, etc.
#include <openssl/sha.h>    // Necessary for SHA256
#include <string.h>         // Necessary for memcpy
#include "hashTable.h"      // Custom hash table functions

#define QUEUE_SIZE 50
#define REQ_SIZE 49
#define RESP_SIZE 8

// Request packet structure
#pragma pack(push, 1)
typedef struct request_packet {
    uint8_t hash[32];
    uint64_t start;
    uint64_t end;
    uint8_t priority;
} request_packet;
#pragma pack(pop)

// Response packet structure
typedef struct response_packet{
    uint64_t answer;
} response_packet;


int createServerTcpSocketAndListen(int port);    
int searchHashTableAndSendValueToClient(int *new_socket, request_packet *req, response_packet *resp, HashTable *hashTable);
int reverseHashUpdateHashTableAndSendValueToClient(int *new_socket, request_packet *req, response_packet *resp, HashTable *hashTable);
int acceptClientConnectionAndReadRequest(int *server_fd, int *new_socket, struct sockaddr_in *address, int *addrlen, request_packet *req);
int reverseHash(uint8_t target_hash[32], uint64_t *start, uint64_t *end, uint64_t *answer);
#endif
