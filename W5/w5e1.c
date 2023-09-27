#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    
    if (argc != 2){
        printf("Usage: %s <file name>\n",argv[0]);
        return(0);
    }

    if (access(argv[1], F_OK) == -1) {
        fprintf(stderr, "ERROR: %s does not exist\n.", argv[1]);
        return(1);
    }
    
    if (access(argv[1], R_OK) == 0){
        printf("Current user can read %s\n", argv[1]);
    }
    
    if (access(argv[1], W_OK) == 0){
        printf("Current user can write %s\n", argv[1]);
    }    

    if (access(argv[1], X_OK) == 0){
        printf("Current user can execute %s\n", argv[1]);
    }

    return 0;
}