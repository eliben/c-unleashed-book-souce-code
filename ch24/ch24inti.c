/* Code by Ian D. K. Kelly for Chapter 24, "C Unleashed"              */

/* Ch24inti.c
* 
*  intint - Sample code for integer division
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

/* This divides two by three, in various ways, and displays the answer*/
#include <stdio.h>

int main (int argc, char * argv[])
{
    /* Set up the "int" variables whose values are 2 and 3            */
    int iTwo = 2;
    int iThree = 3;
    /* Set up the variables into which the answers will be placed     */
    int iA;
    double dB;
    double dC;
    double dD;

    /* Divide "int" by "int", placing the answer into "int". This is  */
    /* an "all int" expression, and inevitably results in truncation: */
    iA = iTwo / iThree;

    /* Divide "int" by "int", placing the answer into "double". This  */
    /* is also an "all int" expression on the right hand side, even   */
    /* though the target is "double". Hence we again have truncation: */
    dB = iTwo / iThree;

    /* Divide "int cast to double" by "int", placing the answer into  */
    /* "double". The right hand side is an expression of mixed type,  */
    /* which, by the rules of C, will be promoted upwards to type     */
    /* double, prior to the division taking place. Hence in this case */
    /* we do NOT hve truncation, and the answer is the expected two-  */
    /* thirds (0.666666...):                                          */
    dC = (double) iTwo / iThree;

    /* Divide "int" by "int", casting the whole expression to double, */
    /* placing the answer into "double". Again the parenthesised sub- */
    /* expression is an "all int" expression, and hence will be evalu-*/
    /* ated and truncated prior to performing the cast. So the answer */
    /* is, slightly unexpectedly, the truncated result:               */
    dD = (double) (iTwo / iThree);

    printf("ia = %d dB=%f dC=%f dD=%f\n",iA,dB,dC,dD);

    return 0;

    /* You might like to try this routine with other mixes of types,  */
    /* to see where type conversion takes place on your system. The   */
    /* descriptions above for types "int" and "double" are strictly   */
    /* according to the ANSI Standard.                                */
}

