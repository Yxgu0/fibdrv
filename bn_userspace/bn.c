#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bn.h"
#include "bn_pool.h"

/* ------------------------ Helper MACROs ------------------------ */

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#ifndef SWAP
#define SWAP(x, y)           \
    do {                     \
        typeof(x) __tmp = x; \
        x = y;               \
        y = __tmp;           \
    } while (0)
#endif

#ifndef DIV_ROUNDUP
#define DIV_ROUNDUP(x, len) (((x) + (len) -1) / (len))
#endif

#define NEW_NODE(head, val)                           \
    ({                                                \
        _bn_node *node = mp_malloc(sizeof(_bn_node)); \
        if (node) {                                   \
            node->value = (val);                      \
            list_add_tail(&node->link, (head));       \
        }                                             \
    })

#define REMOVE_NODE(head)                                          \
    ({                                                             \
        _bn_node *node = list_entry((head)->prev, _bn_node, link); \
        list_del((head)->prev);                                    \
        mp_free(node);                                             \
    })

#define list_for_each_tail_entry(entry, head, member)                  \
    for (entry = list_entry((head)->prev, __typeof__(*entry), member); \
         &entry->member != (head);                                     \
         entry = list_entry(entry->member.prev, __typeof__(*entry), member))

/* ------------------------ Helper functions ------------------------ */

/* Count the leading zeros of src */
static int bn_clz(const bn *src)
{
    int count = 0;
    struct list_head *head = &src->number_head->link;
    _bn_node *node = NULL;

    list_for_each_tail_entry(node, head, link)
    {
        if (node->value) {
            // prevent undefined behavior when src = 0
            count += __builtin_clz(node->value);
            return count;
        } else {
            count += 32;
        }
    }

    return count;
}

/* Count the digits of most significant bit */
static int bn_msb(const bn *src)
{
    return src->size * 32 - bn_clz(src);
}

/*
 * Resize bn
 * return 0 on success, -1 on error
 * data lose IS neglected when shinking the size
 */
static int bn_resize(bn *src, size_t size)
{
    if (!src)
        return -1;
    if (size == 0)  // prevent realloc(0) = free, which will cause problem
        return bn_free(src);

    if (size > src->size) {
        for (int i = 0; i < size - src->size; i++)
            NEW_NODE(&src->number_head->link, 0);
        src->size = size;
    } else if (size < src->size) {
        for (int i = 0; i < src->size - size; i++)
            REMOVE_NODE(&src->number_head->link);
        src->size = size;
    } else if (size == src->size) {
        return 0;
    }

    return 0;
}

/* Set a 32-bits unsigned value to the src
 * return 0 on success, -1 on error
 */
static int bn_set_value(bn *src, uint32_t val)
{
    int result = bn_resize(src, 1);
    if (result < 0)
        return result;

    _bn_node *node = list_first_entry(&src->number_head->link, _bn_node, link);
    node->value = val;

    return 1;
}

/* ------------------------ bn API ------------------------ */

/*
 * Output bn to decimal string
 * Note: the returned string should be freed with the free()
 */
char *bn_to_string(const bn *src)
{
    // log10(x) = log2(x) / log2(10) ~= log2(x) / 3.322
    size_t len = (8 * sizeof(int) * src->size) / 3 + 2 + src->sign;
    char *s = malloc(len);
    char *p = s;
    memset(s, '0', len - 1);
    s[len - 1] = '\0';

    struct list_head *head = &src->number_head->link;
    _bn_node *node = NULL;
    list_for_each_tail_entry(node, head, link)
    {
        for (uint32_t d = 1U << 31; d; d >>= 1) {
            int carry = !!(d & node->value);
            for (int j = len - 2; j >= 0; j--) {
                s[j] += s[j] - '0' + carry;
                carry = (s[j] > '9');
                if (carry)
                    s[j] -= 10;
            }
        }
    }

    // skip leading zero
    while (p[0] == '0' && p[1] != '\0')
        p++;
    if (src->sign)
        *(--p) = '-';
    memmove(s, p, strlen(p) + 1);

    return s;
}

/*
 * Allocate a bn structure with @size list nodes.
 * The value of @size list nodes are initialized to +0
 * Note : there will be @size+1 list nodes in total,
          the additional one is the list head.
 */
bn *bn_alloc(size_t size)
{
    bn *new = malloc(sizeof(bn));
    new->size = size;
    new->sign = 0;
    new->number_head = mp_malloc(sizeof(_bn_node));
    INIT_LIST_HEAD(&new->number_head->link);
    for (int i = 0; i < size; i++) {
        NEW_NODE(&new->number_head->link, 0);
    }

    return new;
}

