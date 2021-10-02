/*
** The proper usage and copyright information for
** this software is covered in DSCRLic.TXT
** This code is Copyright 1999 by Dann Corbit
*/

/*
** Etype comparison functions for C++ templates.
*/

#define PRELUDE template < class Etype > inline

/*
** Is object a strictly greater than object b?
*/
PRELUDE
int             GT(Etype & a, Etype & b)
{
    return a > b;
}

/*
** Is object a greater than or equal to object b?
*/
PRELUDE
int             GE(Etype & a, Etype & b)
{
    return a >= b;
}

/*
** Is object a strictly less than object b?
*/
PRELUDE
int             LT(Etype & a, Etype & b)
{
    return a < b;
}

/*
** Is object a less than or equal to object b?
*/
PRELUDE
int             LE(Etype & a, Etype & b)
{
    return a <= b;
}

/*
** Is object a exactly equal to object b?
*/
PRELUDE
int             EQ(Etype & a, Etype & b)
{
    return a == b;
}

/*
** A generic compare.  The others are better.
** This requres an average of 1.5 compare operations per call.
*/
PRELUDE
int             CMP(Etype & a, Etype & b)
{
    return a > b ? 1 : a < b ? -1 : 0;
}

/*
** Exchange two objects in an array.  
*/
PRELUDE
void            SWAP(Etype & a, Etype & b)
{
    Etype           tmp = a;
    a = b;
    b = tmp;
}
