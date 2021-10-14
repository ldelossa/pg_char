#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int main(void) {
    int fd = open("/dev/pg_char0", O_RDONLY); 
    printf("fd %d\n", fd);
}
