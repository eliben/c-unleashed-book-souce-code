/*
** The proper usage and copyright information for
** this software is covered in DSCRLic.TXT
** This code is Copyright 1999 by Dann Corbit
*/


/*
** This is a sort of "Poor-man's template for C" used to generate
** algorithms for sorting.
*/
#include <math.h>
#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "mtrand.h"

/* This works best for ME.  YMMV. */

/* SmallCutoff is where the O(n*log(n)) sorts switch
** to an O(n^2) subfile sort.
*/
static const unsigned long SmallCutoff = 20;

/* LargeCutoff is where the O(n) sorts switch
** to O(n*log(n)) subfile sort
*/
static const unsigned long LargeCutoff = 200000;

/*
** If you are using C++, PRELUDE will define
** an inline function template.  The normal
** C version does nothing.  You may wish to make
** PRELUDE be static, so that you can use
** allsort.h as an include file in a normal C
** program.  The limitation is that when used this
** way, you can only sort one vector of a given
** type at a time.
*/
#define PRELUDE

/*
** Inteltyp.h contains definitions
** for radix sorting of Intel data types.
*/
#include "inteltyp.h"

/*
** By defininition of one of the following
** symbols, a routine (somewhat like a C++
** template) to define a sort for the named
** data type will be created.
*/
#if defined(ETYPE_UNSIGNED_INT)
#include "uicomp.h"
#elif defined(ETYPE_SIGNED_INT)
#include "sicomp.h"
#elif defined(ETYPE_UNSIGNED_LONG)
#include "ulcomp.h"
#elif defined(ETYPE_SIGNED_LONG)
#include "slcomp.h"
#elif defined(ETYPE_UNSIGNED_CHAR)
#include "uccomp.h"
#elif defined(ETYPE_SIGNED_CHAR)
#include "sccomp.h"
#elif defined(ETYPE_UNSIGNED_LONG_LONG)
#include "ullcomp.h"
#elif defined(ETYPE_SIGNED_LONG_LONG)
#include "sllcomp.h"
#elif defined(ETYPE_FLOAT)
#include "fcomp.h"
#elif defined(ETYPE_DOUBLE)
#include "dcomp.h"
#elif defined(ETYPE_LONG_DOUBLE)
#include "ldcomp.h"
#elif defined(ETYPE_P2_UNSIGNED_INT)
#include "puicomp.h"
#elif defined(ETYPE_P2_SIGNED_INT)
#include "psicomp.h"
#elif defined(ETYPE_P2_UNSIGNED_LONG)
#include "pulcomp.h"
#elif defined(ETYPE_P2_SIGNED_LONG)
#include "pslcomp.h"
#elif defined(ETYPE_P2_UNSIGNED_CHAR)
#include "puccomp.h"
#elif defined(ETYPE_P2_SIGNED_CHAR)
#include "psccomp.h"
#elif defined(ETYPE_P2_UNSIGNED_LONG_LONG)
#include "pullcomp.h"
#elif defined(ETYPE_P2_SIGNED_LONG_LONG)
#include "psllcomp.h"
#elif defined(ETYPE_P2_FLOAT)
#include "pfcomp.h"
#elif defined(ETYPE_P2_DOUBLE)
#include "pdcomp.h"
#elif defined(ETYPE_P2_LONG_DOUBLE)
#include "pldcomp.h"
#elif defined(ETYPE_STRING)
#include "strcomp.h"
#elif defined(ETYPE_COLLATED_STRING)
#include "cstr_comp.h"

#else
#error "PLEASE DEFINE SOME TYPE!"
#endif          /* defined(ETYPE_... */

/*
** Prototypes for sorting
*/
#include "genproto.h"



/*
For partitions of zero length, there is nothing to do.
This is the optimal sort for zero items.
*/
PRELUDE
void            INSERTZERO(Etype array[])
{
    return;
}

/*
For partitions of length one, there is nothing to do.
This is the optimal sort for one item.
*/
PRELUDE
void            INSERTONE(Etype array[])
{
    return;
}

