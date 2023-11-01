#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Define functions
void* tf1(void* arg) {
    unsigned long *tip = (unsigned long *) arg;
    printf("Hello from thread with ID [%lu]! I'm running tf1\n",tip);
    return NULL;
}

void* tf2(void* arg) {
    unsigned long *tip = (unsigned long *) arg;
    printf("Hello from thread with ID [%lu]! I'm running tf2\n",tip);
    return NULL;
}

void* tf3(void* arg) {
    unsigned long *tip = (unsigned long *) arg;
    printf("Hello from thread with ID [%lu]! I'm running tf3\n", tip);
    return NULL;
}

// Function pointer matching tf1, tf2 and tf3
typedef void* (*funcPtr)(void *);

int main() {

    funcPtr functionArray[3] = {tf1,tf2,tf3};
    pthread_t threads[3];
    int rc;

    for ( int i=0; i < 3; i++) {

        // Creat threads
        rc = pthread_create(&threads[i],NULL,functionArray[i], (void *)&threads[i]);
        if (rc) {
            printf("ERROR during pthread_creat");
            exit(1);
        }
    }
    
    // Join threads
    for (int i = 0; i < 3; i++ ) {
        pthread_join(threads[i],NULL);
    }

    return 0;
}
