/* Code by Ian D. K. Kelly for Chapter 24, "C Unleashed"              */

/* Ch24cnst.c
* 
*  Consts - print the generalized arithmetic limit values
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

/* Print (rather inelegantly) the values of the limit constants for   */
/* arithmetic precision. Note that some compilers will have difficulty*/
/* in printing the values for the largest of these (ULONG_MAX, etc.). */
/* You can always look in <limits.h> and <float.h> - but remember that*/
/* this does not allow your code to determine the values!             */

#include <stdio.h>
#include <limits.h>
#include <float.h>

int main (int argc, char* argv[])
{
    /* Character and integer constants:                               */
	printf("    Characters and integers:\n");
	printf("CHAR_BIT=%d  CHAR_MAX=%d  CHAR_MIN=%d\n",
		CHAR_BIT,CHAR_MAX,CHAR_MIN);
	printf("INT_MAX=%d     INT_MIN=%d\n",  INT_MAX,  INT_MIN);
	printf("LONG_MAX=%ld   LONG_MIN=%ld\n",LONG_MAX, LONG_MIN);
    printf("SCHAR_MAX=%d   SCHAR_MIN=%d\n",SCHAR_MAX,SCHAR_MIN);
	printf("SHRT_MAX=%d    SHRT_MIN=%d\n", SHRT_MAX, SHRT_MIN);
    printf("UCHAR_MAX=%d UINT_MAX=%u ULONG_MAX=%u USHRT_MAX=%d\n",
		    UCHAR_MAX,   UINT_MAX,   ULONG_MAX,   USHRT_MAX);

    /* Floating point constants - that is, for type "float":          */
	printf("    Floating point:\n");
    printf("FLT_ROUNDS=%d FLT_RADIX=%d FLT_MANT_DIG=%d FLT_DIG=%d\n",
		    FLT_ROUNDS,   FLT_RADIX,   FLT_MANT_DIG,   FLT_DIG);
	printf("FLT_MIN_EXP=%d FLT_MIN_10_EXP=%d\n",
	    FLT_MIN_EXP,   FLT_MIN_10_EXP);
	printf("FLT_MAX_EXP=%d FLT_MAX_10_EXP=%d\n",
	    FLT_MAX_EXP,   FLT_MAX_10_EXP);
	printf("FLT_MAX=%e FLT_EPSILON=%e FLT_MIN=%e\n",
	    FLT_MAX,   FLT_EPSILON,   FLT_MIN);

    /* Floating point constants for type "double":                    */   
	printf("    Double precision:\n");
    printf("DBL_MANT_DIG=%d DBL_DIG=%d\n",
		    DBL_MANT_DIG,   DBL_DIG);
	printf("DBL_MIN_EXP=%d DBL_MIN_10_EXP=%d\n",
	    DBL_MIN_EXP,   DBL_MIN_10_EXP);
	printf("DBL_MAX_EXP=%d DBL_MAX_10_EXP=%d\n",
	    DBL_MAX_EXP,   DBL_MAX_10_EXP);
	printf("DBL_MAX=%e DBL_EPSILON=%e DBL_MIN=%e\n",
	    DBL_MAX,   DBL_EPSILON,   DBL_MIN);

    /* Floating point constants for type "long double":               */
	printf("    Long Double precision:\n");
    printf("LDBL_MANT_DIG=%d LDBL_DIG=%d\n",
		    LDBL_MANT_DIG,   LDBL_DIG);
	printf("LDBL_MIN_EXP=%d LDBL_MIN_10_EXP=%d\n",
	    LDBL_MIN_EXP,   LDBL_MIN_10_EXP);
	printf("LDBL_MAX_EXP=%d LDBL_MAX_10_EXP=%d\n",
	    LDBL_MAX_EXP,   LDBL_MAX_10_EXP);
	printf("LDBL_MAX=%e LDBL_EPSILON=%e LDBL_MIN=%e\n",
	    LDBL_MAX,   LDBL_EPSILON,   LDBL_MIN);

	return EXIT_SUCCESS;
}
