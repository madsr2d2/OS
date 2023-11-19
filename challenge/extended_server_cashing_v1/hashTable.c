#include "hashTable.h"

// Hash function
unsigned int hash(const uint8_t *key) {
    unsigned int hash = 0;
    
    for (int i = 0; i < KEY_SIZE; ++i) hash += key[i]; // Add all the bytes of the key
    
    return hash & (HASH_TABLE_SIZE - 1); // Return the hash value
}


// Create a new hash table 
HashTable* createHashTable() {
    HashTable *newTable = (HashTable*)calloc(1, sizeof(HashTable));
   
    if (!newTable) return NULL; // Handle case when calloc fails

    for (int i = 0; i < HASH_TABLE_SIZE; i++) 
        if (pthread_mutex_init(&newTable->mutexes[i], NULL) != 0) {
            free(newTable); // Free the hash table if mutex initialization fails
            return NULL;
        }
   
    return newTable;
}

// Create a new node
Node* createNode(const uint8_t *key, uint64_t value) {
    Node *newNode = (Node*)calloc(1,sizeof(Node));
   
    if (!newNode) {
        perror("Node creation failed");
        return NULL; // Handle case when newNode creation fails
    }
   
    // Copy the key and value into the new node
    memcpy(newNode->key, key, KEY_SIZE);
    newNode->value = value;
   
    return newNode;
}

// Insert a new node into the hash table
void insert(HashTable *hashTable, const uint8_t *key, uint64_t value) {
    unsigned int index = hash(key); // Get the index of the bucket

    Node *newNode = createNode(key, value); // Create a new node

    pthread_mutex_lock(&hashTable->mutexes[index]);  // Lock the mutex for the bucket

    // Insert the new node into the bucket
    if (!hashTable->table[index]) hashTable->table[index] = newNode;
    else {
        Node *current = hashTable->table[index];    
        while (current->next) current = current->next;

        current->next = newNode;
    }

    pthread_mutex_unlock(&hashTable->mutexes[index]);  // Unlock the mutex
}

// Search for a key in the hash table
int search(HashTable *hashTable, const uint8_t *key, uint64_t *value) {
    unsigned int index = hash(key);
    
    Node *current = hashTable->table[index];
    
    while (current) { // Search for the key in the bucket
        if (memcmp(current->key, key, KEY_SIZE) == 0) {
            *value = current->value;
            return 0;  // 0 indicates success
        }
        current = current->next;
    }

    return 1;  // 1 indicates key not found error
}

// Free the hash table
void freeHashTable(HashTable *hashTable) {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        Node *current = hashTable->table[i];

        // destroy the mutex
        pthread_mutex_destroy(&hashTable->mutexes[i]);

        // Free the linked list
        while (current) {
            Node *temp = current;
            current = current->next;
            free(temp);
        }
    }

    free(hashTable);   
}

