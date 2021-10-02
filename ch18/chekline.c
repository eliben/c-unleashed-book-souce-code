/*  chekline.c - contains code to illustrate computing simple
 *               checksums on strings read from stdin with
 *               the standard function fgets()
 *
 *  Copyright (C) 2000  Jack Klein
 *                      JK Technology
 *                      Macmillan Computer Publishing
 *
 *  This program is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU General
 *  Public License as published by the Free Software
 *  Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will
 *  be useful, but WITHOUT ANY WARRANTY; without even the
 *  implied warranty of MERCHANTABILITY or FITNESS FOR A
 *  PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General
 *  Public License along with this program; if not, write
 *  to the Free Software Foundation, Inc., 675 Mass Ave,
 *  Cambridge, MA 02139, USA.
 *
 *  Jack Klein may be contacted by email at:
 *     The_C_Guru@yahoo.com
 *
 */

#include <stdio.h>
#include <string.h>

int main(void)
{
  char buff[100];
  int count;
  unsigned long sum;
  char *nl;

  for ( ; ; )
  {
    printf("Enter a string: ");
    fflush(stdout);
    if ( fgets(buff, sizeof buff, stdin) == NULL
        || buff[0] == '\n')
    {
      return 0;
    }
    if ((nl = strchr(buff, '\n')) != NULL)
      *nl = '\0';

    for (count = 0, sum = 0; buff[count] != '\0'; ++count)
    {
      sum += (unsigned char)buff[count];
    }
	printf("The checksum of \"%s\" is %lu decimal, %lX hex\n",
            buff, sum, sum);
  }
}