/*
 * Free entire bn data structure
 * return 0 on success, -1 on error
 */
int bn_free(bn *src)
{
    if (src == NULL)
        return -1;

    // _bn_node *head_node = src->number_head, *node = NULL;
    // list_for_each_entry (node, &head_node->link, link) {
    //     mp_free(node);
    // }
    struct list_head *head = &src->number_head->link;
    while (!list_empty(head))
        mp_free(list_last_entry(head, _bn_node, link));
    mp_free(list_entry(head, _bn_node, link));
    free(src);

    return 0;
}

/*
 * Copy the value from src to dest
 * return 0 on success, -1 on error
 */
int bn_cpy(bn *dest, bn *src)
{
    if (bn_resize(dest, src->size) < 0)
        return -1;

    struct list_head *head = &dest->number_head->link,
                     *dlink = dest->number_head->link.next,
                     *slink = src->number_head->link.next;
    for (; dlink != head; dlink = dlink->next, slink = slink->next) {
        _bn_node *dnode = list_entry(dlink, _bn_node, link),
                 *snode = list_entry(slink, _bn_node, link);
        dnode->value = snode->value;
    }
    dest->sign = src->sign;

    return 0;
}

/*
 * Compare length
 * return 1 if |a| > |b|
 * return -1 if |a| < |b|
 * return 0 if |a| = |b|
 */
int bn_cmp(const bn *a, const bn *b)
{
    if (a->size > b->size) {
        return 1;
    } else if (a->size < b->size) {
        return -1;
    } else {
        // Traverse from tail node, which contain the most significant bit
        struct list_head *head = &a->number_head->link,
                         *alink = a->number_head->link.prev,
                         *blink = b->number_head->link.prev;
        for (; alink != head; alink = alink->prev, blink = blink->prev) {
            _bn_node *anode = list_entry(alink, _bn_node, link),
                     *bnode = list_entry(blink, _bn_node, link);
            if (anode->value > bnode->value)
                return 1;
            if (anode->value < bnode->value)
                return -1;
        }

        return 0;
    }
}

/* Swap bn ptr */
void bn_swap(bn *a, bn *b)
{
    bn tmp = *a;
    *a = *b;
    *b = tmp;
}

/* Left bit shift on bn (maximun shift 31) */
void bn_lshift(bn *src, size_t shift)
{
    size_t z = bn_clz(src);
    shift %= 32;  // only handle shift within 32 bits atm
    if (!shift)
        return;

    if (shift > z)
        bn_resize(src, src->size + 1);

    uint64_t carry = 0;
    struct list_head *head = &src->number_head->link;
    _bn_node *node = NULL;
    list_for_each_entry (node, head, link) {
        carry = ((uint64_t) node->value << shift) + carry;
        node->value = carry;  // assigning lower 32 bits
        carry >>= 32;
    }
}
/* slr += lgr
 * Note : assuming size of slr <= size of lgr
 */
void bn_add_to_smaller(bn *slr, const bn *lgr)
{
    uint32_t carry = 0;
    struct list_head *shead = &slr->number_head->link,
                     *lhead = &lgr->number_head->link, **s = &shead->next,
                     **l = &lhead->next;

    for (;; l = &(*l)->next, s = &(*s)->next) {
        if (*s == shead) {
            if (*l == lhead) {
                if (carry) {
                    NEW_NODE(shead, 1);
                    slr->size++;
                }
                break;
            }

            NEW_NODE(shead, 0);
            slr->size++;
        }

        _bn_node *snode = list_entry(*s, _bn_node, link),
                 *lnode = list_entry(*l, _bn_node, link);

        uint64_t res = (uint64_t) snode->value + lnode->value + carry;
        snode->value = res;  // assigning lower 32 bits
        carry = (res >> 32);
    }
}

/* lgr -= slr
 * Note : assuming size of lgr >= size of slr
 */
void bn_sub_from_larger(bn *lgr, const bn *slr)
{
    int borrow = 0;
    struct list_head *shead = &slr->number_head->link,
                     *lhead = &lgr->number_head->link, *slink = shead->next,
                     *llink = lhead->next;

    for (; slink != shead; slink = slink->next, llink = llink->next) {
        _bn_node *snode = list_entry(slink, _bn_node, link),
                 *lnode = list_entry(llink, _bn_node, link);

        int64_t result = (int64_t) lnode->value - snode->value - borrow;
        lnode->value = result;  // truncate 64-bits negative to 32-bits positive

        if (result < 0)
            borrow = 1;
        else
            borrow = 0;
    }
    if (borrow) {
        _bn_node *lnode = list_entry(llink, _bn_node, link);
        lnode->value--;
    }

    // remove empty tail node, min node size = 1
    int e = bn_clz(lgr) / 32;
    if (e == lgr->size)
        --e;
    bn_resize(lgr, lgr->size - e);
}

