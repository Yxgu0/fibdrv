#include <linux/list.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/types.h>

#ifndef BN_KERNEL_H
#include "bn_kernel.h"
#endif

typedef struct {
    _bn_node *free_list;
} _memory_pool;
static _memory_pool mp;

#ifndef DISABLE_MEMORY_POOLING
void *mp_malloc(uint32_t size);
void mp_free(void *src);
#else
#define mp_malloc(p) kmalloc((p), GFP_KERNEL);
#define mp_free(p) kfree((p));
#endif

void mp_del(void);
