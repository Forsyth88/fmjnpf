#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "rational.h"

void rat_add(rational_t* a, rational_t* b)
{
        if (a->den == b->den) {
                a->num += b->num;
                return;
        }
        a->num = a->num * b->den + b->num * a->den;
        a->den *= b->den;
}

void rat_sub(rational_t* a, rational_t* b)
{
        if (a->den == b->den) {
                a->num -= b->num;
                return;
        }
        a->num = a->num * b->den - b->num * a->den;
        a->den *= b->den;
        if (a->den < 0 && a->num < 0) {
                a->den *= -1;
                a->num *= -1;
        }
}

rational_t rat_sub_cpy(rational_t* a, rational_t* b)
{
        rational_t c;

        if (a->den == b->den) {
                c.num = a->num - b->num;
                c.den = a->den;
                return c;
        }
        c.num = a->num * b->den - b->num * a->den;
        c.den = a->den * b->den;
        if (c.den < 0 && c.num < 0) {
                c.den *= -1;
                c.num *= -1;
        }
        return c;
}

void rat_div(rational_t* a, rational_t* b)
{
        if (a->num == b->num) {
                a->num = b->den;
        } else if(a->den == b->den) {
                a->den = b->num;
        } else {
                a->num *= b->den;
                a->den *= b->num;
        }

        if ((a->num > 0 && a->den < 0) || (a->num < 0 && a->den < 0)) {
                a->num *= -1;
                a->den *= -1;
        }
}

void rat_mul(rational_t* a, rational_t* b)
{
        a->num *= b->num;
        a->den *= b->den;
}

bool rat_equals(rational_t* a, rational_t* b)
{
        int     diff;

        if (a->den == b->den) {
                diff = a->num - b->num;
                return (diff == 0);
        }
        diff = a->num * b->den - b->num * a->den;       
        return (diff == 0);
}

bool rat_neg(rational_t* a)
{
        return (a->den * a->num) < 0;
}

bool rat_pos(rational_t* a)
{
        return (a->den * a->num) > 0;
}

int rat_cmp(rational_t* a, rational_t* b)
{
        int     diff;

        if (a->den == b->den)
                diff = a->num - b->num;
        else
                diff = a->num * b->den - b->num * a->den;

        if (diff > 0)
                return 1;
        else if (diff < 0)
                return -1;
        else
                return 0;
}

bool rat_set(rational_t *a)
{
        return a->num != 0;
}

void rat_print(rational_t *a)
{
        printf("%d/%d\n", a->num, a->den);
}
