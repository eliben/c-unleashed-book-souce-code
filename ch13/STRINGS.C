/*
** The proper usage and copyright information for
** this software is covered in DSCRLic.TXT
** This code is Copyright 1999 by Dann Corbit
*/


/*
** Believe it or not, this is a complete routine to sort arrays of strings.
** The define ETYPE_STRING will cause allsort.h to generate functions that
** operate on array of strings.
**
** Why all this bother and not just use a comparison function?
** Because the qsort() style interface is slow.  This is a bit more painful,
** but the payoff in speed is worth it.
*/
#define ETYPE_STRING
#include "allsort.h"
