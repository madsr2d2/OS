#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdint.h>     // Necessary for uint8_t, uint16_t, etc.
#include <stdio.h>      // Necessary for fprintf, printf, etc.
#include <stdlib.h>     // Necessary for exit
#include <string.h>     // Necessary for memcpy
#include <pthread.h>    // Necessary for pthread_mutex_t
#include "types.h"      

// Function prototypes
HashTable* createHashTable(void);
Node* createNode(const uint8_t *key, uint64_t value);
void insert(HashTable *hashTable, const uint8_t *key, uint64_t value);
int search(HashTable *hashTable, const uint8_t *key, uint64_t *value);
void freeHashTable(HashTable *hashTable);

#endif // HASHTABLE_H