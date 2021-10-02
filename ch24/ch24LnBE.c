/* Code by Ian D. K. Kelly for Chapter 24, "C Unleashed"              */

/* Ch24LnBE.c
* 
*  LongBigE - compute e to ten thousand places, using long int
* 
*  Copyright (C) 1999  Ian D. K. Kelly, 
*                      idkk Consultancy Ltd.
*                      Macmillan Computer Publishing 
* 
*  This program is free software; you can redistribute it and/or modify 
*  it under the terms of the GNU General Public License as published by 
*  the Free Software Foundation; either version 2 of the License, or 
*  (at your option) any later version. 
* 
*  This program is distributed in the hope that it will be useful, 
*  but WITHOUT ANY WARRANTY; without even the implied warranty of 
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
*  GNU General Public License for more details. 
* 
*  You should have received a copy of the GNU General Public License 
*  along with this program; if not, write to the Free Software 
*  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. 
* 
*  Ian Kelly may be contacted at idkk@idkk.com
* 
*/ 

/* The comments have been removed from this code. You will find it    */
/* instructive to construct comments about it - both the techniques   */
/* and the specific values. For a sample set of comments see the file */
/* Ch24BigE.c                                                         */

#include <stdio.h>
#include <limits.h>

#define ILIMIT 3300

#if (LONG_MAX>(ILIMIT*100000))
#define NUMBER_BASE 100000
#define DIGS_IN_BASE 5
#else
#define NUMBER_BASE 10
#define DIGS_IN_BASE 1
#endif

#if (DIGS_IN_BASE==5)
#define NUMBER_WIDTH 2001
#else
#define NUMBER_WIDTH 10001
#endif

int LongDivide ( long int * Numerator, long int * Denominator, long int * Answer );
int LongAdd ( long int * iFirst, long int * iSecond, long int * iAnswer );

int main(int argc, char * argv[])
{
    long int aResult[NUMBER_WIDTH+1];
    long int aWork[NUMBER_WIDTH+1];
	long int i;
    int j;

    aResult[0] = NUMBER_WIDTH;
    aResult[1] = 1;
    aWork[0] = NUMBER_WIDTH;
    aWork[1] = 1;
    for (i=2;(i<=NUMBER_WIDTH);i++)
    {
	aResult[i] = 0;
	aWork[i] = 0;
    }
	
	for (i=1;(i<ILIMIT);i++)
	{
	j = LongDivide ( aWork, &i, aWork );
	if (j<=0)
	{
	    i = ILIMIT + 1;
	}
	LongAdd ( aWork, aResult, aResult );
	}

    printf("e = ");
    printf("%1.1d.",aResult[1]);
    for (i=2;(i<NUMBER_WIDTH);i++)
    {
#if (DIGS_IN_BASE==5)
	printf("%5.5d ",aResult[i]);
#endif
#if (DIGS_IN_BASE==1)
	printf("%1.1d",aResult[i]);
	if ((i % 5)==1)
	   printf(" ");
#endif
	if ((i % 10)==0)
	{
	    printf("\n");
	    if ((i % 100)==0)
	    {
		printf("\n");
	    }
	}
    }

    printf("\n");

	return 1;
}

int LongDivide ( long int * Numerator, long int * Denominator, long int * Answer )
{
    int iStatus = 0;
    int j = 0;
    long int k = 0;
    long int m = 0;
    long int d = 0;

    d = *Denominator;

    Answer[0] = Numerator[0];
    for (j=1;(j<=Numerator[0]);j++)
    {
	if (k>0)
	    m = Numerator[j] + (k * NUMBER_BASE);
	else
	    m = Numerator[j];
	Answer[j] = m / d;
	if (Answer[j] !=0)
	    iStatus = 1;
	k = m % d;
    }

    return iStatus;
}

int LongAdd ( long int * iFirst, long int * iSecond, long int * iAnswer )
{
    int iStatus = 0;
    int i = 0;
    int j = 0;
    long int k = 0;
    long int m = 0;

    i = iFirst[0];
    k = 0;
    for (j=i;(j>0);j--)
    {
	if (iFirst[j]!=0)
	    iStatus = 1;
	m = iFirst[j] + iSecond[j] + k;
	k = 0;
	while (m>=NUMBER_BASE)
	{
	    m -= NUMBER_BASE;
	    k += 1;
	}
	iAnswer[j] = m;
    }

    return iStatus;
}

