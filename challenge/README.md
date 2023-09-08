# Simple Server - OS Challenge

This project provides a simple server capable of accepting connections from clients, processing their requests, and sending responses back. It includes functionalities like reverse hashing and efficient searching using a hash table.

## Files Overview

- `simple_server.c`: Main server implementation file that sets up the server, listens for incoming client requests, processes them and sends back the response.
- `server_utils.h`: Header file containing the definitions of utilities used in the server operations.
- `server_utils.c`: Implementation of the server utility functions.

## Main Components

### simple_server.c

- **Global Variables**: 
  - Server sockets, request and response packets, and hash table.
  - Timing variables for verbose output.
  
- **Functions**:
  - `handle_sigint()`: Handles the Ctrl+C signal to gracefully shut down the server and free resources.
  - `main()`: Main function that sets up the server, reads command line arguments and processes client requests in a loop.

### server_utils.h

- **Data Structures**:
  - `request_packet`: Structure that holds the request details received from clients.
  - `response_packet`: Structure for sending back the response to the client.

- **Functions**:
  - Socket and connection related functions.
  - Hash table operations.

### server_utils.c

- Implementation of the utility functions that are used in the server for setting up the socket, reading client requests, processing them, and sending back the responses.

## Usage

To start the server:

```bash
./simple_server <port> <verbose print>
