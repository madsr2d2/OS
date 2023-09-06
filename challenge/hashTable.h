#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdint.h>

#define HASH_TABLE_SIZE 100 // This value should be assigned according to the available memory
#define KEY_SIZE 32

typedef struct Node {
    uint8_t key[KEY_SIZE];
    uint64_t value;
    struct Node *next;
} Node;

typedef struct HashTable {
    Node *table[HASH_TABLE_SIZE];
} HashTable;

HashTable* createHashTable(void);
Node* createNode(const uint8_t *key, uint64_t value);
void insert(HashTable *hashTable, const uint8_t *key, uint64_t value);
int search(HashTable *hashTable, const uint8_t *key, uint64_t *value);
void freeHashTable(HashTable *hashTable);

#endif // HASHTABLE_H