/*
 * c = a x b
 * Note: work for c == a or c == b
 * using the simple quadratic-time algorithm (long multiplication)
 */
void bn_mult(bn *c, const bn *a, const bn *b)
{
    // max digits = sizeof(a) + sizeof(b))
    int d = bn_msb(a) + bn_msb(b);
    d = DIV_ROUNDUP(d, 32) + !d;  // round up, min size = 1

    // make it work properly when c == a or c == b
    bn *tmp;
    if (c == a || c == b) {
        tmp = c;  // save c
        c = bn_alloc(d);
    } else {
        tmp = NULL;
        bn_resize(c, d);
    }

    struct list_head *ahead = &a->number_head->link,
                     *bhead = &b->number_head->link,
                     *chead = &c->number_head->link;
    _bn_node *anode = NULL, *bnode = NULL, *cnode = NULL;

    // for each node in a[i], calculate b * a[i], the result is stored in c
    uint64_t carry = 0;
    int offset = -1;
    list_for_each_entry (anode, ahead, link) {
        struct list_head *clink = chead->next;
        offset++;
        for (int i = 0; i < offset; i++)
            clink = clink->next;

        list_for_each_entry (bnode, bhead, link) {
            cnode = list_entry(clink, _bn_node, link);
            carry =
                (uint64_t) anode->value * bnode->value + cnode->value + carry;
            cnode->value = carry;  // assigning lower 32 bits

            clink = clink->next;
            carry >>= 32;
        }
        while (carry) {
            cnode = list_entry(clink, _bn_node, link);
            carry = (uint64_t) cnode->value + carry;
            cnode->value = carry;  // assigning lower 32 bits

            clink = clink->next;
            carry >>= 32;
        }
    }

    c->sign = a->sign ^ b->sign;

    // remove empty tail node, min node size = 1
    int e = bn_clz(c) / 32;
    if (e == c->size)
        --e;
    bn_resize(c, c->size - e);

    if (tmp) {
        bn_cpy(tmp, c);  // restore c
        bn_free(c);
    }
}

/* Calculate F(n) and save it to dest */
void bn_fib(bn *dest, uint32_t n)
{
    if (n < 2) {  // Fib(0) = 0, Fib(1) = 1
        bn_resize(dest, 1);
        bn_set_value(dest, n);
        return;
    }

    bn *prev = bn_alloc(1);
    bn *curr = bn_alloc(1);
    bn_set_value(prev, 0);
    bn_set_value(curr, 1);

    for (unsigned int i = 1; i < n; i++) {
        bn_add_to_smaller(prev, curr);
        bn_swap(prev, curr);
    }

    bn_cpy(dest, curr);
    bn_free(prev);
    bn_free(curr);
}

/* Calculate F(n) by fast doubling and save it to dest */
void bn_fib_fdoubling(bn *dest, unsigned int n)
{
    if (n < 2) {  // Fib(0) = 0, Fib(1) = 1
        bn_resize(dest, 1);
        bn_set_value(dest, n);
        return;
    }

    bn *f1 = dest;        /* F(k) */
    bn *f2 = bn_alloc(1); /* F(k+1) */
    bn_set_value(f1, 0);
    bn_set_value(f2, 1);
    bn *k1 = bn_alloc(1);
    bn *k2 = bn_alloc(1);

    /* walk through the digit of n */
    for (unsigned int i = 1U << 31; i; i >>= 1) {
        /* F(2k) = F(k) * [ 2 * F(k+1) – F(k) ] */
        bn_cpy(k1, f2);
        bn_lshift(k1, 1);
        bn_sub_from_larger(k1, f1);
        bn_mult(k1, f1, k1);
        /* F(2k+1) = F(k)^2 + F(k+1)^2 */
        bn_mult(f1, f1, f1);
        bn_mult(f2, f2, f2);
        bn_cpy(k2, f1);
        bn_add_to_smaller(k2, f2);
        if (n & i) {
            bn_cpy(f1, k2);
            bn_cpy(f2, k1);
            bn_add_to_smaller(f2, k2);
        } else {
            bn_cpy(f1, k1);
            bn_cpy(f2, k2);
        }
    }
    // return f[0]
    bn_free(f2);
    bn_free(k1);
    bn_free(k2);
}