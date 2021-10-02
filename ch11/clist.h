/*  clist.h - header for circular list lib
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


#ifndef CLIST_H__
#define CLIST_H__

#define CL_SUCCESS    0
#define CL_NO_MEM     1
#define CL_EMPTY      2
#define CL_ZERO_SIZE  3

typedef struct CL_ITEM
{
  int Tag;
  struct CL_ITEM *Prev;
  struct CL_ITEM *Next;
  void *Object;
  size_t Size;
} CL_ITEM;

typedef struct CLIST
{
  CL_ITEM *CurrentItem;
  size_t NumItems;
} CLIST;

CL_ITEM *CLCreate(int Tag, void *Object, size_t Size);

int CLAddItem(CLIST *List,
              int Tag,
              void *Object,
              size_t Size);

int CLUpdate(CLIST *List,
             int NewTag,
             void *NewObject,
             size_t NewSize);

void *CLGetData(CLIST *List,
                int *Tag,
                size_t *Size);

void CLRotate(CLIST *List, int Places);

int CLDelete(CLIST *List);

void CLDestroy(CLIST *List);

int CLWalk(CLIST *List,
           int(*Func)(int, void *, void *),
           void *Args);
#endif
