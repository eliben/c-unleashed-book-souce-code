/*
** The proper usage and copyright information for
** this software is covered in DSCRLic.TXT
** This code is Copyright 1999 by Dann Corbit
*/


#ifndef  COMPSTR_IS_DEFINED
#define  COMPSTR_IS_DEFINED
/*
** Generic methods needed to compare pointers to strings
*/

/*
** stuff needed by radix sorts
*/
#define KEYSIZE (8)

#define bitsbyte  (CHAR_BIT)
#define bytesword (KEYSIZE)
#define bitsword  (bitsbyte*bytesword)

#define R (1 << bitsbyte)
#define digit(A, B)  ((A >> (bitsword-(B+1)*bitsbyte)) & (R-1))

/*
** NOTE: These comparison methods could be made more efficient.
** This is just a lazy way to do it quickly.
*/

/*
** Is object a strictly greater than object b?
*/
int             GT(Etype a, Etype b)
{
    return (strcmp(a, b) > 0);
}

/*
** Is object a greater than or equal to object b?
*/
int             GE(Etype a, Etype b)
{
    return (strcmp(a, b) >= 0);
}

/*
** Is object a strictly less than object b?
*/
int             LT(Etype a, Etype b)
{
    return (strcmp(a, b) < 0);
}

/*
** Is object a less than or equal to object b?
*/
int             LE(Etype a, Etype b)
{
    return (strcmp(a, b) <= 0);
}

/*
** Is object a exactly equal to object b?
*/
int             EQ(Etype a, Etype b)
{
    return (strcmp(a, b) == 0);
}

/*
** A generic compare.
*/
int             CMP(Etype a, Etype b)
{
    return strcmp(a, b);
}

/*
** Exchange two pointers to char in an array.
*/
void            SWAP(Etype * a, Etype * b)
{
    Etype           tmp = *a;
    *a = *b;
    *b = tmp;
}

#endif          /* COMPPTR_IS_DEFINED */

