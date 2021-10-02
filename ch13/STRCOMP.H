/*
** The proper usage and copyright information for
** this software is covered in DSCRLic.TXT
** This code is Copyright 1999 by Dann Corbit
*/


/*
** string comparison functions.
*/

/*
** stuff needed by radix sorts
*/
typedef unsigned char *Etype;

#define CHUNK(x,y) *(x[(y)])
#define CHUNKS(x, a)\
{ Etype foo = *x;\
  int t;\
  for (t = 0; t < 8 && *x[t]; t++) \
  a[CHUNK((x),(y))+1][t]++;\
}
#define COST 1

/*
** Fake name mangling.  Does not give us a true template in C.
** [If we try to sort two arrays of the same type in a single file
** by using allsort.h as an include file, we will get multiple
** function defintions.
*/
#define GE ge_str
#define GT gt_str
#define LE le_str
#define LT lt_str
#define EQ eq_str
#define CMP cmp_str

#define INSORT                  InSort_str
#define ARRAYISSORTED           ArrayIsSorted_str
#define ARRAYISREVERSED         ArrayIsReversed_str
#define BATCHER                 Batcher_str
#define LINEARINSERTION         LinearInsertion_str
#define INSERTIONSORT           InsertionSort_str
#define INSERTONE               InsertOne_str
#define INSERTTWO               InsertTwo_str
#define INSERTFOUR              InsertFour_str
#define INSERTTHREE             InsertThree_str
#define INSERTZERO              InsertZero_str
#define MEDIAN                  Median_str
#define RADIXLSD                RadixLsd_str
#define REVERSEARRAY            ReverseArray_str
#define SWAP                    Swap_str
#define SHELLSORT               Shellsort_str
#define QSORTB                  qsort_str
#define IQSORT5                 Iqsort5_str
#define HEAPSORT                heapsort_str
#define PERCDOWN                percdown_str
#define PARSCAN                 parscan_str
#define PDELETEMIN              pdeletemin_str
#define PGETMIN                 pgetmin_str
#define PNORMALIZE              pnormalize_str
#define PSHELLSORT              pshellsort_str
#define MERGE_SORT              merge_sort_str
#define RADIXMSD                RadixMsd_str
#define RADIX_MISSING           1
#define MMERGE                  Mmerge_str
#define MSORT                   Msort_str
#define MERGESORTB              Mergesortb_str

#include "compstr.h"
