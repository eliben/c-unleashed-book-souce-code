/**************************************/
/*                                    */
/*   Code from the book C Unleashed   */
/*   Macmillan, 2000                  */
/*   Chapter 6: Data Files            */
/*   Steve Summit 2000-03-17          */
/*                                    */
/**************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void
toieee(float f, unsigned char buf[4])
{
double mantf;
long mantl;
unsigned long mantl2;
int e;
int s = 0;
int shift = 24;
mantf = frexp(f, &e);
if(mantf != 0)
	e += 127 - 1;
if(e <= 0)				/* denormalized */
	{
	shift += e-1;
	e = 0;
	}
#if defined(__STDC__) && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
if(signbit(f))	/* handles -0 better */
#else
if(mantf < 0)
#endif
	{
	s = 1;
	mantf = -mantf;
	}
mantl = (long int)ldexp(mantf, shift+1);
if(mantl & 1)
	mantl += 2;			/* XXX worry about overflow */
mantl >>= 1;
mantl2 = mantl & ~(1L << 23);		/* zap implicit leading 1 */
mantl2 |= ((unsigned long)s << 31) | ((unsigned long)(e & 0xff) << 23);
buf[0] = (unsigned char)(mantl2 & 0xff);
buf[1] = (unsigned char)((mantl2 >> 8) & 0xff);
buf[2] = (unsigned char)((mantl2 >> 16) & 0xff);
buf[3] = (unsigned char)((mantl2 >> 24) & 0xff);
}

float
fromieee(unsigned char buf[4])
{
unsigned long mant;
int e;
int s;
float f;

mant = buf[0];
mant |= (unsigned long)buf[1] << 8;
mant |= (unsigned long)(buf[2] & 0x7f) << 16;

e = ((buf[3] & 0x7f) << 1) | ((buf[2] >> 7) & 0x01);

s = buf[3] & 0x80;

if(e == 0)
	f = (float)ldexp(mant, -126-23);	/* denormalized */
else	{
	mant |= (1L << 23);		/* restore implicit leading 1 */
	f = (float)ldexp(mant, e-127-23);
	}
if(s)	return -f;
else	return f;
}

void
toieeed(double d, unsigned char buf[8])
{
double mantf;
unsigned long mantl;
unsigned long mantl2;
double mantf2;
long mantlhi;
unsigned long mantl2hi;
int e;
int s = 0;
int shift = 53;
int add = 0;
mantf = frexp(d, &e);
if(mantf != 0)
	e += 1022;
if(e <= 0)				/* denormalized */
	{
	shift += e-1;
	e = 0;
	}
#if defined(__STDC__) && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
if(signbit(d))	/* handles -0 better */
#else
if(mantf < 0)
#endif
	{
	s = 1;
	mantf = -mantf;
	}
mantf2 = ldexp(mantf, shift-32+1);
mantlhi = (long)mantf2;
mantl = (unsigned long int)ldexp(mantf2 - mantlhi, 32);
if(mantl & 1)
	add = 1;
mantl >>= 1;
if(mantlhi & 1)
	mantl |= 1L<<31;
mantlhi >>= 1;
if(add)
	{
	mantl++;
	if(mantl == 0)
		mantlhi++;
	}
mantl2 = mantl;
mantl2hi = mantlhi & ~(1L << (52-32));	/* zap implicit leading 1 */
mantl2hi |= ((unsigned long)s << (63-32)) | ((unsigned long)(e & 0x7ff) << (52-32));
buf[0] = (unsigned char)(mantl2 & 0xff);
buf[1] = (unsigned char)((mantl2 >> 8) & 0xff);
buf[2] = (unsigned char)((mantl2 >> 16) & 0xff);
buf[3] = (unsigned char)((mantl2 >> 24) & 0xff);
buf[4] = (unsigned char)(mantl2hi & 0xff);
buf[5] = (unsigned char)((mantl2hi >> (40-32)) & 0xff);
buf[6] = (unsigned char)((mantl2hi >> (48-32)) & 0xff);
buf[7] = (unsigned char)((mantl2hi >> (56-32)) & 0xff);
}

double
fromieeed(unsigned char buf[8])
{
unsigned long mant;
unsigned long manthi;
int e;
int s;
double d;
int shift;

mant = buf[0];
mant |= (unsigned long)buf[1] << 8;
mant |= (unsigned long)buf[2] << 16;
mant |= (unsigned long)buf[3] << 24;
manthi = buf[4];
manthi |= (unsigned long)buf[5] << (40-32);
manthi |= (unsigned long)(buf[6] & 0x0f) << (48-32);

e = ((buf[7] & 0x7f) << 4) | ((buf[6] >> 4) & 0x0f);

s = buf[7] & 0x80;

shift = e - 1023 - 52;
if(e == 0)
	shift++;			/* denormalized */
else	{
	manthi |= (1L << (52-32));	/* restore implicit leading 1 */
	}

d = ldexp(manthi, shift+32) + ldexp(mant, shift);

if(s)	return -d;
else	return d;
}
