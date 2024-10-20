#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define FIB_DEV "/dev/fibonacci"
#define FIB_ITER 0
#define FIB_FAST 1

int main()
{
    FILE *fp = fopen("./plot_iter_fast", "w");
    char buf[100];
    int offset = 100;
    int sampling_size = 1000;

    int fd = open(FIB_DEV, O_RDWR);
    if (fd < 0) {
        perror("Failed to open character device");
        exit(1);
    }

    // for each F(i), measure 'sampling_size' times of execution time
    // and remove outlier from 95% confident interval
    for (int i = 0; i <= offset; i++) {
        lseek(fd, i, SEEK_SET);

        double t1[sampling_size], t2[sampling_size];
        memset(t1, 0, sizeof(double) * sampling_size);
        memset(t2, 0, sizeof(double) * sampling_size);
        double mean1 = 0.0, sd1 = 0.0, result1 = 0.0;
        double mean2 = 0.0, sd2 = 0.0, result2 = 0.0;
        int count1 = 0, count2 = 0;

        // means
        for (int j = 0; j < sampling_size; j++) {
            read(fd, buf, FIB_ITER);
            t1[j] = (double) write(fd, buf, 0); // iterative
            read(fd, buf, FIB_FAST);
            t2[j] = (double) write(fd, buf, 0); // fast doubling
            mean1 += t1[j];
            mean2 += t2[j];
        }

        // mean
        mean1 /= sampling_size;
        mean2 /= sampling_size;

        // standard deviation
        for (int j = 0; j < sampling_size; j++) {
            sd1 += (t1[j] - mean1) * (t1[j] - mean1);
            sd2 += (t2[j] - mean2) * (t2[j] - mean2);
        }
        sd1 = sqrt(sd1 / (sampling_size - 1));
        sd2 = sqrt(sd2 / (sampling_size - 1));

        // 95% of confident interval
        for (int j = 0; j < sampling_size; j++) {
            if ((t1[j] >= (mean1 - 2 * sd1)) && (t1[j] <= (mean1 + 2 * sd1))) {
                result1 += t1[j];
                count1++;
            }
            if ((t2[j] >= (mean2 - 2 * sd2)) && (t2[j] <= (mean2 + 2 * sd2))) {
                result2 += t2[j];
                count2++;
            }
        }
        result1 /= count1;
        result2 /= count2;

        fprintf(fp, "%d %.5lf %.5lf, samples : %d, %d\n", i, result1, result2,
                count1, count2);
    }

    fclose(fp);
    close(fd);
    return 0;
}
