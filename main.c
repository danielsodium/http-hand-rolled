#include <stdio.h>

int main(void) {
    
    FILE *file = fopen("messages.txt", "r");
    if (file == NULL) {
        perror("Failed to open file\n");
        return 1;
    }
    
    const size_t buf_size = 8;
    unsigned char buf[buf_size+1];
    buf[buf_size] = '\0';
    size_t bytes_read;
    
    while((bytes_read = fread(buf, 1, buf_size, file)) != 0) {
        if (bytes_read != 8)
            buf[bytes_read] = '\0';
        printf("read: %s\n", buf);
    }

    return 0;
}