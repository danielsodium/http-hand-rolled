#ifndef TCP_H
#define TCP_H

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>

#include "channel.h"

#define PORT 8080
#define MAX_BUFFER 8

typedef struct {
    int server_fd;
    int client_fd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_len;
} Connection;

typedef struct {
    int client_fd;
    int server_fd;
    Channel* channel;
} LineArgs;

Connection* server_init();
Channel* server_listen(Connection* c, pthread_t* thread);

#endif
