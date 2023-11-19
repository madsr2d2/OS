#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <pthread.h>        // For pthread_create(), pthread_join(), etc.

#define QUEUE_SIZE 1000     // Number of pending connections in the connection queue
#define REQ_SIZE 49         // Size of the request packet
#define RESP_SIZE 8         // Size of the response packet
#define FIFO_SIZE 10000 // FIFO queue size
#define HASH_TABLE_SIZE 4096 // Must be a power of 2 (4096 = 2^12 )
#define KEY_SIZE 32         // Size of the key in bytes

// Node structure
typedef struct Node {
    uint8_t key[KEY_SIZE];
    uint64_t value;
    struct Node *next;
} Node;

// Hash table structure
typedef struct HashTable {
    Node *table[HASH_TABLE_SIZE];
    pthread_mutex_t mutexes[HASH_TABLE_SIZE];  // An array of mutexes for each bucket
} HashTable;

// Request packet structure
#pragma pack(push, 1) // exact fit - no padding
typedef struct request_packet {
    uint8_t hash[32];
    uint64_t start;
    uint64_t end;
    uint8_t priority;
    int client_fd;
} request_packet;
#pragma pack(pop) // back to whatever the previous packing mode was

// Type definitions
typedef struct {
    request_packet data[FIFO_SIZE];  // Example data array for the queue
    int front;
    int rear;
    pthread_mutex_t mutex;  // Mutex for thread-safe operations
} FIFOQueue;


// Response packet structure
typedef struct response_packet{
    uint64_t answer;
} response_packet;

// Arguments for the acceptConnections thread
typedef struct {
    FIFOQueue *queue;
    HashTable *hashTable;
    int port;
} acceptConnectionsArgs;

// Arguments for the processRequests thread
typedef struct {
    FIFOQueue *queue;
    HashTable *hashTable;
} processRequestsArgs;

#endif // TYPES_H