/*
For partitions of length two, we either reverse it or leave it alone.
This is the optimal sort for two items.
*/
PRELUDE
void            INSERTTWO(Etype array[])
{
    if (GT(array[0], array[1]))
        SWAP(&array[0], &array[1]);
    return;
}

/*
Optimal treatment for small partitions of length 3.
This is the optimal sort for three items.
*/
PRELUDE
void            INSERTTHREE(Etype array[])
{
    if (LT(array[0], array[1])) {
        if (LT(array[1], array[2]))
            return;
        else if (LT(array[0], array[2]))
            SWAP(&array[1], &array[2]);
        else {
            Etype           Tmp = array[0];
            array[0] = array[2];
            array[2] = array[1];
            array[1] = Tmp;
        }
    } else {
        if (LT(array[0], array[2]))
            SWAP(&array[0], &array[1]);
        else if (LT(array[1], array[2])) {
            Etype           Tmp = array[0];
            array[0] = array[1];
            array[1] = array[2];
            array[2] = Tmp;
        } else
            SWAP(&array[0], &array[2]);
    }
    return;
}

/*
Optimal treatment for small partitions of length 4.
This is the optimal sort for four items.
*/
PRELUDE
void            INSERTFOUR(Etype array[])
{
    Etype           temp;
    if (GT(array[0], array[1])) {
        temp = array[0];
        array[0] = array[1];
        array[1] = temp;
    }
    if (GT(array[2], array[3])) {
        temp = array[2];
        array[2] = array[3];
        array[3] = temp;
    }
    if (GT(array[1], array[2])) {
        if (GT(array[0], array[3])) {
            temp = array[0];
            array[0] = array[2];
            array[2] = temp;
            temp = array[1];
            array[1] = array[3];
            array[3] = temp;
        } else {
            temp = array[1];
            array[1] = array[2];
            array[2] = temp;
            if (GT(array[0], array[1])) {
                temp = array[0];
                array[0] = array[1];
                array[1] = temp;
            }
            if (GT(array[2], array[3])) {
                temp = array[2];
                array[2] = array[3];
                array[3] = temp;
            }
        }
    }
}

/*
In practice, this "optimal" treatment for partitions of length 5
seem to slow the sort down rather than speed it up.
*/
#ifdef MY_CACHE_IS_ENORMOUS
#include "sort5.h"
#endif


/*
** Algorithm M, from:
**  "The Art of Computer Programming, Volume 3: Sorting and Searching",
**  Donald Ervin Knuth, Addison-Wesley, Hardcover, 2nd edition,
**  Published May 1998, ISBN 0201896850
**  Page 111.
**
** This is a straightforward implementation directly from his description.
** While the primary use of Merge Exchange sorting is parallel supercomputer
** sorting of large batches of data, this version is to be tested for use
** on very tiny partitions (specifically 256 items or less)
*/
static const unsigned shiftedbit[] =
{                               /* First two items are just placeholders */
    0, 0, 1, 2, 2, 4, 4, 4, 4, 8, 8, 8, 8, 8, 8, 8, 8,
    16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128,
    128, 128, 128, 128, 128, 128
};

PRELUDE
void            BATCHER(Etype A[], int N)
{
    unsigned        p,
                    q,
                    r,
                    d;
    unsigned        i;

    switch (N) {
    case 0:
    case 1:
        return;
    case 2:
        INSERTTWO(A);
        return;
    case 3:
        INSERTTHREE(A);
        return;
    case 4:
        INSERTFOUR(A);
        return;
#ifdef MY_CACHE_IS_ENORMOUS
    case 5:
        InsertFive(array);
        return;
#endif

    default:

        assert(N <= 256);

/* M1: */
        p = shiftedbit[N];
/* M2: */
        do {
            q = shiftedbit[N];
            r = 0;
            d = p;
    M3:
            for (i = 0; i < N - d; i++) {
                if ((i & p) == r) {
                    Etype           tmp;
                    /* M4: */
                    if (GT(A[i], A[i + d])) {
                        tmp = A[i];
                        A[i] = A[i + d];
                        A[i + d] = tmp;
                    }
                }
            }
/* M5: */
            if (q != p) {
                d = q - p;
                q >>= 1;
                r = p;
                goto M3;        /* Are you going to argue with Knuth? */
            }
/* M6: */
            p >>= 1;
        }
        while (p > 0);
    }
}

