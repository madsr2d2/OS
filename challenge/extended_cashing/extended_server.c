#include "extended_server_utils.h"

atomic_bool terminate_flag = false; 
HashTable *hashTable; // Global hash table

int server_fd;

int main(int argc, char *argv[]) {
    // Check command line argument
    if(argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        printf("<port>: The port number to use (e.g., 8080)\n");
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);                       // Port number to use
    FIFOQueue queue;                                // FIFO queue
    initQueue(&queue);                              // Initialize queue
    hashTable = createHashTable();                  // Create hash table
    pthread_t thread1, thread2;                     // Thread handles
    acceptConnectionsArgs args = {&queue, port};    // Arguments for the acceptConnections thread
    processRequestsArgs args2 = {&queue, hashTable};// Arguments for the processRequests thread

    // Register signal handler for SIGINT
    if (signal(SIGINT, signal_handler) == SIG_ERR) {
        perror("Error setting up signal handler.");
        exit(EXIT_FAILURE);
    }

    // Create thread for accepting connections
    if (pthread_create(&thread1, NULL, threadAcceptConnectionsHandler, &args)) {
        perror("Error creating thread.");
        return 1;
    }

    // Create thread for processing requests
    if (pthread_create(&thread2, NULL, threadProcessRequests_cashing_Handler, &args2)) {
        perror("Error creating thread.");
        return 1;
    }

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    // Destroy the mutex before exiting
    pthread_mutex_destroy(&queue.mutex);
    printf("Server is shutting down gracefully...\n");

    return 0;
}
