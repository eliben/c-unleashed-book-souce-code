/*  clistmn.c - test driver for circular list lib
 *  (Solves the Josephus problem)
 *
 *  CLIST - Circular List Library
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
#include <time.h>

#include "clist.h"

int main(void)
{
  char *Intro[] =
  {
    "The Josephus Problem",
    "--------------------",
    " ",
    "Consider a ring of N items. If the Kth item",
    "is eliminated, there will now be N - 1 items.",
    "If this procedure is performed iteratively,",
    "eventually there will be just one item",
    "remaining. Which is it?",
    " ",
    "This program provides the answer.",
    " ",
    NULL
  };
  char **Text;
  char buffer[32];
  char *endp;
  CLIST Circle = {0};
  int Result = EXIT_SUCCESS;
  unsigned long N;
  unsigned long K;
  unsigned long i;

  for(Text = Intro; *Text != NULL; ++Text)
  {
    puts(*Text);
  }

  puts("\nHow many items in the ring?");

  if(NULL == fgets(buffer, sizeof buffer, stdin))
  {
    puts("Program aborted.");
    exit(EXIT_FAILURE);
  }

  N = strtoul(buffer, &endp, 10);
  if(endp == buffer || N == 0)
  {
    puts("Program aborted.");
    Result = EXIT_FAILURE;
  }
  else
  {
    puts("Count how many items before removing one?");

    if(NULL == fgets(buffer, sizeof buffer, stdin))
    {
      puts("Program aborted.");
      exit(EXIT_FAILURE);
    }

    K = strtoul(buffer, &endp, 10);
    if(endp == buffer || K == 0)
    {
      puts("Program aborted.");
      Result = EXIT_FAILURE;
    }
  }

  for(i = 0; EXIT_SUCCESS == Result && i < N; i++)
  {
    if(CL_SUCCESS !=
          CLAddItem(&Circle,
                    0,
                    &i,
                    sizeof i))
    {
      printf("Insufficient memory. Sorry.\n");
      Result = EXIT_FAILURE;
    }
  }

  if(EXIT_SUCCESS == Result)
  {
    while(Circle.NumItems > 1)
    {
      CLRotate(&Circle, K);
      printf("Removing item %lu.\n",
                 *(unsigned long *)CLGetData(&Circle,
                                             NULL,
                                             NULL));
      CLDelete(&Circle);
      /* Removing an item makes the next current,
       * which puts our count out by one, so roll
       * the circle back one node.
       */
      CLRotate(&Circle, -1);
    }

    printf("The last item is %lu.\n",
                 *(unsigned long *)CLGetData(&Circle,
                                             NULL,
                                             NULL));
  }

  CLDestroy(&Circle);
  return Result;
}