/*
Straightforward linear insertion.  Binary insertion is better, and
Shell-sort is better yet.  Don't use this for real sorting work.
*/
PRELUDE
void            LINEARINSERTION(Etype a[], unsigned long n)
{
    unsigned long   i,
                    j;
    Etype           tmp;

    for (i = 1; i < n; i++)     /* look for insertion point. */
        for (j = i; j > 0 && GT(a[j - 1], a[j]); j--) {
            /* Move the others down and insert it. */
            tmp = a[j];
            a[j] = a[j - 1];
            a[j - 1] = tmp;
        }
}

/*
This is a straightforward binary insertion sort.
Binary insertion is much faster on average compared to linear insertion.
Shell-sort is even better, according to my calculations.  You might
want to benchmark both binary insertion and shell-sort, but I suspect
shell-sort will outperform binary insertion on most systems.
*/
PRELUDE
void            INSERTIONSORT(Etype array[], unsigned long count)
{
    unsigned long   partition;  /* The end of a new partition */
    long            beg;        /* Search beginning here (this moves toward
                                 * ipg) */
    long            ipg;        /* Current guess for the insertion point
                                 * (average of beg+end) */
    long            end;        /* Search ending here (this moves toward ipg) */
    Etype           temp;       /* Hold one elemennt of the array in
                                 * temporary storage */
    /* * One element is sorted by definition. * Form larger and larger
     * ordered partitions * until the entire array is correctly ordered */
    switch (count) {
    case 0:
    case 1:
        return;
    case 2:
        INSERTTWO(array);
        return;
    case 3:
        INSERTTHREE(array);
        return;
    case 4:
        INSERTFOUR(array);
        return;
#ifdef MY_CACHE_IS_ENORMOUS
    case 5:
        InsertFive(array);
        return;
#endif

    default:

/* Seems like a good idea, but it does not really help.
I left this in here so you won't bother with it.  Or if
you do want to try it, you can just remove the comments.

   if (ARRAYISREVERSED(array, 0, count-1)) {
     REVERSEARRAY(array, 0, count-1);
     return;
   }
 */
        for (partition = 1; partition < count; partition++) {
            /* inline binary search to find point of insertion */
            beg = ipg = 0;      /* The first element of the ordered part of
                                 * the array is element 0 */
            end = partition - 1;/* The last element already ordered is
                                 * element partition-1 */

            /* Without this check, loop terminates only if an equal element
             * is already sorted */
            while (end >= beg) {
                /* insertion point guess of halfway between beginning and
                 * ending */
                ipg = ((end + beg) >> 1);       /* BUGBUG: we can't sort sets
                                                 * > (MAX_LONG)/2 elements */
                /* However, this sort should *NEVER* be used for anything but
                 * tiny partitions */

                /* The element sitting at the end of the partition is the one
                 * we will insert */
                /* It is not ordered yet, but all the array to the left of it
                 * is in sort. */
                if (GT(array[ipg], array[partition]))
                    end = ipg - 1;
                else
                    beg = ++ipg;
            }
            /* make room at array[ipg] for array[i] */
            /* It might already be in the right place */
            if (partition != (unsigned long) ipg) {
                temp = array[partition];        /* Save the new element we
                                                 * are ready to insert */
                /* Move the data from the insertion point downward.  We can't
                ** use memcpy()!
                ** For C++, you will have to replace this because memmove
                ** does not fire constructors!  Shell-sort is better anyway.
                */
                memmove(&array[ipg + 1], &array[ipg], (partition - ipg) * sizeof(Etype));
                array[ipg] = temp;      /* Put the new element in its sorted
                                         * order */
            }
        }

    }
    return;
}

