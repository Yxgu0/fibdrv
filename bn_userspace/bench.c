#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef BN_ORIGIN
#include "bn.h"
#include "bn_pool.h"
#else
#include "bn_origin.h"
#endif

#define ITER 1000

int main(int argc, char *argv[])
{
    bn *fbn = bn_alloc(1);

    for (int i = 1; i < ITER + 1; i++) {
        bn_fib_fdoubling(fbn, i);
    }

    bn_free(fbn);
#ifndef BN_ORIGIN
    mp_del();
#endif
    return 0;
}