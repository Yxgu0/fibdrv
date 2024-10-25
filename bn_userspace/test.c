#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "bn.h"

#define ITER 100

int main(int argc, char *argv[])
{
    bn *fbn = bn_alloc(1);

    printf("bn_fib :\n");
    for (int i = 1; i < ITER + 1; i++) {
        bn_fib(fbn, i);
        char *p = bn_to_string(fbn);
        printf("%d: %s\n", i, p);
        free(p);
    }
    printf("\nbn_fib_fdoubling :\n");
    for (int i = 1; i < ITER + 1; i++) {
        bn_fib_fdoubling(fbn, i);
        char *p = bn_to_string(fbn);
        printf("%d: %s\n", i, p);
        free(p);
    }

    bn_free(fbn);
    return 0;
}