/*
** The h-constants for this version of Shell-sort come from:
** Handbook of Algorithms and Data Structures in Pascal and C
** By Gaston Gonnet, Ricardo Baeza-Yates
** Addison Wesley; ISBN: 0201416077
** These h-increments work better for me than Sedgewick's under
** full optimization.  Your mileage may vary, so I suggest you
** try Sedgewick's suggestions as well.  The h-increment arena
** is a rich environment for exploration.  I suggest attempting
** all possible permutations below 20, since that is where a
** good shell-sort is crucuial.  If you find something wonderul
** you may get your name up in lights.
*/
PRELUDE
void            SHELLSORT(Etype array[], size_t count)
{
    size_t          i,
                    inc,
                    j;
    Etype           tmp;

    switch (count) {
    case 0:
    case 1:
        return;
    case 2:
        INSERTTWO(array);
        return;
    case 3:
        INSERTTHREE(array);
        return;
    case 4:
        INSERTFOUR(array);
        return;
#ifdef MY_CACHE_IS_ENORMOUS
    case 5:
        InsertFive(array);
        return;
#endif

    default:

        for (inc = count; inc > 0;) {
            for (i = inc; i < count; i++) {
                j = i;
                tmp = array[i];
                while (j >= inc && (LT(tmp, array[j - inc]))) {
                    array[j] = array[j - inc];
                    j -= inc;
                }
                array[j] = tmp;
            }                   /* Calculate the next h-increment */
            inc = (size_t) ((inc > 1) && (inc < 5)) ? 1 : 5 * inc / 11;
        }
    }
}

/* Test to see if segment [Lo, ... Hi] is sorted already */
PRELUDE
long            ARRAYISSORTED(Etype A[], unsigned long Lo, unsigned long Hi)
{
    unsigned long   i;
    unsigned int    ascending;
    if (Lo == Hi)
        return 1;
    if ((ascending = (LE(A[Lo], A[Lo + 1]))))
        for (i = Lo + 1; i < Hi; i++) {
            /* Is the next item bigger than this one? */
            if (GT(A[i], A[i + 1])) {
                /* Were all of them the same size up to here? */
                if (EQ(A[i], A[Lo]))
                    /* Maybe the partition is reversed? */
                    if (ARRAYISREVERSED(A, i, Hi)) {
                        /* It was reversed.  Invert it and we are done! */
                        REVERSEARRAY(A, Lo, Hi);
                        return 1;
                    } else {    /* Nope.  The partition is not reversed */
                        return 0;
                    }
                else
                    return 0;
            }
        }
    /* not ascending */
    else if (ARRAYISREVERSED(A, Lo, Hi)) {
        /* It was reversed.  Invert it and we are done! */
        REVERSEARRAY(A, Lo, Hi);
        return 1;
    } else
        return 0;
    return 1;
}

/* Test to see if segment [Lo, ... Hi] is reverse sorted already */
PRELUDE
long            ARRAYISREVERSED(Etype A[], unsigned long Lo, unsigned long Hi)
{
    unsigned long   i;
    for (i = Lo; i < Hi; i++) {
        if (LE(A[i], A[i + 1]))
            return 0;
    }
    return 1;
}
/*
Reverse the order of a partition.
*/
PRELUDE
void            REVERSEARRAY(Etype * A, unsigned long Lo, unsigned long Hi)
{
    Etype          *r = A + Lo;
    for (r = A + (Hi - Lo); A < r; A++, r--) {
        Etype           Tmp = *A;
        *A = *r;
        *r = Tmp;
    }
}

/*
Primitive quicksort.  This is a bad algorithm for general purposes.
Don't use it.  Use IQSORT5 instead.  This thing is a bomb waiting to go off.
*/
PRELUDE
void            QSORTB(Etype * A, int l, int r)
{
    int             loc;

    if (l < r) {
        int             i = l,
                        j = r;
        Etype           tmp,
                        pivot = A[l];
        /* Divide piles into partitions */
        for (;;) {
            while ((GE(A[j], pivot)) && (j > l))
                j--;
            while ((LT(A[i], pivot)) && (i < r))
                i++;
            if (i < j) {
                tmp = A[i];
                A[i] = A[j];
                A[j] = tmp;
            } else {
                loc = j;
                break;
            }
        }
        /* Recurse */
        QSORTB(A, l, loc);
        QSORTB(A, loc + 1, r);
    }
}


