#ifndef HASHTABLE_H
#define HASHTABLE_H

// Include standard libraries for data types, I/O operations, memory management, and threading.
#include <stdint.h>     // For standard integer types like uint8_t, uint64_t, etc.
#include <stdio.h>      // For input/output operations, e.g., printf.
#include <stdlib.h>     // For general utility functions like exit.
#include <string.h>     // For string manipulation functions like memcpy.
#include <pthread.h>    // For threading functions and types, e.g., pthread_mutex_t.

// Define macro constants for hash table configuration.
#define HASH_TABLE_SIZE 4096 // Size of the hash table. Should be a power of 2 for efficient hashing (e.g., 4096 = 2^12).
#define KEY_SIZE 32         // Size of the key in bytes, used in Node structure.

// Define the Node structure for the linked list in each bucket of the hash table.
typedef struct Node {
    uint8_t key[KEY_SIZE];  // Key array with a fixed size of KEY_SIZE bytes.
    uint64_t value;         // Value associated with the key.
    struct Node *next;      // Pointer to the next node in the list (for handling collisions).
} Node;

// Define the HashTable structure.
typedef struct HashTable {
    Node *table[HASH_TABLE_SIZE];                 // Array of pointers to the head nodes of linked lists (buckets).
    pthread_mutex_t mutexes[HASH_TABLE_SIZE];     // Array of mutexes, one for each bucket, for thread-safe operations.
} HashTable;

// Function prototypes for operations on the hash table.

/**
 * Creates a new, empty hash table.
 * 
 * @return Pointer to the newly created hash table, or NULL if memory allocation fails.
 */
HashTable* createHashTable(void);

/**
 * Creates a new node with the given key and value.
 * 
 * @param key Pointer to the key array.
 * @param value The value associated with the key.
 * @return Pointer to the newly created node, or NULL if memory allocation fails.
 */
Node* createNode(const uint8_t *key, uint64_t value);

/**
 * Inserts a key-value pair into the hash table.
 * 
 * @param hashTable Pointer to the hash table.
 * @param key Pointer to the key array.
 * @param value The value to be inserted.
 */
void insert(HashTable *hashTable, const uint8_t *key, uint64_t value);

/**
 * Searches for a key in the hash table and retrieves its associated value.
 * 
 * @param hashTable Pointer to the hash table.
 * @param key Pointer to the key array.
 * @param value Pointer to store the retrieved value.
 * @return 0 if the key is found (success), 1 otherwise (key not found).
 */
int search(HashTable *hashTable, const uint8_t *key, uint64_t *value);

/**
 * Frees all the resources associated with the hash table.
 * 
 * @param hashTable Pointer to the hash table to be freed.
 */
void freeHashTable(HashTable *hashTable);

#endif // HASHTABLE_H
