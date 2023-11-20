#include "hashTable.h"

unsigned int hash(const uint8_t *key) {
    unsigned int hash = 0;
    for (int i = 0; i < KEY_SIZE; ++i) {
        hash += key[i]; // Summing all the bytes of the key to compute the hash
    }
    return hash & (HASH_TABLE_SIZE - 1); // Ensuring hash value is within table size limits
}

HashTable* createHashTable() {
    HashTable *newTable = (HashTable*)calloc(1, sizeof(HashTable));
    if (!newTable) return NULL; // If memory allocation fails, return NULL

    // Initialize mutexes for each bucket in the hash table
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        if (pthread_mutex_init(&newTable->mutexes[i], NULL) != 0) {
            free(newTable); // Free allocated table if mutex initialization fails
            return NULL;
        }
    }
    return newTable;
}

Node* createNode(const uint8_t *key, uint64_t value) {
    Node *newNode = (Node*)calloc(1,sizeof(Node));
    if (!newNode) {
        perror("Node creation failed");
        return NULL; // Return NULL if memory allocation fails
    }

    memcpy(newNode->key, key, KEY_SIZE); // Copying the key into the node
    newNode->value = value; // Assigning the value to the node
    return newNode;
}

void insert(HashTable *hashTable, const uint8_t *key, uint64_t value) {
    unsigned int index = hash(key); // Calculate hash index for the key

    Node *newNode = createNode(key, value); // Create a new node
    pthread_mutex_lock(&hashTable->mutexes[index]); // Lock the mutex for thread safety

    // Insert the new node into the hash table
    if (!hashTable->table[index]) {
        hashTable->table[index] = newNode; // Insert as the first node in the bucket
    } else {
        // Insert at the end of the bucket list
        Node *current = hashTable->table[index];
        while (current->next) current = current->next;
        current->next = newNode;
    }

    pthread_mutex_unlock(&hashTable->mutexes[index]); // Unlock the mutex
}

int search(HashTable *hashTable, const uint8_t *key, uint64_t *value) {
    unsigned int index = hash(key); // Calculate hash index for the key

    Node *current = hashTable->table[index];
    while (current) { // Traverse the linked list at the bucket
        if (memcmp(current->key, key, KEY_SIZE) == 0) {
            *value = current->value;
            return 0; // Key found, return success
        }
        current = current->next;
    }
    return 1; // Key not found, return failure
}

void freeHashTable(HashTable *hashTable) {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        Node *current = hashTable->table[i];

        // Destroy the mutex for the current bucket
        pthread_mutex_destroy(&hashTable->mutexes[i]);

        // Free the nodes in the current bucket
        while (current) {
            Node *temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(hashTable); // Finally, free the hash table itself
}