/*
Find the median of 3, with a bit of stirring.
*/
PRELUDE
void            MEDIAN(Etype A[], unsigned long n)
{
    unsigned long   m,
                    x,
                    i,
                    j,
                    k;
    Etype           t;
    i = (mtrand() + 1) % n;     /* We randomly stir the pot up a bit... */
    j = (mtrand() + 1) % n;     /* We randomly stir the pot up a bit... */
    k = n / 2;                  /* The middle element of a partition is a
                                 * good guess */
    /* Pick the center of these three */
    if (LE(A[i], A[j])) {
        if (LE(A[j], A[k])) {
            m = j;
            x = k;
        } else if (LE(A[i], A[k])) {
            m = k;
            x = j;
        } else {
            m = i;
            x = j;
        }
    } else {
        if (LE(A[i], A[k])) {
            m = i;
            x = k;
        } else if (LE(A[j], A[k])) {
            m = k;
            x = i;
        } else {
            m = j;
            x = i;
        }
    }
    /* Store out of the way... */
    t = A[0];
    A[0] = A[m];
    A[m] = t;
    t = A[n - 1];
    A[n - 1] = A[x];
    A[x] = t;
}

/*
Singleton's sorting algorithm.  ACM Algorithm 347 is due
to Richard Singleton.  The general outline I use was shown
to me by Dan Stubbs, and seems to work better than others
I have tried.  The test for an in-order partition was a
particularly good idea that he suggested.  The reverse check
and partition reversal was my idea.  Also, I use shell-sort
instead of insertion sort.  It is always better for a broad
variety of distributions, according to my tests. YMMV.
*/
PRELUDE
void            IQSORT5(Etype A[], unsigned long n)
{
    unsigned long   i,
                    j;
    Etype           t;
    /* Change algorithm for small partitions */
    if (n < SmallCutoff) {
        SHELLSORT(A, n);
        return;
    }
    /* Check for preordered array: * The idea to check for a presorted array
     * came * to me from Dan Stubbs [dstubbs@garden.csc.calpoly.edu].  If the
     * partition is already ordered, then we are done.  I added a second
     * check for a reversed partition.  If the * partition is reversed, I
     * reverse it and we are done. */
    if (ARRAYISSORTED(A, 0, n - 1))
        return;

    /* Stir up the data a bit, and pick a median estimate. */
    MEDIAN(A, n);

    i = 0;
    j = n;
    /* Form partitions */
    for (;;) {
        do
            i++;
        while (LT(A[i], A[0]) && i < n);
        do
            j--;
        while (GT(A[j], A[0]) && j > 0);
        if (j < i)
            break;
        t = A[i];
        A[i] = A[j];
        A[j] = t;
    }
    /* Restore pivot */
    t = A[0];
    A[0] = A[j];
    A[j] = t;

    /* Recurse smaller partition first */
    if (j < n - j) {
        IQSORT5(A, j);
        IQSORT5(A + j + 1, n - j - 1);
    } else {
        IQSORT5(A + j + 1, n - j - 1);
        IQSORT5(A, j);
    }
}

/*
** Test to see if segment [Lo, ... Hi] is sorted already
** This one does not try to fix anything because it is for
** testing purposes only.
*/
PRELUDE
long            INSORT(Etype A[], unsigned long Hi)
{
    unsigned long   i;
    for (i = 0; i < Hi - 1; i++) {      /* Is the next item bigger than this
                                         * one? */
        if (GT(A[i], A[i + 1])) {
            return 0;
        }
    }
    return 1;
}

/*
This version of heapsort is adapted from:

Data Structures and Algorithm Analysis in C (Second Edition)
by Mark Allen Weiss
Addison-Wesley, 1997
ISBN: 0-201-49840-5
Page 228.

It is simple and interesting, but quick-sort is better than heap-sort.
*/

#define LeftChild( i )  ( 2 * ( i ) + 1 )

void            PERCDOWN(Etype A[], int i, int N)
{
    int             Child;
    Etype           Tmp;

    for (Tmp = A[i]; LeftChild(i) < N; i = Child) {
        Child = LeftChild(i);
        if (Child != N - 1 && GT(A[Child + 1], A[Child]))
            Child++;
        if (LT(Tmp, A[Child]))
            A[i] = A[Child];
        else
            break;
    }
    A[i] = Tmp;
}

