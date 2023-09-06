#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashTable.h"

unsigned int hash(const uint8_t *key) {
    unsigned int hash = 0;
    while (*key) {
        hash = (hash << 5) + *key++;
    }
    return hash % HASH_TABLE_SIZE;
}

HashTable* createHashTable() {
    HashTable *newTable = (HashTable*)malloc(sizeof(HashTable));
    if (!newTable) {
        printf("Memory error\n");
        return NULL;
    }
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        newTable->table[i] = NULL;
    }
    return newTable;
}

Node* createNode(const uint8_t *key, uint64_t value) {
    Node *newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) {
        printf("Memory error\n");
        return NULL;
    }
    strncpy((char*)newNode->key, (char*)key, KEY_SIZE - 1);
    newNode->key[KEY_SIZE - 1] = '\0';  // Ensuring null termination
    newNode->value = value;
    newNode->next = NULL;
    return newNode;
}

void insert(HashTable *hashTable, const uint8_t *key, uint64_t value) {
    unsigned int index = hash(key);
    Node *newNode = createNode(key, value);
    if (!hashTable->table[index]) {
        hashTable->table[index] = newNode;
    } else {
        Node *current = hashTable->table[index];
        while (current->next) {
            current = current->next;
        }
        current->next = newNode;
    }
}

int search(HashTable *hashTable, const uint8_t *key, uint64_t *value) {
    unsigned int index = hash(key);
    Node *current = hashTable->table[index];
    while (current) {
        if (strncmp((char*)current->key, (char*)key, KEY_SIZE) == 0) {
            *value = current->value;
            return 0;  // 0 indicates success
        }
        current = current->next;
    }
    printf("Key not found\n");
    return -1;  // -1 indicates key not found error
}

void freeHashTable(HashTable *hashTable) {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        Node *current = hashTable->table[i];
        while (current) {
            Node *temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(hashTable);
}

