/*  dllisteg.c - Alternative test driver program for DLLIST
 *
 *  DLLIST - Double-Linked List Library
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
#include <assert.h>

#include "dllist.h"

typedef struct EMAIL
{
  char Name[30];
  char Address[30];
} EMAIL;

int PrintEmail(int Tag, void *Memory, void *Args)
{
  EMAIL *e = Memory;

  assert(Tag == 0);

  printf("%s:%s\n",
         e->Name,
         e->Address);

  return 0;
}

int ParseEmail(EMAIL *e, char *Buffer)
{
  char *Token;
  int Result = 0;

  assert(e != NULL);

  Token = strtok(Buffer, ":\t\n");

  if(Token != NULL)
  {
    strcpy(e->Name, Token);
    Token = strtok(NULL, ":\t\n");
    if(Token != NULL)
    {
      strcpy(e->Address, Token);
      Result = 1;
    }
  }
  return Result;
}

/* This function merely exercises the linked
 * list routines, and is not intended to be
 * an illustration of optimum sorting!
 *
 * (I have to say that, or Dann will poleaxe me.)
 */
DLLIST *SortList(DLLIST *List)
{
  DLLIST *Outer;
  DLLIST *Inner;
  DLLIST *Temp;
  EMAIL *OuterKey;
  EMAIL *InnerKey;

  int Items = DLCount(List);

  if(Items > 1)
  {
    for(Outer = DLGetFirst(List);
        Outer != NULL;
        Outer = DLGetNext(Outer))
    {
      OuterKey = DLGetData(Outer, NULL, NULL);
      for(Inner = DLGetNext(Outer);
          Inner != NULL;
          Inner = DLGetNext(Inner))
      {
        InnerKey = DLGetData(Inner, NULL, NULL);

        if(strcmp(OuterKey->Name, InnerKey->Name) > 0)
        {
          DLExchange(Outer, Inner);
          Temp = Outer;
          Outer = Inner;
          Inner = Temp;
          OuterKey = InnerKey;
        }
      }
    }
  }
  return List;
}

/* This test driver is illustrative,
 * not industrial strength! Input is
 * expected in the format Name:email
 * e.g. Richard:binary@eton.powernet.co.uk
 */
int main(void)
{
  EMAIL Email = {0};
  EMAIL *e = NULL;
  DLLIST *List = NULL;
  char Buffer[sizeof(EMAIL) + sizeof '\n'] = {0};
  int AllsWell = 1;

  while(AllsWell &&
        NULL != fgets(Buffer,
                      sizeof Buffer,
                      stdin))
  {
    if(ParseEmail(&Email, Buffer))
    {
      if(DL_SUCCESS !=
          DLAddAfter(&List, 0, &Email, sizeof Email))
      {
        puts("Couldn't allocate enough memory.");
        AllsWell = 0;
      }
    }
    else
    {
      printf("Couldn't parse line, data [%s].", Buffer);
      AllsWell = 0;
    }
  }

  if(AllsWell)
  {
    /* Print the list, unsorted */

    printf("\nUnsorted list\n\n");
    DLWalk(List, PrintEmail, NULL);

    /* Sort the list (peace, Dann!) */
    List = SortList(List);

    /* Print the list, sorted */

    printf("\nSorted list\n\n");
    DLWalk(List, PrintEmail, NULL);
  }

  /* Destroy the list */
  DLDestroy(&List);

  return 0;
}
