#include "max_heap.h"

struct heap_node maxHeap[MAX_HEAP_SIZE];
int heapSize = 0;

// Check if the heap is empty
int isEmpty() {
    return heapSize == 0;
}

void swap(struct heap_node *a, struct heap_node *b) {
    struct heap_node temp = *a;
    *a = *b;
    *b = temp;
}

void heapify(int index) {
    int largest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < heapSize && maxHeap[left].req.priority > maxHeap[largest].req.priority) {
        largest = left;
    }

    if (right < heapSize && maxHeap[right].req.priority > maxHeap[largest].req.priority) {
        largest = right;
    }

    if (largest != index) {
        swap(&maxHeap[index], &maxHeap[largest]);
        heapify(largest);
    }
}

void insert(struct request_packet req, int client_socket) {
    if (heapSize >= MAX_HEAP_SIZE) {
        return; // Heap is full
    }

    struct heap_node newNode;
    newNode.req = req;
    newNode.client_socket = client_socket;
    maxHeap[heapSize] = newNode;
    heapSize++;

    int i = heapSize - 1;
    while (i > 0 && maxHeap[(i - 1) / 2].req.priority < maxHeap[i].req.priority) {
        swap(&maxHeap[(i - 1) / 2], &maxHeap[i]);
        i = (i - 1) / 2;
    }
}

struct heap_node extractMax() {
    struct heap_node root = maxHeap[0];

    maxHeap[0] = maxHeap[heapSize - 1];
    heapSize--;

    heapify(0);

    return root;
}

void initialize_heap() {
    heapSize = 0;
}
