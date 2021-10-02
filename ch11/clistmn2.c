/*  clistmn2.c - alternative test driver for circular list
 *  library (simulates a child's merry-go-round)
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

int GetRnd(int Num)
{
  return (int)(Num * (rand() / ((double)RAND_MAX + 1.0)));
}

int PrintChildName(int Tag, void *Object, void *Args)
{
  printf("%s is on the merry-go-round!\n", (char *)Object);

  return 0;
}

void RemoveNewLine(char *s)
{
  char *p = strchr(s, '\n');
  if(p != NULL)
  {
    *p = '\0';
  }
}

int main(void)
{
  char buffer[32];
  CLIST Carousel = {0};
  int Result = EXIT_SUCCESS;

  char *InitNames[] =
  {
    "Matt", "Liz", "Nick", "Alex", "Amanda"
  };
  int i;
  int j;

  srand((unsigned)time(NULL));

  puts("Welcome to the CLIST merry-go-round!");
  puts("------------------------------------\n");
  puts("We'll start off with five children\n");

  for(i = 0; EXIT_SUCCESS == Result && i < 5; i++)
  {
    if(CL_SUCCESS !=
          CLAddItem(&Carousel,
                    0,
                    InitNames[i],
                    strlen(InitNames[i]) + 1))
    {
      printf("Insufficient memory. Sorry.\n");
      Result = EXIT_FAILURE;
    }
  }

  while(EXIT_SUCCESS == Result && Carousel.NumItems > 0)
  {
    CLWalk(&Carousel, PrintChildName, NULL);

    if(0 == GetRnd(3))
    {
      j = GetRnd(Carousel.NumItems);
      for(i = 0; i < j; i++)
      {
        PrintChildName(0,
                       CLGetData(&Carousel, NULL, NULL),
                       NULL);
        CLRotate(&Carousel, 1);
      }

      printf("The merry-go-round has stopped.\n");
      if(0 == GetRnd(6))
      {
        printf("Please type in a child's name.\n");
        if(NULL == fgets(buffer, sizeof buffer, stdin))
        {
          Result = EXIT_FAILURE;
        }
        else
        {
          RemoveNewLine(buffer);
          if(CL_SUCCESS !=
                CLAddItem(&Carousel,
                          0,
                          buffer,
                          strlen(buffer) + 1))
          {
            printf("Insufficient memory. Sorry.\n");
            Result = EXIT_FAILURE;
          }
          else
          {
            printf("%s gets on the merry-go-round!\n",
                   buffer);
          }
        }
      }
      else if(0 == GetRnd(3))
      {
        printf("%s gets off the merry-go-round!\n",
               (char *)CLGetData(&Carousel, NULL, NULL));
        CLDelete(&Carousel);
      }
      else
      {
        printf("Nobody got on or off.\n");
      }

      if(Carousel.NumItems > 0)
      {
        printf("Here we go again!\n");
      }
    }
  }

  printf("That-a-that-a-that-a-that's all, folks!\n");

  CLDestroy(&Carousel);
  return Result;
}
