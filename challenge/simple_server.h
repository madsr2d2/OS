#ifndef SIMPLE_SERVER_H
#define SIMPLE_SERVER_H

#include <stdint.h>
#include <stdio.h>          // Necessary for fprintf, printf, etc.
#include <stdlib.h>         // Necessary for exit and atoi.
#include <string.h>         // Necessary for memcmp.
#include <unistd.h>         // Necessary for read, write, and close.
#include <arpa/inet.h>      // Necessary for socket operations like htons, accept, etc.
#include <signal.h>         // Necessary for signal handling.
#include "reverse_hash.h"   // Custom reverse hash function
#include "server_utils.h" // Custom TCP server setup functions
#include "hashTable.h"      // Custom hash table functions

#define REQ_SIZE 49
#define RESP_SIZE 8


#endif