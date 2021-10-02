/* Code by Ian D. K. Kelly for Chapter 24, "C Unleashed"              */

/* Ch24Drft.c
* 
*  Drift - arithmetic drift detection
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

/* This code may be used, with modifications, to detect arithmetic    */
/* "drift" in your environment. You will need to consider:            */
/*   (a)  different arithmetic types, other than "double, and         */
/*   (b)  different values for "dB" and "dA" which may produce errors */
/*        of rounding due to bias.                                    */

#include <stdio.h>

int main (int argc, char * argv[])
{
    double dA;
    double dB;
    double dC;
    double dD;
    double dE;
    int i;

    dA = 1.0;
    dB = 0.555555555555555555555555555555;
    dC = dA + dB;
    dC -= dA;
    dE = dB - dC;
    printf("diff=%20.18f\n",dE);
    dD = dB;
    dE = dA;

    /* Drift may require many more iterations than 1000 - you need to */
    /* experiment with this too:                                      */
    for (i=0;(i<1000);i++)
    {
        dD = (((dA + dD) - dA) + dA) - dA;
        dE = (((dB + dE) - dB) + dB) - dB;
    }

    printf("dA=%20.18f dB=%20.18f dC=%20.18f\n",dA,dB,dC);
    printf("dD=%20.18f dE=%20.18f\n",dD,dE);

    return 0;
}
