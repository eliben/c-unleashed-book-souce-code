/* Code by Ian D. K. Kelly for Chapter 24, "C Unleashed"              */


/* Ch24BigE.c
* 
*  BigE - Compute "e" to ten thousand places
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

/* This computes the value of "e" to ten thousand places. This code   */
/* can easily be modified to extend the number of places at will - but*/
/* beware that it takes time to compute more places! The illustration */
/* is of very-high precision arithmetic, using a subset of the opera- */
/* tions. Note that this representation will NOT work if you need to  */
/* compute much higher numbers than those used here, as each element  */
/* must be able to hold the square of the base by itself. You could   */
/* investigate an alternative algorithm (or method of calculating this*/
/* one) which does not have that restriction.                         */

#include <stdio.h>
#include <stdlib.h>
/* Note that we are referring to the arithmetic limits, so we need to */
/* include the library file that defines those limits:                */
#include <limits.h>

/* The constant ILIMIT is the number of iterations required to get the*/
/* result accurate to ten thousand places. Note that 3300 is slightly */
/* too high - the code stops repeating when "enough" iterations have  */
/* been performed. You might like to look at (a) how fast this number */
/* (ILIMIT) increases with respect to the number of decimal places you*/
/* require in the answer, and (b) how you can estimate this, without  */
/* experiment. Obviously, the easiest method is to draw a graph or    */
/* construct a table of "iterations required" against "decimal places"*/
/* by experiment - but there is an abstract relationship.             */
/* A simpler question is "what is the lowest value of ILIMIT required */
/* by this program, to compute e to ten thousand places?"             */
#define ILIMIT 3300

/* Depending upon the number of bits in an "int" set (a) the base for */
/* the arithmetic we will be doing, and (b) the number of decimal dig-*/
/* its that this base represents:                                     */
#if (INT_MAX>(ILIMIT*100000))
#define NUMBER_BASE 100000
#define DIGS_IN_BASE 5
#else
#define NUMBER_BASE 10
#define DIGS_IN_BASE 1
#endif

/* According to the number of digits in the base, set a constant to   */
/* the number of array elements required to hold a number to the      */
/* required precision:                                                */
#if (DIGS_IN_BASE==5)
#define NUMBER_WIDTH 2001
#else
#define NUMBER_WIDTH 10001
#endif

/* Pre-declarations of the internal functions in this program:        */
int LongDivide ( int * Numerator, int * Denominator, int * Answer );
int LongAdd ( int * iFirst, int * iSecond, int * iAnswer );

int main(int argc, char * argv[])
{
    int aResult[NUMBER_WIDTH+1];
    int aWork[NUMBER_WIDTH+1];
	int i;
    int j;

    /* Indicate the length of the result array and the work array in  */
    /* their initial elements, and set their values to one. Note that */
    /* this program makes the assumption that the first of the value  */
    /* words - and just the first - represents that part of the value */
    /* which is to the left of the decimal point, and the rest of the */
    /* value is that which is to the right of the decimal point. This */
    /* is not a general representation, and should you require a more */
    /* flexible set of routines, you will need to consider the code in*/
    /* CH24AOKw.c and the remarks in the comments to that code for    */
    /* extending it to cover general number styles.                   */
    aResult[0] = NUMBER_WIDTH;
    aResult[1] = 1;
    aWork[0] = NUMBER_WIDTH;
    aWork[1] = 1;
    for (i=2;(i<=NUMBER_WIDTH);i++)
    {
	aResult[i] = 0;
	aWork[i] = 0;
    }
	
    /* Repeat the calculation, adding one new term, for the number of */
    /* iterations that we have pre-set:                               */
	for (i=1;(i<ILIMIT);i++)
	{
	/* Divide the work variable by the current iteration-number,  */
	/* to create the reciprocal of the factorial of the iteration-*/
	/* number:                                                    */
	j = LongDivide ( aWork, &i, aWork );

	/* If this is a zero term then we have finished the calcula-  */
	/* tion, and all subsequent terms will be even smaller. So set*/
	/* the loop control variable to indicate the end of the loop: */
	if (j<=0)
	{
	    i = ILIMIT + 1;
	}
	/* If non zero, add the new term on to the accumulating answer*/
	else LongAdd ( aWork, aResult, aResult );
	}   /* end of "for/i" iteration through successive terms          */

    /* We have computed "e" to the desired number of places. Print it:*/
    printf("e = ");
    printf("%1.1d.",aResult[1]);
    for (i=2;(i<NUMBER_WIDTH);i++)
    {
	/* We would like to print the number in groups of five digits.*/
	/* These are either done five at a time, or we do them one at */
	/* a time and insert a space when needed, depending upon the  */
	/* number of digits in the chosen base:                       */
#if (DIGS_IN_BASE==5)
	printf("%5.5d ",aResult[i]);
#endif
#if (DIGS_IN_BASE==1)
	printf("%1.1d",aResult[i]);
	if ((i % 5)==1)
	   printf(" ");
#endif

	/* In any case, after every ten groups of five put a new line,*/
	/* and another new line every ten lines (grouping the digits  */
	/* into "packs" of five hundred:                              */
	if ((i % 10)==0)
	{
	    printf("\n");
	    if ((i % 100)==0)
	    {
		printf("\n");
	    }   /* end of "if/then" printing newilne after ten lines  */
	}       /* end of "if/then" printing newline after ten groups */
    }           /* end of "for/i" iteration along array               */

    /* One final new line, in case this code is modified to produce a */
    /* number of digits that is not a multiple of a hundred. You could*/
    /* also write a terminating message here:                         */
    printf("\n");

	return EXIT_SUCCESS;
}

