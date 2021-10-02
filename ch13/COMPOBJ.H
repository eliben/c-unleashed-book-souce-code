/*
** The proper usage and copyright information for
** this software is covered in DSCRLic.TXT
** This code is Copyright 1999 by Dann Corbit
*/


#ifndef COMPARE_IS_DEFINED
#define COMPARE_IS_DEFINED

/*
** Generic methods needed to compare to direct objects
** (in contrast to comparing two pointers to object)
*/

/*
** stuff needed by radix sorts
*/
#define KEYSIZE (sizeof(Etype))

#define bitsbyte  (CHAR_BIT)
#define bytesword (KEYSIZE)
#define bitsword  (bitsbyte*bytesword)

#define R (1 << bitsbyte)
#define digit(A, B)  ((A >> (bitsword-(B+1)*bitsbyte)) & (R-1))

/*
** Is object a strictly greater than object b?
*/
int             GT(Etype a, Etype b)
{
    return a > b;
}

/*
** Is object a greater than or equal to object b?
*/
int             GE(Etype a, Etype b)
{
    return a >= b;
}

/*
** Is object a strictly less than object b?
*/
int             LT(Etype a, Etype b)
{
    return a < b;
}

/*
** Is object a less than or equal to object b?
*/
int             LE(Etype a, Etype b)
{
    return a <= b;
}

/*
** Is object a exactly equal to object b?
*/
int             EQ(Etype a, Etype b)
{
    return a == b;
}

/*
** A generic compare.  The others are better.
*/
int             CMP(Etype a, Etype b)
{
    return a > b ? 1 : a < b ? -1 : 0;
}

/*
** Exchange two objects in an array.
*/
void            SWAP(Etype * a, Etype * b)
{
    Etype           tmp = *a;
    *a = *b;
    *b = tmp;
}

#endif          /* COMPARE_IS_DEFINED */

