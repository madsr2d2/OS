
#include "hashTable.h"

// Our own simple hash function
unsigned int hash(const uint8_t *key) {
    unsigned int hash = 0;
    for (int i = 0; i < KEY_SIZE; ++i) {
        hash += key[i];  // Simply adds up the values of each byte in the key
    }
    return hash % HASH_TABLE_SIZE;  // Uses modulo to limit the hash values to the range of 0 to HASH_TABLE_SIZE - 1
}

// Hash function using xxHash
//unsigned int hash(const uint8_t *key) {
//    return XXH64(key, KEY_SIZE, 0) % HASH_TABLE_SIZE;
//}


// Print key
void printKey(const uint8_t *key, size_t size) {
    for(size_t i = 0; i < size; ++i) {
        printf("%02x", key[i]);
    }
    printf("\n");
}

// Create a new hash table
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

// Create a new node
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

// Insert a new node into the hash table
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

// Search for a key in the hash table
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

// Free the hash table
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

