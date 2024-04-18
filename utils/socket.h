#ifndef SOCKET_UTIL
#define SOCKET_UTIL

#include <arpa/inet.h>
#include <malloc.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

struct sockaddr_in* createAddress(char* ip, int port);
int createSocket();

#endif
