#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Student struct
typedef struct Student {
    char name[20];
    int age;
    int id;
} Student;

//linked list fuction
typedef struct Node {
    int data;
    struct Node *next;
} Node;

// linked list insert fucntion
void insert(Node *head, int data) {
    Node *current = head;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = malloc(sizeof(Node));
    current->next->data = data;
    current->next->next = NULL;
}

// function to print list
void printList(Node *head){
   int i = 0;
   Node *current = head;
   while (current->next != NULL) {
    printf("Node %d val: %d\n", i, current->data);
    current = current->next;
    i++;
   } 
   printf("Node %d val: %d\n", i, current->data);
}


// main function
int main() {
    
    
    printf("Exercise 1:\n");
    // create a student
    Student s1;
    strcpy(s1.name, "StudentName");
    // alternative: s1.name = "StudentName";

    s1.age = 20;
    s1.id = 123456;

    // print student
    printf("Student name: %s\n", s1.name);
    printf("Student age: %d\n", s1.age);
    printf("Student id: %d\n", s1.id);


    printf("\nExercise 2:\n");

    // create a linked list
    Node *head = NULL;
    head = malloc(sizeof(Node));

    for (int i = 0; i < 3; i++){
        insert(head,i);
    }

    printList(head);



}
