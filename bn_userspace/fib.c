/* Usage : ./fib <number> */

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

int main(int argc, char *argv[])
{
    bn *fbn = bn_alloc(1);

    bn_fib_fdoubling(fbn, atoi(argv[1]));

#ifdef OUTPUT
    char *p = bn_to_string(fbn);
    printf("%d: %s\n", atoi(argv[1]), p);
    free(p);
#endif

    bn_free(fbn);

#ifndef BN_ORIGIN
    mp_del();
#endif

    return 0;
}