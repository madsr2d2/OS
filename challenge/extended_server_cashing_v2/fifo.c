#include "fifo.h"

void initQueue(FIFOQueue *queue) {
    queue->front = 0;
    queue->rear = 0;
    pthread_mutex_init(&queue->mutex, NULL);  // Initialize the mutex
}

void enqueue(FIFOQueue *queue, request_packet req) {
    pthread_mutex_lock(&queue->mutex);  // Lock the mutex

    if ((queue->rear + 1) % FIFO_SIZE == queue->front) { // Queue is full
        perror("Error: Queue is full!");
        pthread_mutex_unlock(&queue->mutex);  // Unlock the mutex before returning
        return;
    }

    queue->data[queue->rear] = req;
    queue->rear = (queue->rear + 1) % FIFO_SIZE;

    pthread_mutex_unlock(&queue->mutex);  // Unlock the mutex
}

int dequeue(FIFOQueue *queue, request_packet *req) {
    pthread_mutex_lock(&queue->mutex);  // Lock the mutex

    if (queue->front == queue->rear) {  // Queue is empty
        pthread_mutex_unlock(&queue->mutex);  // Unlock the mutex before returning
        return -1; // Return -1 to indicate error (empty queue)
    }

    *req = queue->data[queue->front];
    queue->front = (queue->front + 1) % FIFO_SIZE;

    pthread_mutex_unlock(&queue->mutex);  // Unlock the mutex
    return 0; // Return 0 to indicate success
}