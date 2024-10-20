#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define FIB_DEV "/dev/fibonacci"

int main()
{
    char buf[100];
    char write_buf[] = "testing writing";
    int offset = 300;

    int fd = open(FIB_DEV, O_RDWR);
    if (fd < 0) {
        perror("Failed to open character device");
        exit(1);
    }

    for (int i = 0; i <= offset; i++) {
        lseek(fd, i, SEEK_SET);
        long long left = read(fd, buf, 1);
        if(left){
            printf("Reading from " FIB_DEV
               " at offset %d, returned message was truncated.\n", i);
        }else{
            printf("Reading from " FIB_DEV
               " at offset %d, returned the sequence "
               "%s.\n",
               i, buf);
        }
    }
    
    close(fd);
    return 0;
}