void            HEAPSORT(Etype A[], int N)
{
    int             i;

    for (i = N / 2; i >= 0; i--)
        /* BuildHeap */
        PERCDOWN(A, i, N);
    for (i = N - 1; i > 0; i--) {
        SWAP(&A[0], &A[i]);
        /* DeleteMax */
        PERCDOWN(A, 0, i);
    }
}


/*
** The purpose of this routine is to discover partitions in the data.
** This is a two state FSM.
** Ascend = 1, Descend = 0.
**
** This is a vastly improved version of my ugly code.
** The large improvement was from Kang Su Gatlin.
**
*/
PRELUDE
int             PARSCAN(Etype * array, unsigned long n, partition ps[], long max_par)
{
    unsigned long   i;
    char            direction;
    long            pcount = 0;

    ps[pcount].start = 0;
    ps[pcount].ascending = GE(array[1], array[0]);
    for (i = 1; i < n; i++) {
        direction = GE(array[i], array[i - 1]);
        if (ps[pcount].ascending != direction) {
            ps[pcount].end = i - 1;
            pcount++;
            if (pcount > max_par)
                return -max_par;
            ps[pcount].start = i;
            if (i == n - 1)
                ps[pcount].ascending = 1;
            else
                ps[pcount].ascending = GE(array[i + 1], array[i]);
        }
    }
    ps[pcount].end = n - 1;
    pcount++;
    return pcount;
}

/*
** Remove the smallest item from a partition.
*/
PRELUDE
Etype PDELETEMIN(partition * p, char *end, Etype data[])
{
    Etype           e;
    if (p->start < p->end) {
        *end = 0;
    } else {
        *end = 1;
        if (p->start > p->end)
            puts("Error! Deletion from empty partition");
    }
    if (p->ascending) {
        e = data[p->start++];
    } else {
        e = data[p->end--];
    }

    return e;
}

/*
** Retrieve the smallest item from a partition.
*/
PRELUDE
Etype PGETMIN(partition p, Etype data[])
{
    Etype           e;
    if (p.ascending) {
        e = data[p.start];
    } else {
        e = data[p.end];
    }
    return e;
}

/*
** This shell-sort operates on partitions of data, rather
** than on simple data elements.
*/
PRELUDE
void            PSHELLSORT(partition array[], size_t count, Etype * data)
{
    size_t          i,
                    inc,
                    j;

    partition       tmp;
    Etype           etmp;

    for (inc = count; inc > 0;) {
        for (i = inc; i < count; i++) {
            j = i;
            tmp = array[i];
            etmp = PGETMIN(tmp, data);
            while (j >= inc && (LT(etmp, PGETMIN(array[j - inc], data)))) {
                array[j] = array[j - inc];
                j -= inc;
            }
            array[j] = tmp;
        }                       /* Calculate the next h-increment */
        inc = (size_t) ((inc > 1) && (inc < 5)) ? 1 : 5 * inc / 11;
    }
}

/* Normalize a partition by removing the first item
** or changing its precedence and moving to the right
** location.
*/
PRELUDE
void            PNORMALIZE(partition * array, size_t count, Etype data[])
{
    long            beg;        /* Search beginning here (this moves toward
                                 * ipg) */
    long            ipg;        /* Current guess for the insertion point */
    long            end;        /* Search ending here (this moves toward ipg) */
    partition       temp;       /* Hold one partition in temporary storage */
    long            i;

    Etype           McGuffin = PGETMIN(array[0], data);
    /* Maybe we don't need to do anything (I'm an optimist) */
    if (count < 2 || LE(McGuffin, PGETMIN(array[1], data)))
        return;

    /* inline binary search to find point of insertion */
    beg = ipg = 1;              /* The first element of the ordered part of
                                 * the data is element 0 */
    end = count - 1;            /* The last element already ordered is
                                 * element partition */
    /* Without this check, loop terminates only if an equal element is
     * already sorted */
    while (end >= beg) {
        /* insertion point guess of halfway between beginning and ending */
        ipg = ((end + beg) >> 1);
        if (GE(PGETMIN(array[ipg], data), McGuffin))
            end = ipg - 1;
        else
            beg = ++ipg;
    }
    /* make room at data[ipg] for data[0] */
    temp = array[0];            /* Save the new element we are ready to
                                 * insert */
    for (i = 0; i < ipg; i++)
        array[i] = array[i + 1];
    array[ipg - 1] = temp;      /* Put the new element in its sorted order */
    return;
}

