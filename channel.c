#include "channel.h"

#include <pthread.h>
#include <string.h>

void channel_push(Channel* c, char* str) {
    // Possible put in a check here so that
    // Our channel buffer doesn't overflow
    pthread_mutex_lock(&c->lock);
    if (!c->closed) {
	strcpy(c->stream[c->size], str);
	c->size++;
    }
    pthread_mutex_unlock(&c->lock);
}
int channel_pull(Channel* c, char* str) {
    pthread_mutex_lock(&c->lock);
    if (c->size <= c->current) {
	pthread_mutex_unlock(&c->lock);
	return 0;
    }
    strcpy(str, c->stream[c->current]);
    c->current++;
    pthread_mutex_unlock(&c->lock);
    return 1;
}
void channel_close(Channel* c) {
    pthread_mutex_lock(&c->lock);
    c->closed = 1;
    pthread_mutex_unlock(&c->lock);
}

int channel_closed(Channel* c) {
    pthread_mutex_lock(&c->lock);
    int closed = c->closed;
    pthread_mutex_unlock(&c->lock);
    return closed;
}
