#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>

#include "max_heap.h"
#include "reverse_hash.h"

#define REQ_SIZE 49

volatile sig_atomic_t keep_running = 1;
pthread_mutex_t max_heap_mutex = PTHREAD_MUTEX_INITIALIZER;
int server_fd;

void handle_sigint(int sig) {
    printf("Debug: SIGINT received. Shutting down...\n");
    keep_running = 0;
    close(server_fd);
}

struct response_packet {
    uint64_t answer;
};

void* listen_and_insert(void* arg) {
    printf("Debug: listen_and_insert thread started.\n");
    
    int server_fd = *((int*) arg);
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    while (keep_running) {
        printf("Debug: Waiting for new client...\n");
        
        int *new_socket_ptr = malloc(sizeof(int));

        
        if ((*new_socket_ptr = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("accept");
            free(new_socket_ptr);
            continue;
        }

        struct request_packet req;
        int valread = read(*new_socket_ptr, &req, REQ_SIZE);

        if (valread != REQ_SIZE) {
            perror("read");
            free(new_socket_ptr);
            continue;
        }

        printf("Debug: Inserting request into max heap...\n");
        
        pthread_mutex_lock(&max_heap_mutex);
        insert(req, *new_socket_ptr);
        pthread_mutex_unlock(&max_heap_mutex);

        printf("Debug: Insertion done. Unlocking mutex.\n");
    }

    printf("Debug: listen_and_insert thread is exiting.\n");
    return NULL;
}

void* process_and_respond(void* arg) {
    printf("Debug: process_and_respond thread started.\n");

    struct response_packet resp;

    while (keep_running) {
        // Lock the mutex to check if the heap is empty and, if not, to extract the max element
        pthread_mutex_lock(&max_heap_mutex);

        if (isEmpty()) {  // Assuming `isEmpty` is a function that checks if the heap is empty
            printf("Debug: Max heap is empty. Waiting for new requests...\n");
            pthread_mutex_unlock(&max_heap_mutex);
            usleep(1000);  // Sleep for a short time to prevent busy-waiting
            continue;
        }

        printf("Debug: About to extract from max heap...\n");
        
        struct heap_node highestPriorityNode = extractMax();
        
        pthread_mutex_unlock(&max_heap_mutex);

        printf("Debug: Performing reverse hash operation...\n");
        printf("Debug: highestPriorityNode.req.priority: %d...\n", highestPriorityNode.req.priority);
        printf("Debug: highestPriorityNode.req.start: %lu...\n", highestPriorityNode.req.start);
        printf("Debug: highestPriorityNode.req.end: %lu...\n", highestPriorityNode.req.end);
        uint64_t answer = reverse_hash(highestPriorityNode.req.hash, highestPriorityNode.req.start, highestPriorityNode.req.end);
        
        resp.answer = htobe64(answer);

        printf("Debug: Writing response to client...\n");
        int valwrite = write(highestPriorityNode.client_socket, &resp, sizeof(resp));

        if (valwrite != sizeof(resp)) {
            perror("write");
        }

        printf("Debug: Closing client socket.\n");
        close(highestPriorityNode.client_socket);
    }

    printf("Debug: process_and_respond thread is exiting.\n");
    return NULL;
}


int main(int argc, char *argv[]) {
    printf("Debug: Main function started.\n");

    if(argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    signal(SIGINT, handle_sigint);

    struct sockaddr_in address;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 50) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Debug: Creating threads...\n");

    pthread_t listen_thread, process_thread;

    if (pthread_create(&listen_thread, NULL, listen_and_insert, &server_fd) != 0) {
        perror("pthread_create for listen_thread");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&process_thread, NULL, process_and_respond, NULL) != 0) {
        perror("pthread_create for process_thread");
        exit(EXIT_FAILURE);
    }

    printf("Debug: Threads created. Waiting for them to finish...\n");

    pthread_join(listen_thread, NULL);
    pthread_join(process_thread, NULL);

    printf("Debug: Shutting down the server.\n");
    close(server_fd);

    return 0;
}
