#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdint.h>     // Necessary for uint8_t, uint16_t, etc.
#include <stdio.h>      // Necessary for fprintf, printf, etc.
#include <stdlib.h>     // Necessary for exit
#include <string.h>     // Necessary for memcpy
#include <pthread.h>    // Necessary for pthread_mutex_t

#define HASH_TABLE_SIZE 4096 // This value should be assigned according to the available memory
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

// Function prototypes
HashTable* createHashTable(void);
Node* createNode(const uint8_t *key, uint64_t value);
void insert(HashTable *hashTable, const uint8_t *key, uint64_t value);
int search(HashTable *hashTable, const uint8_t *key, uint64_t *value);
void freeHashTable(HashTable *hashTable);

#endif // HASHTABLE_H
