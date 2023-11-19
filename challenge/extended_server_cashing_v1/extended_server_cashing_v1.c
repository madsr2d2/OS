#include "extended_server_utils.h"

atomic_bool terminate_flag = 0; // Terminate flag 
HashTable *hashTable; // Global hash table
FIFOQueue queue; // Global FIFO queue

int main(int argc, char *argv[]) {
    if(argc != 2) { // Check command line argument
        printf("Usage: %s <port>\n", argv[0]);
        printf("<port>: The port number to use (e.g., 8080)\n");
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]); // Port number to use
    
    initQueue(&queue); // Initialize queue
    
    if((hashTable = createHashTable()) == NULL) { // Create hash table
        perror("Error creating hash table.");
        exit(EXIT_FAILURE);
    };                      
    
    pthread_t thread1, thread2; // Thread handles
    acceptConnectionsArgs args = {&queue, port}; // Arguments for the acceptConnections thread
    processRequestsArgs args2 = {&queue, hashTable}; // Arguments for the processRequests thread

    // Register signal handler for SIGINT
    if (signal(SIGINT, signal_handler) == SIG_ERR) {
        perror("Error setting up signal handler.");
        exit(EXIT_FAILURE);
    }

    // Create thread for accepting connections
    if (pthread_create(&thread1, NULL, handleConnectionAcceptanceThread, &args)) {
        perror("Error creating thread.");
        exit(EXIT_FAILURE);
    }

    // Create thread for processing requests
    if (pthread_create(&thread2, NULL, handleRequestProcessingThread, &args2)) {
        perror("Error creating thread.");
        exit(EXIT_FAILURE);
    }

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    // Free the hash table and destroy the queue mutex
    freeHashTable(hashTable);
    printf("Hash table freed.\n");
    pthread_mutex_destroy(&queue.mutex);
    printf("Queue mutex destroyed.\n");
    printf("Exiting gracefully...\n");

    return 0;
}