int LongDivide ( int * Numerator, int * Denominator, int * Answer )
{
    int iStatus = EXIT_SUCCESS;
    int j = 0;
    int k = 0;
    int m = 0;
    int d = 0;

    /* This routine divides the very long number in its first arg-    */
    /* ument by the single number in its second argument to give the  */
    /* very long numer in its third argument. Note that the first and */
    /* third arguments MAY point to the same incoming array - no      */
    /* assumption is made here about that:                            */

    /* Get a local copy of the denominator from the caller. This has  */
    /* no effect, except (possibly) for optimisation. The variable "d"*/
    /* could be declared as "register" if, in your environment, this  */
    /* has any effect:                                                */
    d = *Denominator;

    Answer[0] = Numerator[0];

    /* Compute the next highest ("to the right") term in the division,*/
    /* bringing in the "carry" from the term before:                  */
    for (j=1;(j<=Numerator[0]);j++)
    {
	if (k>0)
	    m = Numerator[j] + (k * NUMBER_BASE);
	else
	    m = Numerator[j];
	/* Compute the quotient of this one term, and the remainder,  */
	/* which will be taken forward as the "carry". Note that these*/
	/* two statements should be kept adjacent to assist those com-*/
	/* pilers that can optimize them into a single machine instuc-*/
	/* tion which computes both the quotient and the remainder at */
	/* the same time:                                             */
	Answer[j] = m / d;
	k = m % d;
	/* If we have a non-zero answer, indicate this in the flag:   */
	if (Answer[j] !=0)
	    iStatus = EXIT_FAILURE;
    }   /* end of "for/i" loop along the numerator being divided      */

    return iStatus;
}

int LongAdd ( int * iFirst, int * iSecond, int * iAnswer )
{
    int iStatus = EXIT_SUCCESS;
    int i = 0;
    int j = 0;
    int k = 0;
    int m = 0;

    /* This routine adds the very long number in the first argument to*/
    /* the very long number in the second argument, giving the third  */
    /* argument. Note that this routine makes no assumption about the */
    /* caller having passed in different addresses for the first and  */
    /* third arguments, which may well be the same.                   */

    /* Set the initial conditions, including clearing the carry:      */
    i = iFirst[0];
    k = 0;

    /* Loop backwards down the arrays, from right to left, adding the */
    /* terms, with any carry from the previous term, and calculate the*/
    /* carry out to the next term:                                    */
    for (j=i;(j>0);j--)
    {
	/* If there is a non-zero value, indicate this in the flag:   */
	if (iFirst[j]!=0)
	    iStatus = EXIT_FAILURE;
	/* Perform the elementary addition:                           */
	m = iFirst[j] + iSecond[j] + k;
	/* compute the carry, according to the resultant value:       */
	k = 0;
	while (m>=NUMBER_BASE)
	{
	    m -= NUMBER_BASE;
	    k += 1;
	}
	/* Tell the caller the answer to this bit of the addition:    */
	iAnswer[j] = m;
    }

    return iStatus;
}

