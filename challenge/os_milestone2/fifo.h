#ifndef FIFO_H
#define FIFO_H

#include <pthread.h>
#include <stdio.h>

#define FIFO_SIZE 50 // FIFO queue size

// Type definitions
typedef struct {
    int data[FIFO_SIZE];  // Example data array for the queue
    int front;
    int rear;
    pthread_mutex_t mutex;  // Mutex for thread-safe operations
} FIFOQueue;

// Function prototypes
void initQueue(FIFOQueue *queue);
void enqueue(FIFOQueue *queue, int value);
int dequeue(FIFOQueue *queue);

#endif
