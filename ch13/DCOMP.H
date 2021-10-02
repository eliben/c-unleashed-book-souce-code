/*
** The proper usage and copyright information for
** this software is covered in DSCRLic.TXT
** This code is Copyright 1999 by Dann Corbit
*/


/*
 ** Double comparison functions.
 */

typedef double  Etype;

/*
** stuff needed by radix sorts
*/
#define CHUNK(x, y) digit(double2key((*(x))), (y))
#define CHUNKS(x, a)\
{ uint64 foo = double2key((*(x))); \
  int t; \
  for (t = 0; t < sizeof(foo); t++)\
  a[digit(foo,t)+1][t]++;\
}

#define COST 4
/*
** Fake name mangling.  Does not give us a true template in C.
** [If we try to sort two arrays of the same type in a single file
** by using allsort.h as an include file, we will get multiple
** function defintions.
*/
#define GE ge_d
#define GT gt_d
#define LE le_d
#define LT lt_d
#define EQ eq_d
#define CMP cmp_d
/*
** Token pasting would be neater, but less obvious.
*/
#define INSORT                  InSort_d
#define ARRAYISSORTED           ArrayIsSorted_d
#define ARRAYISREVERSED         ArrayIsReversed_d
#define BATCHER                 Batcher_d
#define LINEARINSERTION         LinearInsertion_d
#define INSERTIONSORT           InsertionSort_d
#define INSERTONE               InsertOne_d
#define INSERTTWO               InsertTwo_d
#define INSERTFOUR              InsertFour_d
#define INSERTTHREE             InsertThree_d
#define INSERTZERO              InsertZero_d
#define MEDIAN                  Median_d
#define RADIXLSD                RadixLsd_d
#define REVERSEARRAY            ReverseArray_d
#define SWAP                    Swap_d
#define SHELLSORT               Shellsort_d
#define IQSORT5                 Iqsort5_d
#define QSORTB                  qsort_d
#define HEAPSORT                heapsort_d
#define PERCDOWN                percdown_d
#define PARSCAN                 parscan_d
#define PDELETEMIN              pdeletemin_d
#define PGETMIN                 pgetmin_d
#define PNORMALIZE              pnormalize_d
#define PSHELLSORT              pshellsort_d
#define MERGE_SORT              merge_sort_d
#define RADIXMSD                RadixMsd_d
#define MMERGE                  Mmerge_d
#define MSORT                   Msort_d
#define MERGESORTB              Mergesortb_d

#include "compobj.h"
