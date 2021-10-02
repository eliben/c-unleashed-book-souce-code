/*
** The proper usage and copyright information for
** this software is covered in DSCRLic.TXT
** This code is Copyright 1999 by Dann Corbit
*/


/*
 ** signed int comparison functions.
 */

typedef signed int Etype;

#define CHUNK(x,y) digit(((Etype)*(x)-(Etype)INT_MIN),(y))
#define CHUNKS(x, a)\
{ unsigned foo = (Etype)*(x)-(Etype)INT_MIN; \
  int t;\
  for (t = 0; t < sizeof(Etype); t++) \
  a[digit(foo,t)+1][t]++;\
}
#define COST 1
#define GE ge_si
#define GT gt_si
#define LE le_si
#define LT lt_si
#define EQ eq_si
#define CMP cmp_si

#define INSORT                  InSort_si
#define ARRAYISSORTED           ArrayIsSorted_si
#define ARRAYISREVERSED         ArrayIsReversed_si
#define BATCHER                 Batcher_si
#define LINEARINSERTION         LinearInsertion_si
#define INSERTIONSORT           InsertionSort_si
#define INSERTONE               InsertOne_si
#define INSERTTWO               InsertTwo_si
#define INSERTFOUR              InsertFour_si
#define INSERTTHREE             InsertThree_si
#define INSERTZERO              InsertZero_si
#define MEDIAN                  Median_si
#define RADIXLSD                RadixLsd_si
#define REVERSEARRAY            ReverseArray_si
#define SWAP                    Swap_si
#define SHELLSORT               Shellsort_si
#define QSORTB                  qsort_si
#define IQSORT5                 Iqsort5_si
#define HEAPSORT                heapsort_si
#define PERCDOWN                percdown_si
#define PARSCAN                 parscan_si
#define PDELETEMIN              pdeletemin_si
#define PGETMIN                 pgetmin_si
#define PNORMALIZE              pnormalize_si
#define PSHELLSORT              pshellsort_si
#define MERGE_SORT              merge_sort_si
#define RADIXMSD                RadixMsd_si
#define MMERGE                  Mmerge_si
#define MSORT                   Msort_si
#define MERGESORTB              Mergesortb_si

#include "compobj.h"
