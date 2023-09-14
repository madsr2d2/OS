#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>



// a simple TCP server that sends the message back to the client and then closes the conenction
int main(int argc, char *argv[]) {

    // Exit if no port number is provided 
    if (argc != 2) {
        fprintf(stderr,"Usage: %s <port number>\n", argv[0]);
        exit(1);
    }
    int buffer_len = 256; // length of read buffer
    int sock_fd, newsock_fd, port_num; // Declare socket file descriptors and port - these are of type int
    struct sockaddr_in server_addr, cli_addr; // Declare socket address types for server and client sockets
    socklen_t addr_len = sizeof(server_addr);
    char buffer[buffer_len]; // Declare buffer 
    int n; 
    

    // Create socket - SOCKET_STREAM specifies a TCP socket
    if((sock_fd = socket(AF_INET, SOCK_STREAM,0)) < 0) {
        perror("ERROR opening socket");
        exit(1);
    }; 

    // Configure the server address 
    memset(&server_addr,0,addr_len); // Initialize all bytes in server_addr to 0 - this is good practice 
    server_addr.sin_family = AF_INET; // use the IPv4 protocol
    server_addr.sin_addr.s_addr = INADDR_ANY; // use any incoming address 
    server_addr.sin_port = htons(atoi(argv[1])); // set the server address port to port_num using network byte order (Big-endian).

    // binding the socket to the address
    if (bind(sock_fd, (struct sockaddr *) &server_addr, addr_len) < 0) { // the cast of server_addr from sockaddr_in to sockaddr is required  by the bind() function
        fprintf(stderr,"ERROR on bind\n");
        close(sock_fd);
        exit(1);
    }

    // start listening to the server socket
    listen(sock_fd, 5);
    if ((newsock_fd = accept(sock_fd, (struct sockaddr *) &cli_addr, &addr_len)) < 0) {
        fprintf(stderr,"ERROR on accept\n");
        close(sock_fd);
        exit(1);
    }

    // Initialize read buffer to zero - goog practice
    memset(buffer, 0 , buffer_len); 
    
    // read from newsock_fd
    if ((n = read(newsock_fd, buffer, buffer_len)) < 0) {
        fprintf(stderr, "ERROR on read\n");
        close(sock_fd);
        close(newsock_fd);
        exit(1);
    }

    // echo back to client
    write(newsock_fd, buffer, n);

    // print the message
    printf("The message was %s\n", buffer);

    // close sockets and exit
    close(sock_fd);
    close(newsock_fd);
    exit(0);
}