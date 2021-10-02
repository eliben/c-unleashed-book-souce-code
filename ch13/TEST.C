/*
** The proper usage and copyright information for
** this software is covered in DSCRLic.TXT
** This code is Copyright 1999 by Dann Corbit
*/


/*
** A test driver for sort routines.
** For small data sets, a profiler is needed because the
** routines are much faster than the resolution of clock()
*/
#include <limits.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include "inteltyp.h"
#include "distribs.h"
#include "genproto.h"
#include "mtrand.h"

#ifdef WIN32
#define CDECL __cdecl
#else
#define CDECL
#endif




static const char *dlist[] =
{
    "constant",
    "five",
    "perverse",
    "ramp",
    "random",
    "reverse",
    "sorted",
    "ten",
    "trig",
    "twenty",
    "two",
    NULL
};


/*
   **  Functions to time another function
   **  WARNING: Static variables are modified, so this code is NOT REENTRANT!
   **  Do not use this code in multi-threading programs!
 */

static clock_t  c_start,
                c_now;
static time_t   start,
                now;
static const double dclocks_per_sec = CLOCKS_PER_SEC;
void            reset_timer()
{
    start = time(NULL);
    c_start = clock();
}

double          dTotal = 0;
double          elapsed_time_since_reset(const char *message)
{
    double          delta;
    double          fract;
    now = time(NULL);
    c_now = clock();
    delta = difftime(now, start);
    fract = (c_now - c_start) / dclocks_per_sec;
    if (delta > 10000) {
        if (*message)
            printf("%s:%g\n", message, delta);
        dTotal += delta;
    } else {
        if (*message)
            printf("%s:%g\n", message, fract);
        dTotal += fract;
    }
    start = now;
    c_start = c_now;
    return delta;
}


void            dup(int la[], double da[], int count)
{
    int             i;
    for (i = 0; i < count; i++)
        da[i] = (double) la[i];
}

