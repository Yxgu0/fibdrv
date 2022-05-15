#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define FIB_DEV "/dev/fibonacci"
#define FIB_ITERATIVE 0
#define FIB_FAST 1

int main()
{
    FILE *fp = fopen("./plot_single", "w");

    char write_buf[100];
    int offset = 100;

    int fd = open(FIB_DEV, O_RDWR);
    if (fd < 0) {
        perror("Failed to open character device");
        exit(1);
    }

    for (int i = 0; i <= offset; i++) {
        lseek(fd, i, SEEK_SET);

        long long time1 = write(fd, write_buf, FIB_ITERATIVE);
        long long time2 = write(fd, write_buf, FIB_FAST);

        fprintf(fp, "%d %lld %lld\n", i, time1, time2);
    }

    close(fd);
    fclose(fp);
    return 0;
}
