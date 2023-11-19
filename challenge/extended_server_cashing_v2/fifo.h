#ifndef FIFO_H
#define FIFO_H

#include <pthread.h>
#include <stdio.h>
#include "types.h"


// Function prototypes
void initQueue(FIFOQueue *queue);
void enqueue(FIFOQueue *queue, request_packet req);
int dequeue(FIFOQueue *queue, request_packet *req);

#endif
