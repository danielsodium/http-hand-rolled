#include <stdio.h>
#include <string.h>

int main(void) {
    
    FILE *file = fopen("messages.txt", "r");
    if (file == NULL) {
        perror("Failed to open file\n");
        return 1;
    }
    
    const size_t buf_size = 8;
    char buf[buf_size+1];
    buf[buf_size] = '\0';
    size_t bytes_read;
    
    const size_t max_line = 1024;
    char line[max_line];
    line[0] = '\0';
    
    while((bytes_read = fread(buf, 1, buf_size, file)) != 0) {
        if (bytes_read != 8) {
            buf[bytes_read] = '\0';
	    strcat(line, buf);
            printf("read: %s\n", line);
	    break;
        }

	char* split = strtok(buf, "\n");
	int breaks = 0;
	while (split != NULL) {
	    strcat(line, split);
	    split = strtok(NULL, "\n");
	    if (split != NULL) {
		printf("read: %s\n", line);
		line[0] = '\0';
	    }
	}
    }

    return 0;
}
