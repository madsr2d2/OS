#include "extended_server_utils.h"

atomic_bool terminate_flag = false; 
int server_fd;

int main(int argc, char *argv[]) {
    // Check command line argument
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        fprintf(stderr, "  <port>: The port number to use (e.g., 8080)\n");
        exit(1);
    }

    int port = atoi(argv[1]);                       // Port number to use
    FIFOQueue queue;                                // FIFO queue
    initQueue(&queue);                              // Initialize queue
    pthread_t thread1, thread2;                     // Thread handles
    acceptConnectionsArgs args = {&queue, port};    // Arguments for the acceptConnections thread
    signal(SIGINT, signal_handler);                 // Register signal handler for SIGINT

    // Create thread for accepting connections
    if (pthread_create(&thread1, NULL, threadAcceptConnectionsHandler, &args)) {
        perror("Error creating thread.");
        return 1;
    }

    // Create thread for processing requests
    if (pthread_create(&thread2, NULL, threadProcessRequestsHandler, &queue)) {
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
