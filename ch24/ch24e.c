/* Code by Ian D. K. Kelly for Chapter 24, "C Unleashed"    */
/* Calculate the number "e" to as much accuracy as possible */

/* Ch24E.c
* 
*  E - compute e to as much accuracy as possible for "long double"
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
int main(int argc, char * argv[])
{
	long double dResult;
	long double dWork;
	int i;

	dResult = 1.0;
	dWork = 1.0;

	for (i=1;(i<30);i++)
	{
		dWork /= i;
		dResult += dWork;
	}
	printf("e = %32.28lf\n",dResult);

	return 0;
}
