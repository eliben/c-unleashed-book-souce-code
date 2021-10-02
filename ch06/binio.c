/**************************************/
/*                                    */
/*   Code from the book C Unleashed   */
/*   Macmillan, 2000                  */
/*   Chapter 6: Data Files            */
/*   Steve Summit 2000-03-17          */
/*                                    */
/**************************************/

#include <stdio.h>

#define TRUE 1
#define FALSE 0

extern void toieee(float, unsigned char [4]);
extern float fromieee(unsigned char [4]);

extern void toieeed(double, unsigned char [8]);
extern double fromieeed(unsigned char [8]);

void putint(int i, FILE *ofp)
{
	putc(i & 0xff, ofp);
	putc((i >> 8) & 0xff, ofp);
}

void putlong(long int i2, FILE *ofp)
{
	putc(i2 & 0xff, ofp);
	putc((i2 >> 8) & 0xff, ofp);
	putc((i2 >> 16) & 0xff, ofp);
	putc((i2 >> 24) & 0xff, ofp);
}

/*
 *  This version of putfloat() is more complete than the one printed
 *  in the chapter.  It defers the packing step to the toieee()
 *  function (in ieee754.c), which handles denormalized numbers
 *  and rounding.
 */

void putfloat(float f, FILE *ofp)
{
	unsigned char buf[4];
	toieee(f, buf);
	fwrite(buf, 1, 4, ofp);
}

void putdouble(double d, FILE *ofp)
{
	unsigned char buf[8];
	toieeed(d, buf);
	fwrite(buf, 1, 8, ofp);
}

int getint(int *ip, FILE *ifp)
{
	int i;
	int c;
	if((c = getc(ifp)) == EOF)
		return FALSE;
	i = c;
	if((c = getc(ifp)) == EOF)
		return FALSE;
	i |= c << 8;
	*ip = i;
	return TRUE;
}

int getlong(long int *ip, FILE *ifp)
{
	long int i2;
	unsigned char buf[4];
	if(fread(buf, 1, 4, ifp) != 4)
		return FALSE;
	i2 = buf[0];
	i2 |= (unsigned long)buf[1] << 8;
	i2 |= (unsigned long)buf[2] << 16;
	i2 |= (unsigned long)buf[3] << 24;
	*ip = i2;
	return TRUE;
}

/*
 *  This version of getfloat() is more complete than the one printed
 *  in the chapter.  It defers the unpacking step to the fromieee()
 *  function (in ieee754.c), which handles denormalized numbers.
 */

int getfloat(float *fp, FILE *ifp)
{
	unsigned char buf[4];
	if(fread(buf, 1, 4, ifp) != 4)
		return FALSE;
	*fp = fromieee(buf);
	return TRUE;
}

int getdouble(double *dp, FILE *ifp)
{
	unsigned char buf[8];
	if(fread(buf, 1, 8, ifp) != 8)
		return FALSE;
	*dp = fromieeed(buf);
	return TRUE;
}
