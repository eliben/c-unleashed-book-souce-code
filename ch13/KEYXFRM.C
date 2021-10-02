/*
** The proper usage and copyright information for
** this software is covered in DSCRLic.TXT
** This code is Copyright 1999 by Dann Corbit
*/


/*
** This file is INTEL specific.  It should be pretty easy to generate one
** for any sort of architecture.
**
** The whole thing *can* be done generically by using frexp().
** The most important bit is the sign of the number.
** The next most important bit is the sign of the exponent.
** Then comes the exponent magnitude, followed by the mantissa.
** You will need to add the bias to the exponent, so that it becomes unsigned.
**
** Finally, it would actually be a lot faster to transform the whole
** vector *first* then sort it, then transform it back.  Some people
** might be a bit nervous about that, but there is no reason it should
** not work.
**
** Assume typedef's for uint32 and uint64, of the same size and endian-ness
** as float and double parameters!
** These floating point mapping functions are due to Terje Mathisen.
** They are reprinted with his permission.
*/
#include <assert.h>
#include "inteltyp.h"

#ifdef ASSERT
#undef ASSERT
#endif

#ifdef _DEBUG
#define ASSERT(x) assert((x))
#else
#define ASSERT(x)
#endif


uint32
float2key(float f)
{
    uint32          sign,
                    mant,
                    mask;

    ASSERT(sizeof(float) == sizeof(uint32));
    mant = *(uint32 *) & f;     /* Load float as array of bits */
    sign = mant & SB_MASK32;    /* Isolate the leading sign bit */
    mant ^= SB_MASK32;          /* Invert the sign bit, making + > - */
    mask = sign - (sign >> 31); /* Either 0 or 0x7fffffff */
    mant ^= mask;               /* Invert exp and mant if negative */
    return mant;
}

uint64
double2key(double d)
{
    uint64          sign,
                    mant,
                    mask;

    ASSERT(sizeof(double) == sizeof(uint64));
    mant = *(uint64 *) & d;     /* Load float as array of bits */
    sign = mant & SB_MASK64;    /* Isolate the leading sign bit */
    mant ^= SB_MASK64;          /* Invert the sign bit, making + > - */
    mask = sign - (sign >> 63); /* Either 0 or 0x7fffffffffffffff */
    mant ^= mask;               /* Invert exp and mant if negative */
    return mant;
}
