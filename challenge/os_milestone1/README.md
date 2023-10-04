# Simple Server - OS Challenge

In this project we build a simple server that can accept reverse hashing requests from clients, process the requests and send back the response. The server maintains a hash table data structure to store processed requests. The server is implemented in C and uses sockets for communication with the clients. This folder contains the source code for building the server.

## Usage

**Build the server:**
```bash
$ make
```

**Remove the build:**
```bash
$ make clean
```

**Run the server:**
```bash
$ ./simple_server <port> <verbose print>
```

- `<port>`:  Port number on which the server will listen for incoming connections.
- `<verbose print>`: 0 for no verbose output, 1 for verbose output.

## Files Overview

- `simple_server.c`: Main server implementation file that sets up the server, listens for incoming client requests, processes them and sends back the response.
- `server_utils.c`: Implementation of the server utility functions.
- `server_utils.h`: Header file containing struct definitions and function prototypes for the functions defined in `server_utils.c`.
- `hashTable.c`: Implementation of the hash table data structure.
- `hashTable.h`: Header file containing struct definitions and function prototypes for the functions defined in `hashTable.c`.
- `Makefile`: Makefile for building the server.

## Main   Components

### simple_server.c

- **Global Variables**: 
  - Server sockets, request and response packets, and hash table.
  - Timing variables for verbose output.
  
- **Functions**:
  - `main()`: Main function that sets up the server, reads command line arguments and processes client requests in a loop.

### server_utils.h

- **Data Structures**:
  - `request_packet`: Structure that holds the request details received from clients.
  - `response_packet`: Structure for sending back the response to the client.

- **Functions prototypes**:
  - `int createServerTcpSocketAndListen(int port)`: Creates a TCP socket and listens for incoming connections. Returns the socket file descriptor.
  - `int searchHashTableAndSendValueToClient(int *new_socket, request_packet *req, response_packet *resp, HashTable *hashTable)`: Searches the hash table for the request received from the client and sends back the value to the client. Returns 1 if the request is not found in the hash table, 0 otherwise.
  - `int reverseHashUpdateHashTableAndSendValueToClient(int *new_socket, request_packet *req, response_packet *resp, HashTable *hashTable)`: Reverses the hash, updates the hash table and sends back the value to the client. Returns 0 if successful.
  - `int acceptClientConnectionAndReadRequest(int *server_fd, int *new_socket, struct sockaddr_in *address, int *addrlen, request_packet *req)`: Accepts the client connection and reads the request from the client. Returns 0 if successful, 1 otherwise.
  - `int reverseHash(uint8_t target_hash[32], uint64_t *start, uint64_t *end, uint64_t *answer)`: Reverses the hash and stores the answer in the uint64_t *answer argument. Returns 0 if successful, 1 otherwise.
  - `void handle_sigint(int sig)`: Signal handler for SIGINT. Free's the hash table, closes the sockets and exits the program.

### server_utils.c

- Implementation of the utility function prototypes contained in `server_utils.h`.

### hashTable.h

- **Data Structures**:
  - `Node`: Structure for the nodes of the hash table.
  - `HashTable`: Structure for the hash table.

- **Functions prototypes**:
  - `HashTable* createHashTable(void)`: Creates a hash table and returns a pointer to it.
  - `Node* createNode(const uint8_t *key, uint64_t value)`: Creates a node for the hash table and returns a pointer to it.
  - `void insert(HashTable *hashTable, const uint8_t *key, uint64_t value)`: Inserts a node into the hash table.
  - `int search(HashTable *hashTable, const uint8_t *key, uint64_t *value)`: Searches the hash table for the key and returns the value in the uint64_t *value argument. Returns 0 if successful, 1 otherwise.
  - `void freeHashTable(HashTable *hashTable)`: Frees the hash table.

### hashTable.c

- Implementation function prototypes contained in `hashTable.h`.