/*
Most significant digit radix sort.
The outline of these sorts comes from "Algorithms in C"
by Robert Sedgewick, ISBN:0-201-31452-5.

I have added a generic CHUNK operator.  This operator
is analogous to the compare operator for a standard sort.
Besides just peeling out the digit, CHUNK performs whatever
transforation is needed in order to put the bits in the
proper order of significance.
*/
#ifndef RADIX_MISSING

#define bin(A) l+count[A]

PRELUDE
void            RADIXMSD(Etype a[], long l, long r, unsigned w)
{
    if (w > KEYSIZE || r <= l)
        return;
    if (r - l <= LargeCutoff * COST) {
        IQSORT5(a + l, r - l + 1);
        return;
    } else {
        long            i,
                        j,
                        count[R + 1] = {0};
        Etype          *b = malloc((r - l + 1) * sizeof(Etype));

        /* Use standard comparison sort if allocation fails */
        if (b == NULL) {
            IQSORT5(a + l, r - l + 1);
            return;
        }
        /* increment the starting place for the next bin */
        for (i = l; i <= r; i++) {
            count[CHUNK(a + i, w) + 1]++;
        }

        /* Add the previous bin counts to find true offset */
        for (j = 1; j < R; j++)
            count[j] += count[j - 1];

        /* Distribute according to bin positions */
        for (i = l; i <= r; i++) {
            b[count[CHUNK(a + i, w)]++] = a[i];
        }
        /* Transfer back to the original array */
        for (i = l; i <= r; i++)
            a[i] = b[i - l];
        free(b);

        /* Process the next chunk of the key for the first bin */
        RADIXMSD(a, l, bin(0) - 1, w + 1);
        for (j = 0; j < R - 1; j++) {
            /* Process the next chunk of the key for the rest of the  bins */
            RADIXMSD(a, bin(j), bin(j + 1) - 1, w + 1);
        }

    }
}
/*
Least significant digit radix sort.
The outline of these sorts comes from "Algorithms in C"
by Robert Sedgewick, ISBN:0-201-31452-5.

I have added a generic CHUNK operator.  This operator
is analogous to the compare operator for a standard sort.
Besides just peeling out the digit, CHUNK performs whatever
transforation is needed in order to put the bits in the
proper order of significance.
There is also a CHUNKS operator that finds all single chunks
in the data object at once.
*/
PRELUDE
void            RADIXLSD(Etype a[], long l, long r, size_t keysize)
{
    int             i,
                    j,
                    w;
    Etype          *b;

    if (r - l <= LargeCutoff * COST) {
        IQSORT5(a + l, r - l + 1);
        return;
    }
    /* For long keys, LSD Radix sort is too expensive */
    if (KEYSIZE > 8) {
        RADIXMSD(a, l, r, 0);
        return;
    } else {
        unsigned long   cnts[R + 1][8] = {0};
        b = malloc((r - l + 1) * sizeof(Etype));

        /* Use standard comparison sort if allocation fails */
        if (b == NULL) {
            IQSORT5(a + l, r - l + 1);
            return;
        }
        /* Count the bins all at once */
        for (i = l; i <= r; i++)
            CHUNKS(a + i, cnts);
        for (w = KEYSIZE - 1; w >= 0; w--)
            /* Add the previous bin counts to find true offset */
            for (j = 1; j < R; j++)
                cnts[j][w] += cnts[j - 1][w];


        for (w = KEYSIZE - 1; w >= 0; w--) {
            long            count[R + 1] =
            {0};

            /* Distribute according to bin positions */
            for (i = l; i <= r; i++) {
                b[cnts[CHUNK(&a[i], w)][w]++] = a[i];
            }
            /* Transfer back to the original array */
            for (i = l; i <= r; i++)
                a[i] = b[i];
        }
        free(b);
    }
}
#endif


