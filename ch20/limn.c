/*  limn.c - limit lines to n bytes
 *
 *  LIMN - LIMit line length to N bytes
 *
 *  Copyright (C) 2000  Richard Heathfield
 *                      Eton Computer Systems Ltd
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
 *  Richard Heathfield may be contacted by email at:
 *     binary@eton.powernet.co.uk
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void help(void)
{
  char *helpmsg[] =
  {
    "limn",
    "Usage: limn <n>",
    "Example: limn 72",
    "Copies standard input to standard output,",
    "ensuring that no line exceeds n characters",
    "in length. Longer lines are split, and a",
    "terminating \\ character is added to the",
    "line at the split point. n must be at least 3."
  };

  size_t i, j = sizeof helpmsg / sizeof helpmsg[0];

  for(i = 0; i < j; i++)
  {
    puts(helpmsg[i]);
  }
}

int main(int argc, char **argv)
{
  int status = EXIT_SUCCESS;

  char *endp;
  int thischar;
  size_t lim;
  size_t currline = 0;

  if(argc < 2 || strcmp(argv[1], "-?") == 0)
  {
    help();
    status = EXIT_FAILURE;
  }
  else
  {
    lim = (size_t)strtoul(argv[1], &endp, 10);

    if(endp == argv[1] || lim < 3)
    {
      help();
      printf("\nInvalid arg %s\n", argv[1]);
      status = EXIT_FAILURE;
    }
    else
    {
      while((thischar = getchar()) != EOF)
      {
        if(thischar == '\n')
        {
          currline = 0;
        }
        else if(++currline == lim)
        {
          putchar('\\');
          putchar('\n');
          currline = 1;
        }
        putchar(thischar);
      }
    }
  }

  return status;
}
