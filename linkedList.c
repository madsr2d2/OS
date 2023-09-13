#include <stdlib.h>
#include <stdio.h>

typedef struct Node {
    int data;
    struct Node *next;
} Node;

Node* createNode (int nodeData) {
    Node *newNode = malloc(sizeof(Node));
    newNode->data = nodeData;
    newNode->next = NULL;
    return newNode;
}

void insert(Node *head, int nodeData) {
    Node *current = head;
    while (current->next != NULL){
        current = current->next;
    }
    current->next = createNode(nodeData);
}

void printLinkedList(Node *head) {
    int i = 0;
    Node *current = head;
    while(current->next != NULL) {
        printf("Node %d has data %d\n", i++, current->data);
        current = current->next;
    }
    printf("Node %d has data %d\n", i++, current->data);
}

void freeList(Node *head) {
    Node *current = head;
    Node *nextNode; 
    while (current->next != NULL) {
        nextNode = current->next;
        free(current);
        current = nextNode;
    }
    free(current);
}

int main() {
    Node *head = createNode(1); // Create linked list 

    for (int i = 1; i < 10; i++) {
        insert(head, i+1);
    }

    printLinkedList(head);
    freeList(head);

    return 0;
}

