
#include "hashTable.h"

//unsigned int hash(const uint8_t *key) {
//    unsigned int hash = 0;
//    for (int i = 0; i < KEY_SIZE; ++i) {
//        hash = (hash << 7) + key[i];
//    }
//    return hash & 1023;
//}

unsigned int hash(const uint8_t *key) {
    return XXH64(key, KEY_SIZE, 0) % HASH_TABLE_SIZE;
}

void printKey(const uint8_t *key, size_t size) {
    for(size_t i = 0; i < size; ++i) {
        printf("%02x", key[i]);
    }
    printf("\n");
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
    memcpy(newNode->key, key, KEY_SIZE);
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
        if (memcmp(current->key, key, KEY_SIZE) == 0) {
            *value = current->value;
            return 0;  // 0 indicates success
        }
        current = current->next;
    }
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