/*
** This merge sort is *almost* actually useful.
** The file I/O based version does have a real purpose.
** This one is just for fun.  Don't bother with it.
** quick-sort beats it hands-down and does not need extra memory.
*/
PRELUDE
void            MERGE_SORT(Etype a[], unsigned long count, partition * pset, unsigned long max_par)
{
    long            pcount;
    Etype          *alternate = NULL;
    unsigned long   current = 0;
    unsigned long   increment = 0;

    /*
    ** -- PURE DISCOVERY of partitions --
    */
    pcount = PARSCAN(a, count, pset, max_par);
    /* If too many partitions by discovery, we use a cookie cutter to make
     * them. */
    /*
    ** -- COOKIE CUTTER if we ran out of partitions --
    */
    if (pcount < 0) {
        unsigned long   j;
        increment = count / (max_par) + 1;
        if (increment < SmallCutoff)
            increment = SmallCutoff;
        for (j = 0; j < count; j += increment)
            IQSORT5(&a[j], increment);
        if (count % increment)
            IQSORT5(&a[count - (count % increment)], count % increment);
        /*
        ** This is actually pretty lazy of me.  But this routine is
        ** just a toy anyway.  The real partition merge-sort is elsewhere.
        */
        pcount = PARSCAN(a, count, pset, max_par);

    }
    /*
    ** If we have a single partition we can save some time
    */
    if (pcount == 1) {
        if (pset[0].ascending)
            return;
        else {
            REVERSEARRAY(a, pset[0].start, pset[0].end);
            return;
        }
    }
    /*
    ** One great weakness of many merge sort routines -- memory hogs.
    */
    if ((alternate = malloc(count * sizeof(Etype)))) {
        /* After this step, all partitions will be ordered from smallest to
         * largest */
        PSHELLSORT(pset, pcount, a);
        {
            char            end = 0;
            Etype           e;
            while (current < count) {
                e = PDELETEMIN(pset, &end, a);
                alternate[current++] = e;

                if (end) {
                    pset++;
                    pcount--;
                } else {
                    PNORMALIZE(pset, pcount, a);
                }
            }
        }
        memcpy(a, alternate, count * sizeof(Etype));
        free(alternate);
    } else /* So much for stable.  But what if malloc() fails? */
        IQSORT5(a, count);
}
/*
** The following routines are for a horrible but easily understandable
** form of merge-sort.   Here, we actually join the ordered data
*/
PRELUDE
void            MMERGE(Etype A[], Etype B[], size_t l, size_t m, size_t r)
{
    size_t          i = l;
    size_t          j = m + 1;
    size_t          k = l;
    /* Put the smallest thing into array B */
    while ((i <= m) && (j <= r)) {
        if (LT(A[i], A[j]))
            B[k++] = A[i++];
        else
            B[k++] = A[j++];
    }
    /* Copy leftover (if any) */
    while (i <= m) {
        B[k++] = A[i++];
    }
    while (j <= r) {
        B[k++] = A[j++];
    }
    /* Transfer back to original array */
    for (k = l; k <= r; k++) {
        A[k] = B[k];
    }
}

/* Helper function for icky(tm) merge sort */
PRELUDE
void            MSORT(Etype A[], Etype B[], size_t l, size_t r)
{
    size_t          m;          /* The middle */
    if (l < r) {                /* Cut problem to half size until 1 unit */
        /* Divide partition by 2, sort and merge */
        m = ((l + r) >> 1);
        MSORT(A, B, l, m);
        MSORT(A, B, m + 1, r);
        MMERGE(A, B, l, m, r);  /* A single element is ordered */
    }
}
/*
This is a really bad merge sort.  Please don't use it.
It is for educational purposes only.
*/
PRELUDE
void            MERGESORTB(Etype A[], size_t count)
{
    Etype          *B;
    if ((B = malloc(count * sizeof(Etype)))) {
        MSORT(A, B, 0, count - 1);
        free(B);
    } else                      /* Hopefully, we will fail and use a good
                                 * sort. ;-) */
        IQSORT5(A, count);
}