#define MAX_PAR 100
int             main(int argc, char **argv)
{
    int             k;
    double          dt[512];
    int             it[512];
    long            COUNT = 1000000L;
    long            pmin,
                    pmax;
    unsigned long   cycles = 0;
    long            iterations;
    size_t          count;
    size_t          pass;
    char            pause[3];
    int             which = 0;
    int             sorttype;
    int             iseed = 7;
    int            *iarray;
    double         *darray;
    double          bd = 0,
                    hd = 0,
                    sd = 0,
                    id = 0,
                    qd = 0,
                    ld = 0,
                    pd = 0,
                    md = 0;
    double          bi = 0,
                    si = 0,
                    hi = 0,
                    ii = 0,
                    qi = 0,
                    li = 0,
                    pi = 0,
                    mi = 0;

    partition       pset[MAX_PAR] = {0};

    enum distribution_type d = constant;
    if (argc > 1) {
        COUNT = atoi(argv[1]);
        if (COUNT < 1) {
            puts("Count must be >= 1");
            exit(EXIT_FAILURE);
        }
    }
    iarray = malloc(COUNT * sizeof(int));
    darray = malloc(COUNT * sizeof(double));
    if (!iarray || !darray) {
        puts("Error allocating arrays for sort tests.");
        exit(EXIT_FAILURE);
    }
    if (argc > 3) {
        pmin = atol(argv[2]);
        pmax = atol(argv[3]);
    } else {
        pmin = 2;
        pmax = COUNT;
    }
    mtsrand(4357U);
    printf("pmin = %ld, pmax = %ld\n", pmin, pmax);
    printf("Sort type            (n) Batch Shell Insert  Quick RadixL RadixM  Heap Merge\n");
    for (pass = pmin; pass <= pmax;) {
#ifdef _DEBUG
        iterations = 1;
#else
        iterations = (long) (1e4 / (log(pass)));
#endif
        if (iterations < 1)
            iterations = 1;
#ifdef _DEBUG
        printf("pass = %ld, iterations = %ld\n", pass, iterations);
#endif
        count = pass;
        while (d < unknown) {
#ifdef _DEBUG
            printf("type=%s, element count = %ld, interations = %ld\n", dlist[which], count, iterations);
#endif

            for (sorttype = 0; sorttype < 5; sorttype++) {
                cycles++;
                make_distrib(darray, iarray, pass, d);
                if (count <= 64) {
                    memcpy(dt, darray, count * sizeof dt[0]);
                    memcpy(it, iarray, count * sizeof it[0]);

                    switch (sorttype) {
                    case 0:
#ifdef _DEBUG
                        if ((cycles + 1) % 2000 == 0)
                            putchar('i');
#endif
                        for (k = 0; k < iterations; k++) {
                            memcpy(dt, darray, count * sizeof dt[0]);
                            memcpy(it, iarray, count * sizeof it[0]);
                            reset_timer();
                            InsertionSort_si(it, count);
                            ii += elapsed_time_since_reset("");

                            reset_timer();
                            InsertionSort_d(dt, count);
                            id += elapsed_time_since_reset("");
                            if (!InSort_si(it, count - 1))
                                puts("NOT SORTED");
                            if (!InSort_d(dt, count - 1))
                                puts("NOT SORTED");
                        }

                        break;
                    case 1:
#ifdef _DEBUG
                        if ((cycles + 1) % 2000 == 0)
                            putchar('b');
#endif
                        for (k = 0; k < iterations; k++) {
                            memcpy(dt, darray, count * sizeof dt[0]);
                            memcpy(it, iarray, count * sizeof it[0]);
                            reset_timer();
                            Batcher_si(it, count);
                            bi += elapsed_time_since_reset("");
                            reset_timer();
                            Batcher_d(dt, count);
                            bd += elapsed_time_since_reset("");
                            if (!InSort_si(it, count - 1))
                                puts("NOT SORTED");
                            if (!InSort_d(dt, count - 1))
                                puts("NOT SORTED");
                        }
                        break;
                    case 2:
#ifdef _DEBUG
                        if ((cycles + 1) % 2000 == 0)
                            putchar('s');
#endif
                        for (k = 0; k < iterations; k++) {
                            memcpy(dt, darray, count * sizeof dt[0]);
                            memcpy(it, iarray, count * sizeof it[0]);
                            reset_timer();
                            Shellsort_si(it, count);
                            si += elapsed_time_since_reset("");
                            reset_timer();
                            Shellsort_d(dt, count);
                            sd += elapsed_time_since_reset("");
                            if (!InSort_si(it, count - 1))
                                puts("NOT SORTED");
                            if (!InSort_d(dt, count - 1))
                                puts("NOT SORTED");
                        }
                        break;
                    }
                }
                switch (sorttype) {
                case 0:
                    for (k = 0; k < iterations; k++) {
                        make_distrib(darray, iarray, pass, d);
                        reset_timer();
                        Iqsort5_si(iarray, count);
                        qi += elapsed_time_since_reset("");
                        reset_timer();
                        Iqsort5_d(darray, count);
                        qd += elapsed_time_since_reset("");
                        if (!InSort_si(iarray, count - 1))
                            puts("NOT SORTED");
                        if (!InSort_d(darray, count - 1))
                            puts("NOT SORTED");
                    }
                    break;
                case 1:
                    for (k = 0; k < iterations; k++) {
                        make_distrib(darray, iarray, pass, d);
                        reset_timer();
                        RadixLsd_si(iarray, 0, count - 1, 0);
                        li += elapsed_time_since_reset("");
                        reset_timer();
                        RadixLsd_d(darray, 0, count - 1, 0);
                        ld += elapsed_time_since_reset("");
                        if (!InSort_si(iarray, count - 1))
                            puts("NOT SORTED");
                        if (!InSort_d(darray, count - 1))
                            puts("NOT SORTED");
                    }
                    break;
                case 2:
                    for (k = 0; k < iterations; k++) {
                        make_distrib(darray, iarray, pass, d);
                        reset_timer();
                        heapsort_si(iarray, count);
                        hi += elapsed_time_since_reset("");
                        reset_timer();
                        heapsort_d(darray, count);
                        hd += elapsed_time_since_reset("");
                        if (!InSort_si(iarray, count - 1))
                            puts("NOT SORTED");
                        if (!InSort_d(darray, count - 1))
                            puts("NOT SORTED");
                    }
                    break;

                case 3:
                    for (k = 0; k < iterations; k++) {
                        make_distrib(darray, iarray, pass, d);
                        reset_timer();
                        merge_sort_si(iarray, count, pset, MAX_PAR);
                        pi += elapsed_time_since_reset("");
                        reset_timer();
                        merge_sort_d(darray, count, pset, MAX_PAR);
                        pd += elapsed_time_since_reset("");
                        if (!InSort_si(iarray, count - 1))
                            puts("NOT SORTED");
                        if (!InSort_d(darray, count - 1))
                            puts("NOT SORTED");
                    }
                    break;

                case 4:
                    for (k = 0; k < iterations; k++) {
                        make_distrib(darray, iarray, pass, d);
                        reset_timer();
                        RadixMsd_si(iarray, 0, count - 1, 0);
                        mi += elapsed_time_since_reset("");
                        reset_timer();
                        RadixMsd_d(darray, 0, count - 1, 0);
                        md += elapsed_time_since_reset("");
                        if (!InSort_si(iarray, count - 1))
                            puts("NOT SORTED");
                        if (!InSort_d(darray, count - 1))
                            puts("NOT SORTED");
                    }
                }
            }
            which++;
            d++;
        }
        which = 0;
        d = constant;
        printf("Integral sorts %9lu %5.1f %5.1f  %5.1f  %5.1f  %5.1f  %5.1f %5.1f %5.1f\n", pass, bi, si, ii, qi, li, mi, hi, pi);
        printf("Double   sorts %9lu %5.1f %5.1f  %5.1f  %5.1f  %5.1f  %5.1f %5.1f %5.1f\n", pass, bd, sd, id, qd, ld, md, hd, pd);
        bi = si = ii = qi = li = mi = hi = pi = 0;
        bd = sd = id = qd = ld = md = hd = pd = 0;
        if (pass < 64)
            pass++;
        else
            pass = pass * 2;

    }

    free(darray);
    free(iarray);
    printf("Press enter to continue.\n");
    fgets(pause, sizeof pause, stdin);

    return 0;
}
