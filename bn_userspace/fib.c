/* Usage : ./fib <number> */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "bn.h"

int main(int argc, char *argv[])
{
    bn *fbn = bn_alloc(1);

    bn_fib_fdoubling(fbn, atoi(argv[1]));
    char *p = bn_to_string(fbn);
    printf("%d: %s\n", atoi(argv[1]), p);
    free(p);
    bn_free(fbn);
    return 0;
}