#ifndef RATIONAL_H
#define RATIONAL_H

#include <stdbool.h>

typedef struct rational_t rational_t;

struct rational_t {
        int     num;
        int     den;
};

void rat_add(rational_t*, rational_t*);
void rat_sub(rational_t*, rational_t*);
rational_t rat_sub_cpy(rational_t*, rational_t*);
void rat_div(rational_t*, rational_t*);
void rat_mul(rational_t*, rational_t*);
bool rat_equals(rational_t*, rational_t*);
int  rat_cmp(rational_t*, rational_t*);
bool rat_neg(rational_t*);
bool rat_pos(rational_t*);
void rat_print(rational_t*);
bool rat_set(rational_t*);

#endif
