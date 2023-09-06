#ifndef TCPSERVERSETUP_H
#define TCPSERVERSETUP_H

#include <arpa/inet.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#define QUEUE_SIZE 50

int createServerTcpSocketAndListen(int port);    

#endif
