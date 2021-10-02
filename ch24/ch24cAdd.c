/* Code by Ian D. K. Kelly for "C Unleashed", chapter 24               */

/* Ch24cAdd.c
* 
*  cAdd - add very long umers in strings of integers
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

#include <stdio.h>
#include <limits.h>

/* Indicate the precision to which all arithmetic variables are to be */
/* held. This implies that every number-style string will have this   */
/* many characters (plus one):                                        */

#define MAX_PRECISION 12000

/* The following routine takes three arguments, each of which is a    */
/* pointer to a number-style string. All of these strings are the same*/
/* length, and this routine assumes that there will be no "carry" out */
/* of the topmost place. It adds together the first and the second    */
/* operands, and places the result in the third operand. The arguments*/
/* are presumed to be in integer format, with no decimal places.      */

int cAdd ( char * aOne, char * aTwo, char * aThree )
{
    int iStatus = 0;
    int carry = 0;
    int i = 0;
    int j = 0;

    /* Addition is performed from right to left. So we start at the   */
    /* top-most character, and proceed through smaller and smaller    */
    /* subscripts. At each stage we incorporate the carry from the    */
    /* position before, and compute the carry out to the next position*/
    for (i=MAX_PRECISION;(i>=0);i--)
    {
        /* Place into variable "j" the decimal result of adding these */
        /* two digits, plus the carry from the previous pair, if any: */
        /* <- digit one ->   <- digit two ->    carry                 */

        j = *(aOne+i) - '0' + *(aTwo+i) - '0' + carry;

        /* The following pair of statements use both the integral     */
        /* quoient and remainder (modulus) when dividing by the number*/
        /* base (which in this case is ten). Note that some C com-    */
        /* pilers will optimize this pair of statements to compute    */
        /* both of these in a single machine instruction.             */

        carry = j / 10;
        j = j % 10;

        /* Set the outgoing digit to the computed value, plus the     */
        /* amount required to turn a decimal number into a numeric    */
        /* character (which is the value of the character zero '0'):  */

        *(aThree+i) = j + '0';
    }
    return iStatus;
}

/* Can you think of a way of re-expressing the four main statements in*/
/* the innermost loop so that we do not have to both subtract and then*/
/* add back in the value of '0'? Only one pair can be cancelled, and  */
/* the resultant code is a little less readable, but faster.          */
