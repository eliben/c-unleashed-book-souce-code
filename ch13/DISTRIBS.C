/*
** The proper usage and copyright information for
** this software is covered in DSCRLic.TXT
** This code is Copyright 1999 by Dann Corbit
*/


/*
** Create a bunch of different kinds of distributions for testing
*/
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "distribs.h"
#include "mtrand.h"

static sortfile dn[] =
{
    {"constant", "constant.dbl", "constant.int", NULL, NULL},
    {"five", "five.dbl", "five.int", NULL, NULL},
    {"perverse", "perverse.dbl", "perverse.int", NULL, NULL},
    {"ramp", "ramp.dbl", "ramp.int", NULL, NULL},
    {"random", "random.dbl", "random.int", NULL, NULL},
    {"reverse", "reverse.dbl", "reverse.int", NULL, NULL},
    {"sorted", "sorted.dbl", "sorted.int", NULL, NULL},
    {"ten", "ten.dbl", "ten.int", NULL, NULL},
    {"trig", "trig.dbl", "trig.int", NULL, NULL},
    {"twenty", "twenty.dbl", "twenty.int", NULL, NULL},
    {"two", "two.dbl", "two.int", NULL, NULL},
    {NULL, NULL, NULL, NULL, NULL}
};

int             dopen(void)
{
    int             start = 0;
    while (dn[start].dtype) {
        if ((dn[start].fdbl = fopen(dn[start].dname, "rb")) == NULL) {
            printf("failed to open file %s for reading.\n", dn[start].dname);
            return 0;
        }
        if ((dn[start].fint = fopen(dn[start].iname, "rb")) == NULL) {
            printf("failed to open file %s for reading.\n", dn[start].iname);
            return 0;
        }
        start++;
    }
    return 1;
}

int             dclose(void)
{
    int             start = 0;
    while (dn[start].dtype) {
        fclose(dn[start].fdbl);
        fclose(dn[start].fint);
        start++;
    }
    return 1;
}

void            dsave(int a[], double d[], size_t n, const char *name)
{
    FILE           *fint;
    FILE           *fdbl;
    size_t          written;
    char            nameint[FILENAME_MAX];
    char            namedbl[FILENAME_MAX];
    sprintf(nameint, "%s.int", name);
    sprintf(namedbl, "%s.dbl", name);
    fint = fopen(nameint, "wb");
    if (!fint) {
        printf("Fatal error trying to open %s for writing.\n", nameint);
        exit(EXIT_FAILURE);
    }
    fdbl = fopen(namedbl, "wb");
    if (!fdbl) {
        printf("Fatal error trying to open %s for writing.\n", namedbl);
        exit(EXIT_FAILURE);
    }
    written = fwrite(a, sizeof(int), n, fint);
    if (written != n) {
        printf("tried to write %d items to %s but only wrote %d items\n", n, nameint, written);
    }
    written = fwrite(d, sizeof(double), n, fdbl);
    if (written != n) {
        printf("tried to write %d items to %s but only wrote %d items\n", n, namedbl, written);
    }
}
long            dload(int a[], double d[], size_t n, const char *name)
{
    size_t          ricount = 0;
    size_t          rdcount = 0;

    int             start = 0;
    if (!name)
        return 0;
    while (dn[start].dtype && strcmp(dn[start].dtype, name)) {
        start++;
    }

    if (dn[start].dtype) {
        rewind(dn[start].fint);
        ricount = fread(a, sizeof(int), n, dn[start].fint);
        if (ricount != n) {
            printf("tried to read %ld items from %s but only read %ld items\n", n, dn[start].dname, ricount);
        }
        rewind(dn[start].fdbl);
        rdcount = fread(d, sizeof(double), n, dn[start].fdbl);
        if (rdcount != n) {
            printf("tried to read %ld items from %s but only read %ld items\n", n, dn[start].iname, rdcount);
        }
        if (ricount != rdcount) {
            printf("inconsistent values for item counts %lu != %lu.\n", (unsigned long) ricount, (unsigned long) rdcount);
        }
    }
    return ricount > rdcount ? ricount : rdcount;
}

