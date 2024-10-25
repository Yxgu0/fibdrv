#include <stdint.h>

#include "list.h"

/*
 * Bignum data structure
 * number[0] contains least significant bits
 * number[size - 1] contains most significant bits
 * sign = 1 for negative number
 */
typedef struct __bn_node {
    uint32_t value;
    struct list_head link;
} _bn_node;

typedef struct _bn {
    _bn_node *number_head;
    size_t size;
    int sign;
} bn;

/*
 * Output bn to decimal string
 * Note: the returned string should be freed with the kfree()
 */
char *bn_to_string(const bn *src);

/*
 * Allocate a bn structure with the given size
 * the value is initialized to +0
 */
bn *bn_alloc(size_t size);

/*
 * Free entire bn data structure
 * return 0 on success, -1 on error
 */
int bn_free(bn *src);

/*
 * Copy the value from src to dest
 * return 0 on success, -1 on error
 */
int bn_cpy(bn *dest, bn *src);

/*
 * Compare length
 * return 1 if |a| > |b|
 * return -1 if |a| < |b|
 * return 0 if |a| = |b|
 */
int bn_cmp(const bn *a, const bn *b);

/* Swap bn ptr */
void bn_swap(bn *a, bn *b);

/* Left bit shift on bn (maximun shift 31) */
void bn_lshift(bn *src, size_t shift);

/* slr += lgr */
void bn_add_to_smaller(bn *slr, const bn *lgr);

/* lgr -= slr */
void bn_sub_from_larger(bn *lgr, const bn *slr);

/* c = a + b */
void bn_mult(bn *c, const bn *a, const bn *b);

/* Calculate F(n) and save it to dest */
void bn_fib(bn *dest, unsigned int n);

/* Calculate F(n) by fast doubling and save it to dest */
void bn_fib_fdoubling(bn *dest, unsigned int n);