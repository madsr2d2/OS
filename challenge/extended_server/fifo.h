#ifndef FIFO_H
#define FIFO_H

#include <pthread.h>
#include <stdio.h>

#define FIFO_SIZE 10000 // Define the size of the FIFO queue.

// FIFOQueue structure definition.
typedef struct {
    int data[FIFO_SIZE];  // Array to store the queue elements.
    int front;            // Index of the front element in the queue.
    int rear;             // Index of the rear element in the queue.
    pthread_mutex_t mutex; // Mutex for synchronizing access to the queue.
} FIFOQueue;

/**
 * Initializes a FIFO queue to an empty state.
 * 
 * @param queue Pointer to the FIFOQueue structure to be initialized.
 */
void initQueue(FIFOQueue *queue);

/**
 * Adds a new element to the rear of the FIFO queue.
 * 
 * @param queue Pointer to the FIFOQueue structure.
 * @param value The integer value to be enqueued.
 */
void enqueue(FIFOQueue *queue, int value);

/**
 * Removes and returns an element from the front of the FIFO queue.
 * 
 * @param queue Pointer to the FIFOQueue structure.
 * @return The integer value at the front of the queue, or -1 if the queue is empty.
 */
int dequeue(FIFOQueue *queue);

#endif // FIFO_H
