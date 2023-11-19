#include "fifo.h"

void initQueue(FIFOQueue *queue) {
    queue->front = 0;
    queue->rear = 0;
    pthread_mutex_init(&queue->mutex, NULL);  // Initialize the mutex
}

void enqueue(FIFOQueue *queue, int value) {
    pthread_mutex_lock(&queue->mutex);  // Lock the mutex

    if ((queue->rear + 1) % FIFO_SIZE == queue->front) { // Queue is full
        perror("Error: Queue is full!");
        pthread_mutex_unlock(&queue->mutex);  // Unlock the mutex before returning
        return;
    }

    queue->data[queue->rear] = value;
    queue->rear = (queue->rear + 1) % FIFO_SIZE;

    pthread_mutex_unlock(&queue->mutex);  // Unlock the mutex
}

int dequeue(FIFOQueue *queue) {
    pthread_mutex_lock(&queue->mutex);  // Lock the mutex

    if (queue->front == queue->rear) {         // Queue is empty
        pthread_mutex_unlock(&queue->mutex);  // Unlock the mutex before returning
        return -1; // Return -1 or another special value to indicate error
    }

    int value = queue->data[queue->front];
    queue->front = (queue->front + 1) % FIFO_SIZE;

    pthread_mutex_unlock(&queue->mutex);  // Unlock the mutex
    return value;
}