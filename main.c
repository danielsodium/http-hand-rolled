#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

typedef struct {
    pthread_mutex_t lock;
    char stream[64][512];
    int size;
} Channel;

typedef struct {
    FILE* file;
    Channel* f_channel;
} LineArgs;

void* get_lines(void* arg) {
    LineArgs* args = (LineArgs *) arg;
    FILE* file = args->file;
    Channel* f_channel = args->f_channel;
    free(args);
    
    const size_t buf_size = 8;
    char buf[buf_size+1];
    buf[buf_size] = '\0';
    size_t bytes_read;
    
    const size_t max_line = 256;
    char line[max_line];
    line[0] = '\0';

    while((bytes_read = fread(buf, 1, buf_size, file)) != 0) {
        if (bytes_read != 8) {
            buf[bytes_read-1] = '\0';
	    strcat(line, buf);
	    pthread_mutex_lock(&f_channel->lock);
	    strcpy(f_channel->stream[f_channel->size], line);
	    pthread_mutex_unlock(&f_channel->lock);
	    break;
        }

	// strtok passes the first characters that are \0 or delimiter
	// manually check for the first character
	// there is definetly a cleaner way to organize this
	// but im not there yet

	if (buf[0] == '\n') {
	    pthread_mutex_lock(&f_channel->lock);
	    strcpy(f_channel->stream[f_channel->size], line);
	    f_channel->size++;
	    pthread_mutex_unlock(&f_channel->lock);
	    line[0] = '\0';
	}

	char* split = strtok(buf, "\n");
	int breaks = 0;
	while (split != NULL) {
	    strcat(line, split);
	    split = strtok(NULL, "\n");
	    if (split != NULL) {
		pthread_mutex_lock(&f_channel->lock);
		strcpy(f_channel->stream[f_channel->size], line);
		f_channel->size++;
		pthread_mutex_unlock(&f_channel->lock);
		line[0] = '\0';
	    }
	}
    }   
    return NULL;
}

Channel* start_io(char* f_name, pthread_t* thread) {
    FILE* file = fopen(f_name, "r");
    if (file == NULL)
       return NULL;

    Channel* f_channel = (Channel*) malloc(sizeof(Channel));
    if (f_channel == NULL)
	return NULL;

    LineArgs* line_args = malloc(sizeof(LineArgs));
    line_args->file = file;
    line_args->f_channel = f_channel;
    f_channel->size = 0;
    pthread_mutex_init(&f_channel->lock, NULL);

    pthread_create(thread, NULL, get_lines, (void*) line_args);

    return f_channel;
}

int main(void) {
    pthread_t io_thread;
    Channel* f_channel = start_io("messages.txt", &io_thread);
    pthread_join(io_thread, NULL);

    int i;
    for (i = 0; i <= f_channel->size; i++) {
	printf("read: %s\n", f_channel->stream[i]);
    }
    free(f_channel);

    return 0;
}
