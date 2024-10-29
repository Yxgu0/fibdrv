#include <stdint.h>

#ifndef SYSPROG21_LIST_H
#include "list.h"
#endif

#ifndef BN_H
#include "bn.h"
#endif

typedef struct {
    _bn_node *free_list;
} _memory_pool;
static _memory_pool mp;

#ifndef DISABLE_MEMORY_POOLING
void *mp_malloc(uint32_t size);
void mp_free(void *src);
#else
#define mp_malloc(p) malloc((p));
#define mp_free(p) free((p));
#endif

void mp_del();
