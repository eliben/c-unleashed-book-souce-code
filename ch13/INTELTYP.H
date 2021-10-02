/*
** The proper usage and copyright information for
** this software is covered in DSCRLic.TXT
** This code is Copyright 1999 by Dann Corbit
*/


/*
** Typdefs for Intel formats.
** See keyxfrm.c for usage.
*/

typedef unsigned long uint32;
#define SB_MASK32 0x80000000UL

#ifdef _MSC_VER
typedef unsigned __int64 uint64;
typedef __int64 sint64;
#define SB_MASK64 0x8000000000000000ui64
#else
typedef unsigned long long uint64;
typedef long long sint64;
#define SB_MASK64 0x8000000000000000ULL
#endif
extern uint32   float2key(float f);
uint64          double2key(double d);
