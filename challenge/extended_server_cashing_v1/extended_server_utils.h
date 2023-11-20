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

// User-defined modules.
#include "fifo.h"        // FIFO queue definition and related operations.
#include "hashTable.h"   // Hash table definition and related operations.

// Macros for various sizes and limits.
#define QUEUE_SIZE 1000  // Maximum number of pending connections in the server's listening queue.
#define REQ_SIZE 49      // Size of a request packet in bytes.
#define RESP_SIZE 8      // Size of a response packet in bytes.

// Global flag to indicate when to terminate server operations.
extern atomic_bool terminate_flag;

// Structures for packet formats and thread arguments.

// Request packet structure
#pragma pack(push, 1)
typedef struct request_packet {
    uint8_t hash[32];   // Hash value.
    uint64_t start;     // Start of the computation range.
    uint64_t end;       // End of the computation range.
    uint8_t priority;   // Priority of the request.
} request_packet;
#pragma pack(pop)  // Restore the default packing alignment.

// Response packet structure.
typedef struct response_packet {
    uint64_t answer;    // Computed answer or result.
} response_packet;

// Structure for passing arguments to the connection acceptance thread.
typedef struct {
    FIFOQueue *queue;   // Pointer to the FIFO queue.
    int port;           // Port number to listen on.
} acceptConnectionsArgs;

// Structure for passing arguments to the request processing thread.
typedef struct {
    FIFOQueue *queue;      // Pointer to the FIFO queue.
    HashTable *hashTable;  // Pointer to the hash table.
} processRequestsArgs;

// Function prototypes for server operations and thread handlers.
/**
 * Creates and configures a TCP socket to listen on a specified port.
 * @param port The port number to listen on.
 * @param address The sockaddr_in structure to configure the server address.
 * @return A file descriptor for the created socket, or -1 on error.
 */
int createServerTcpSocketAndListen(int port, struct sockaddr_in *address);

/**
 * Attempts to reverse the given hash within a specified range and find the original value.
 * @param target_hash The hash to reverse.
 * @param start The start of the range to search.
 * @param end The end of the range to search.
 * @param answer Pointer to store the found value, if successful.
 * @return 0 if the hash was successfully reversed, 1 otherwise.
 */
int reverseHash(uint8_t target_hash[32], uint64_t *start, uint64_t *end, uint64_t *answer);

/**
 * Reverses a hash, updates the hash table with the result, and sends the result to the client.
 * @param client_fd Pointer to the client's file descriptor.
 * @param req The request packet containing the hash and range.
 * @param resp The response packet to be sent to the client.
 * @return 0 on success, 1 on failure.
 */
int reverseHashAndSendValueToClient(int *client_fd, request_packet *req, response_packet *resp);

/**
 * Reverses the hash specified in the request packet, updates the hash table with the result,
 * and sends the computed value back to the client.
 * @param client_fd Pointer to the client's file descriptor.
 * @param req The request packet containing the hash to be reversed.
 * @param resp The response packet to be sent back to the client.
 * @param hashTable Pointer to the hash table for updating with the reverse hash result.
 * @return 0 on successful execution and sending, 1 on failure to find the reverse hash, 
 *         or on failure to send the response to the client.
 */
int reverseHashUpdateHashTableAndSendValueToClient(int *client_fd, request_packet *req, response_packet *resp, HashTable *hashTable);

/**
 * Searches the hash table for the given key and sends the found value to the client.
 * @param client_fd Pointer to the client's file descriptor.
 * @param req The request packet containing the key to search.
 * @param resp The response packet to be sent to the client.
 * @param hashTable Pointer to the hash table.
 * @return 0 if the key is found, 1 if not found, 2 on write failure.
 */
int searchHashTableAndSendValueToClient(int *new_socket, request_packet *req, response_packet *resp, HashTable *hashTable);

/**
 * Reads a request packet from the client, ensuring all data is received.
 * @param client_fd The client's file descriptor.
 * @param req Pointer to the request packet structure.
 * @return 0 on success, 1 on recv error, 2 if the socket is closed before reading all data.
 */
int readRequestFromClient(int client_fd, request_packet *req);

/**
 * Thread function for accepting incoming connections and adding them to a FIFO queue.
 * @param args Pointer to acceptConnectionsArgs structure with queue and port information.
 * @return NULL always.
 */
void* handleConnectionAcceptanceThread(void* arg);

/**
 * Thread function for processing client requests.
 * @param arg Pointer to processRequestsArgs structure with queue and hash table information.
 * @return NULL always.
 */
void* handleRequestProcessingThread(void* arg);

/**
 * Handles the signal specified by 'signum'. Specifically designed to handle SIGINT (Ctrl+C).
 * This function sets the global 'terminate_flag' to 1 when a SIGINT signal is received.
 * 
 * @param signum The signal number to be handled. This function specifically checks for SIGINT.
 */
void signal_handler(int signum);

#endif // SERVER_UTILS_H
