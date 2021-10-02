/*  parity.c - contains a simple command line demonstration
 *             of parity generation and its results
 *
 *  Copyright (C) 2000  Jack Klein
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

unsigned int even_parity(unsigned int ch)
{
  int temp = 0;
  int count;

  /* trim to 7 bit ASCII  */
  ch &= 0x7f;

  /* set temp to 0 for even parity or 1 for odd parity  */
  for (count = 0; count < 8; ++count)
  {
      temp ^= ((ch >> count) & 1);
  }

  if (temp)
  {
    ch |= 0x80;
  }

  return ch;
}

int main(void)
{
  char buff[22];
  char *cp;

  for ( ; ; )
  {
    printf("\nEnter up to 20 characters: ");
    fflush(stdout);

    if (fgets(buff, sizeof buff, stdin) == NULL ||
          buff [0] == '\n')
    {
      puts("Goodbye!");
      return 0;
    }

    /* remove newline from string if present  */
    if ((cp = strchr(buff, '\n')) != NULL)
    {
      *cp = '\0';
    }

    for (cp = buff; *cp != '\0'; ++cp)
    {
      printf("0x%02X is 0x%02X with even parity\n",
        *cp & 0x7f, even_parity(*cp & 0x7f));
    }
  }
}
