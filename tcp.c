#include "tcp.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

Connection* server_init() {
    Connection* c = malloc(sizeof(Connection));

    c->client_len = sizeof(c->client_addr);

    c->server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (c->server_fd == -1) {
	printf("failed to create socket\n");
	return NULL;
    }

    // DEV: stopping TIME_WAIT
    int opt = 1;
    if (setsockopt(c->server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
	perror("setsockopt(SO_REUSEADDR) failed");
	exit(1);
    }

    c->server_addr.sin_family = AF_INET;
    c->server_addr.sin_addr.s_addr = INADDR_ANY;
    c->server_addr.sin_port = htons(PORT);

    if (bind(c->server_fd, (struct sockaddr*)&c->server_addr,
		sizeof(c->server_addr)) == -1) {
	perror("failed to bind");
	close(c->server_fd);
	exit(EXIT_FAILURE);
    }

    return c;
}
void* get_lines(void* arg) {
    LineArgs* args = (LineArgs *) arg;
    int client_fd = args->client_fd;
    int server_fd = args->server_fd;
    Channel* channel = args->channel;
    free(args);
    
    const size_t buf_size = 8;
    char buf[buf_size+1];
    buf[buf_size] = '\0';
    size_t bytes_read;
    
    const size_t max_line = 256;
    char line[max_line];
    line[0] = '\0';

    while((bytes_read = recv(client_fd, buf, buf_size, 0)) != 0) {
        if (bytes_read != 8) {
            buf[bytes_read] = '\0';
	    strcat(line, buf);
	    channel_push(channel, line);
	    break;
        }

	// strtok passes the first characters that are \0 or delimiter
	// manually check for the first character
	// there is definetly a cleaner way to organize this
	// but im not there yet

	if (buf[0] == '\n') {
	    channel_push(channel, line);
	    line[0] = '\0';
	}

	char* split = strtok(buf, "\n");
	int breaks = 0;
	while (split != NULL) {
	    strcat(line, split);
	    split = strtok(NULL, "\n");
	    if (split != NULL) {
		channel_push(channel, line);
		line[0] = '\0';
	    }
	}
    }   

    close(client_fd);
    close(server_fd);
    channel_close(channel);
    printf("Connection closed.\n");
    return NULL;
}

Channel* server_listen(Connection* c, pthread_t* thread) {

    char buffer[MAX_BUFFER];

    if (listen(c->server_fd, 5) == -1) {
	perror("Failed listening");
	close(c->server_fd);
	return NULL;
    }
    c->client_fd = accept(c->server_fd, (struct sockaddr*)&c->client_addr, &c->client_len);
    if (c->client_fd == -1) {
	perror("Failed accept");
	close(c->server_fd);
	return NULL;
    }

    Channel* channel = malloc(sizeof(Channel));
    LineArgs* line_args = malloc(sizeof(LineArgs));
    line_args->client_fd = c->client_fd;
    line_args->server_fd = c->server_fd;
    line_args->channel = channel;
    channel->size = 0;
    channel->closed = 0;

    pthread_mutex_init(&channel->lock, NULL);
    pthread_create(thread, NULL, get_lines, (void*) line_args);

    return channel;
}