/*------------------------------------------------------------------*/
int             drandom(int old)
{
    static int      a = 16807,
                    m = 2147483647,
                    q = 127773,
                    r = 2836;
    int             inew;
    inew = a * (old % q) - r * (old / q);
    if (inew > 0)
        return inew;
    else
        return (inew + m);
}
/*------------------------------------------------------------------*/
void            init_random(int a[], double d[], int const n, int const seed, int const max)
{
    int             index,
                    the_int = seed;

    for (index = 0; index < n; index++) {
        the_int = drandom(the_int);
        a[index] = the_int % max;
        d[index] = a[index];
    }
}
/*------------------------------------------------------------------*/
void            init_two(int a[], double d[], int const n, int const seed)
{
    int             index,
                    the_int = seed;

    for (index = 0; index < n; index++) {
        the_int = drandom(the_int);
        if (the_int % 2 == 0)
            a[index] = 0;
        else
            a[index] = 1;

        d[index] = a[index];
    }
}
/*------------------------------------------------------------------*/
void            init_five(int a[], double d[], int const n, int const seed)
{
    int             index,
                    the_int = seed;
    int             value;

    for (index = 0; index < n; index++) {
        the_int = drandom(the_int);
        value = the_int % (5 * n);
        if (value < n)
            a[index] = 5;
        else if (value < 2 * n)
            a[index] = 4;
        else if (value < 3 * n)
            a[index] = 3;
        else if (value < 4 * n)
            a[index] = 2;
        else
            a[index] = 1;

        d[index] = a[index];
    }
}
/*------------------------------------------------------------------*/
void            init_ten(int a[], double d[], int const n, int const seed)
{
    int             index,
                    the_int = seed;
    int             value;

    for (index = 0; index < n; index++) {
        the_int = drandom(the_int);
        value = the_int % (10 * n);
        if (value < n)
            a[index] = 10;
        else if (value < 2 * n)
            a[index] = 9;
        else if (value < 3 * n)
            a[index] = 8;
        else if (value < 4 * n)
            a[index] = 7;
        else if (value < 5 * n)
            a[index] = 6;
        else if (value < 6 * n)
            a[index] = 5;
        else if (value < 7 * n)
            a[index] = 4;
        else if (value < 8 * n)
            a[index] = 3;
        else if (value < 9 * n)
            a[index] = 2;
        else
            a[index] = 1;

        d[index] = a[index];

    }
}
/*------------------------------------------------------------------*/
void            init_twenty(int a[], double d[], int const n, int const seed)
{
    int             index,
                    the_int = seed;
    int             value;

    for (index = 0; index < n; index++) {
        the_int = drandom(the_int);
        value = the_int % (20 * n);
        if (value < n)
            a[index] = 20;
        else if (value < 2 * n)
            a[index] = 19;
        else if (value < 3 * n)
            a[index] = 18;
        else if (value < 4 * n)
            a[index] = 17;
        else if (value < 5 * n)
            a[index] = 16;
        else if (value < 6 * n)
            a[index] = 15;
        else if (value < 7 * n)
            a[index] = 14;
        else if (value < 8 * n)
            a[index] = 13;
        else if (value < 9 * n)
            a[index] = 12;
        else if (value < 10 * n)
            a[index] = 11;
        else if (value < 11 * n)
            a[index] = 10;
        else if (value < 12 * n)
            a[index] = 9;
        else if (value < 13 * n)
            a[index] = 8;
        else if (value < 14 * n)
            a[index] = 7;
        else if (value < 15 * n)
            a[index] = 6;
        else if (value < 16 * n)
            a[index] = 5;
        else if (value < 17 * n)
            a[index] = 4;
        else if (value < 18 * n)
            a[index] = 3;
        else if (value < 19 * n)
            a[index] = 2;
        else
            a[index] = 1;

        d[index] = a[index];

    }
}
/*------------------------------------------------------------------*/
void            init_sorted(int a[], double d[], int const n, int const first, int const diff)
{
    int             index,
                    the_int = first;

    for (index = 0; index < n; index++) {
        a[index] = the_int;
        the_int += diff;
    }
}
/*------------------------------------------------------------------*/
void            init_reverse(int a[], double d[], int const n)
{
    int             index;

    for (index = 0; index < n; index++) {
        a[index] = n - index;
        d[index] = a[index];

    }
}
/*------------------------------------------------------------------*/
void            init_constant(int a[], double d[], int const n, int const value)
{
    int             index;
    for (index = 0; index < n; index++) {
        a[index] = value;
        d[index] = a[index];

    }
}
/*------------------------------------------------------------------*/
void            init_ramp(int a[], double d[], int const n, int const first, int const diff)
{
    int             index,
                    the_int = first,
                    mid = n / 2;

    for (index = 0; index < mid; index++) {
        a[index] = the_int;
        the_int += diff;
        d[index] = a[index];

    }
    for (index = mid; index < n; index++) {
        a[index] = the_int;
        the_int -= diff;
        d[index] = a[index];

    }
}
/*------------------------------------------------------------------*/
#define TWO_PI 6.283185307179586476925286766559
void            init_trig(int a[], double d[], int const n)
{
    int             i;
    double          dtheta = TWO_PI / 3600;
    double          cosdtheta = cos(dtheta);
    d[0] = 1.;
    d[1] = cosdtheta;
    for (i = 2; i < n; i++) {
        d[i] = 2 * d[i - 1] * cosdtheta - d[i - 2];
        a[i] = (int) (d[i] * 32000.0);
    }
}
void            init_perverse(int a[], double d[], int const n)
{
    int             index;
    for (index = 0; index < n; index++)
        switch (mtrand() % 4) {
        case 0:
            a[index] = INT_MAX;
            break;
        case 1:
            a[index] = INT_MIN;
            break;
        case 2:
            a[index] = 0;
            break;
        case 3:
            a[index] = mtrand();
            break;
        }


    for (index = 0; index < n; index++)
        switch (mtrand() % 8) {
        case 0:
            d[index] = DBL_MAX;
            break;
        case 1:
            d[index] = DBL_MIN;
            break;
        case 2:
            d[index] = -DBL_MAX;
            break;
        case 3:
            d[index] = -DBL_MIN;
            break;
        case 4:
            d[index] = 0;
            break;
        case 5:
            d[index] = -exp(99999.0);
            break;
        case 6:
            d[index] = exp(99999.0);
            break;
        case 7:
            d[index] = mtrand();
            break;
        }

}
void            create_distribs(double d[], int a[], size_t n)
{
    int             iseed = 7;
    init_constant(a, d, n, 100);
    dsave(a, d, n, "constant");
    init_five(a, d, n, iseed);
    dsave(a, d, n, "five");
    init_ramp(a, d, n, 0, 50);
    dsave(a, d, n, "ramp");
    init_random(a, d, n, iseed, INT_MAX);
    dsave(a, d, n, "random");
    init_reverse(a, d, n);
    dsave(a, d, n, "reverse");
    init_sorted(a, d, n, 0, 50);
    dsave(a, d, n, "sorted");
    init_ten(a, d, n, iseed);
    dsave(a, d, n, "ten");
    init_twenty(a, d, n, iseed);
    dsave(a, d, n, "twenty");
    init_two(a, d, n, iseed);
    dsave(a, d, n, "two");
    init_perverse(a, d, n);
    dsave(a, d, n, "perverse");
    init_trig(a, d, n);
    dsave(a, d, n, "trig");
}

void            make_distrib(double d[], int a[], size_t n, enum distribution_type which)
{
    int             iseed = 7;
    switch (which) {
    case constant:
        init_constant(a, d, n, 100);
        break;
    case five:
        init_five(a, d, n, iseed);
        break;
    case ramp:
        init_ramp(a, d, n, 0, 50);
        break;
    case haphazard:
        init_random(a, d, n, iseed, INT_MAX);
        break;
    case reverse:
        init_reverse(a, d, n);
        break;
    case sorted:
        init_sorted(a, d, n, 0, 50);
        break;
    case ten:
        init_ten(a, d, n, iseed);
        break;
    case twenty:
        init_twenty(a, d, n, iseed);
        break;
    case two:
        init_two(a, d, n, iseed);
        break;
    case perverse:
        init_perverse(a, d, n);
        break;
    case trig:
        init_trig(a, d, n);
        break;
    default:
        puts("Unexpected distribution request.\n");
    }
}
