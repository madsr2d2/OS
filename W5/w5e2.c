#include <stdio.h>
#include <unistd.h>

int main() {

    int pid = fork();

    if (pid == -1) {
        dprintf(STDERR_FILENO,"ERROR on fork call");
    } 
    else if (pid == 0)
    {
        printf("Im the child process and I have process ID [%d]\n", getpid());
    } 
    else
    {
        printf("Im the parrent process and I have process ID [%d]\n", getpid());
    }

    return 0;
}