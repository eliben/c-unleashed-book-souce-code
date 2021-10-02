/*  sllistmn.c - test driver for single linked list lib
 *
 *  SLLIST - Single-Linked List Library
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
#include <assert.h>

#include "sllist.h"

typedef struct BOOK
{
  char Title[30];
  char Author[30];
} BOOK;

typedef struct FIELD_INFO
{
  int TitleWidth;
  int AuthWidth;
} FIELD_INFO;

int PrintBook(int Tag, void *Memory, void *Args)
{
  BOOK *b = Memory;
  FIELD_INFO *f = Args;

  assert(Tag == 0);

  printf("Read %*s, by %*s\n",
         f->TitleWidth,
         b->Title,
         f->AuthWidth,
         b->Author);

  return 0;
}


int main(void)
{
  BOOK Book[] =
  {
    {"Expert C Programming", "van der Linden"},
    {"C Programming FAQs", "Summit"},
    {"C++ Programming Language", "Stroustrup"},
    {"Algorithms in C", "Sedgewick"},
    {"Teach Yourself BCB", "Reisdorph"},
    {"The Standard C Library", "Plauger"},
    {"C++ Unleashed", "Liberty"},
    {"Data Structures & Algorithms", "Lafore"},
    {"C Programming Language", "Kernighan & Ritchie"},
    {"Linux Unleashed", "Husain and Parker"},
    {"C Unleashed", "Heathfield & Kirby"},
    {"C : A Reference Manual", "Harbison & Steele"},
    {"DOS Programmers Reference", "Dettmann & Johnson"},
    {"C: How to Program", "Deitel & Deitel"},
    {"Builder Unleashed", "Calvert"},
    {"UNIX Unleashed", "Burk and Horvath"}

  };

  SLLIST *List = NULL;
  SLLIST *Removed = NULL;

  BOOK *Data;

  FIELD_INFO FldInfo = { 30, 30};
  size_t NumBooks = sizeof Book / sizeof Book[0];

  size_t i;

  /* Populate the list */
  for(i = 0; i < NumBooks; i++)
  {
    if(SL_SUCCESS !=
          SLFront(&List, 0, Book + i, sizeof(BOOK)))
    {
      puts("Couldn't allocate enough memory.");
      SLDestroy(&List);
      exit(EXIT_FAILURE);
    }
  }

  /* Print the list */
  SLWalk(List, PrintBook, &FldInfo);

  /* Remove one item */
  Removed = List;

  for(i = 0; i < NumBooks / 2; i++)
  {
    Removed = Removed->Next;
  }

  Data = SLGetData(Removed->Next, NULL, NULL);
  printf("\nRemoving title %s\n\n", Data->Title);
  SLDeleteNext(Removed);

  /* Print the list again to confirm deletion */
  SLWalk(List, PrintBook, &FldInfo);

  /* Destroy the list */
  SLDestroy(&List);

  return 0;
}
