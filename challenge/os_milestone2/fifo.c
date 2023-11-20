#include "fifo.h"

void initQueue(FIFOQueue *queue) {
    queue->front = 0;  // Initialize the front index to the start of the queue.
    queue->rear = 0;   // Initialize the rear index to the start of the queue.
    pthread_mutex_init(&queue->mutex, NULL);  // Initialize the mutex for synchronizing access to the queue.
}

void enqueue(FIFOQueue *queue, int value) {
    pthread_mutex_lock(&queue->mutex);  // Lock the mutex to ensure thread-safe access.

    // Check if the queue is full. The queue is full if the rear is just behind the front.
    if ((queue->rear + 1) % FIFO_SIZE == queue->front) {
        perror("Error: Queue is full!");
        pthread_mutex_unlock(&queue->mutex);  // Unlock the mutex before returning from the function.
        return;
    }

    queue->data[queue->rear] = value;  // Place the new value at the rear of the queue.
    queue->rear = (queue->rear + 1) % FIFO_SIZE;  // Increment the rear index with wrap-around.

    pthread_mutex_unlock(&queue->mutex);  // Unlock the mutex after updating the queue.
}

int dequeue(FIFOQueue *queue) {
    pthread_mutex_lock(&queue->mutex);  // Lock the mutex to ensure thread-safe access.

    // Check if the queue is empty. The queue is empty if the front and rear are at the same position.
    if (queue->front == queue->rear) {
        pthread_mutex_unlock(&queue->mutex);  // Unlock the mutex before returning from the function.
        return -1;  // Return -1 to indicate that the queue is empty.
    }

    int value = queue->data[queue->front];  // Retrieve the value from the front of the queue.
    queue->front = (queue->front + 1) % FIFO_SIZE;  // Increment the front index with wrap-around.

    pthread_mutex_unlock(&queue->mutex);  // Unlock the mutex after removing the value from the queue.
    return value;  // Return the dequeued value.
}
