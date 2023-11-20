#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <pthread.h>  // For threading functionalities like pthread_create, pthread_join, etc.

// Macro definitions
#define QUEUE_SIZE 1000        // Max number of pending connections in the connection queue.
#define REQ_SIZE 49            // Size of a request packet in bytes.
#define RESP_SIZE 8            // Size of a response packet in bytes.
#define FIFO_SIZE 10000        // Size of the FIFO queue.
#define HASH_TABLE_SIZE 4096   // Size of the hash table (should be a power of 2; here 2^12).
#define KEY_SIZE 32            // Size of the hash key in bytes.

// Node structure for the hash table.
typedef struct Node {
    uint8_t key[KEY_SIZE];  // Key array.
    uint64_t value;         // Associated value with the key.
    struct Node *next;      // Pointer to the next node (for handling collisions).
} Node;

// Hash table structure.
typedef struct HashTable {
    Node *table[HASH_TABLE_SIZE];               // Array of pointers to nodes (buckets).
    pthread_mutex_t mutexes[HASH_TABLE_SIZE];   // Array of mutexes for thread-safe operations on each bucket.
} HashTable;

// Request packet structure with no padding.
#pragma pack(push, 1)
typedef struct request_packet {
    uint8_t hash[32];    // Hash value.
    uint64_t start;      // Start range for calculation.
    uint64_t end;        // End range for calculation.
    uint8_t priority;    // Priority of the request.
    int client_fd;       // Client file descriptor.
} request_packet;
#pragma pack(pop)  // Restore the previous packing mode.

// FIFO queue structure.
typedef struct {
    request_packet data[FIFO_SIZE];  // Array for storing request packets.
    int front;                       // Index of the front element.
    int rear;                        // Index of the rear element.
    pthread_mutex_t mutex;           // Mutex for thread-safe enqueue and dequeue operations.
} FIFOQueue;

// Response packet structure.
typedef struct response_packet {
    uint64_t answer;  // Answer to the client's request.
} response_packet;

// Structure for arguments passed to the acceptConnections thread.
typedef struct {
    FIFOQueue *queue;        // Pointer to the FIFO queue.
    HashTable *hashTable;    // Pointer to the hash table.
    int port;                // Port number for the server.
} acceptConnectionsArgs;

// Structure for arguments passed to the processRequests thread.
typedef struct {
    FIFOQueue *queue;        // Pointer to the FIFO queue.
    HashTable *hashTable;    // Pointer to the hash table.
} processRequestsArgs;

#endif // TYPES_H
