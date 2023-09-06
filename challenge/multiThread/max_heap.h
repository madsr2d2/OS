#ifndef MAX_HEAP_H
#define MAX_HEAP_H

#include <stdint.h>
#define MAX_HEAP_SIZE 1000

#pragma pack(push, 1)
struct request_packet {
    uint8_t hash[32];
    uint64_t start;
    uint64_t end;
    uint8_t priority;
};
#pragma pack(pop)

struct heap_node {
    struct request_packet req;
    int client_socket;
};

void swap(struct heap_node *a, struct heap_node *b);
void heapify(int index);
void insert(struct request_packet req, int client_socket);
struct heap_node extractMax();
void initialize_heap();
int isEmpty();

#endif // MAX_HEAP_H
