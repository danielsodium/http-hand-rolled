#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "tcp.h"

int main(void) {
    pthread_t io_thread;
    Connection* connection = server_init();
    Channel* channel = server_listen(connection, &io_thread);

    char line[512];
    while (!channel_closed(channel)) {
	if (channel_pull(channel, line))
	    printf("read: %s\n", line);
    }
    free(channel);
    free(connection);

    return 0;
}
