/*  c11_022.c - naive single linked list, using an array.
 *
 *  C11_022 - single linked lists
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

typedef struct ITEM
{
  char Title[30];
  char Author[30];
  int Next;
} ITEM;

int main(void)
{
  ITEM List[] =
  {
    {"UNIX Unleashed", "Burk and Horvath", 2},
    {"Algorithms in C", "Sedgewick", 9},
    {"Builder Unleashed", "Calvert", 10},
    {"C++ Unleashed", "Liberty", 12},
    {"Linux Unleashed", "Husain and Parker", 8},
    {"Teach Yourself BCB", "Reisdorph", 1},
    {"Data Structures & Algorithms", "Lafore", 3},
    {"DOS Programmers Reference", "Dettmann & Johnson", 11},
    {"C Programming Language", "Kernighan & Ritchie", 6},
    {"C++ Programming Language", "Stroustrup", 13},
    {"C: How to Program", "Deitel & Deitel", 7},
    {"C : A Reference Manual", "Harbison & Steele", 15},
    {"The Standard C Library", "Plauger", 5},
    {"C Programming FAQs", "Summit", 14},
    {"Expert C Programming", "van der Linden", -1},
    {"C Unleashed", "Heathfield & Kirby", 4}
  };

  int Current = 0;

  while(Current != -1)
  {
    printf("Read %s, by %s.\n",
           List[Current].Title,
           List[Current].Author);
    Current = List[Current].Next;
  }

  return 0;
}
