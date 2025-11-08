#ifndef CHANNEL_H
#define CHANNEL_H

#include <sys/types.h>

#define CHANNEL_MAX_STRINGS 64
#define CHANNEL_MAX_LENGTH 512

typedef struct {
    pthread_mutex_t lock;
    char stream[CHANNEL_MAX_STRINGS][CHANNEL_MAX_LENGTH];
    int size;
    int current;
    int closed;
} Channel;

void channel_push(Channel* c, char* str);
int channel_pull(Channel* c, char* str);
void channel_close(Channel* c);
int channel_closed(Channel* c);

#endif
