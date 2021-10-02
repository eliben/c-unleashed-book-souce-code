/*  sllist.c - source for single linked list lib
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

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "sllist.h"

int SLAdd(SLLIST **Item,
          int Tag,
          void *Object,
          size_t Size)
{
  SLLIST *NewItem;
  int Result = SL_SUCCESS;

  assert(Item != NULL);

  if(Size > 0)
  {
    NewItem = malloc(sizeof *NewItem);
    if(NewItem != NULL)
    {
      NewItem->Tag    = Tag;
      NewItem->Size   = Size;
      NewItem->Object = malloc(Size);

      if(NewItem->Object != NULL)
      {
        memcpy(NewItem->Object, Object, Size);
        /* Handle empty list */
        if(NULL == *Item)
        {
          NewItem->Next = NULL;
          *Item = NewItem;
        }
        else /* Insert just after current item */
        {
          NewItem->Next = (*Item)->Next;
          (*Item)->Next = NewItem;
        }
      }
      else
      {
        free(NewItem);
        Result = SL_NO_MEM;
      }
    }
    else
    {
      Result = SL_NO_MEM;
    }
  }
  else
  {
    Result = SL_ZERO_SIZE;
  }

  return Result;
}

int SLFront(SLLIST **Item,
            int Tag,
            void *Object,
            size_t Size)
{
  int Result = SL_SUCCESS;

  SLLIST *p = NULL;

  assert(Item != NULL);

  Result = SLAdd(&p, Tag, Object, Size);
  if(SL_SUCCESS == Result)
  {
    p->Next = *Item;
    *Item = p;
  }

  return Result;
}

int SLAppend(SLLIST **Item,
             int Tag,
             void *Object,
             size_t Size)
{
  int Result = SL_SUCCESS;
  SLLIST *EndSeeker;

  assert(Item != NULL);

  if(NULL == *Item)
  {
    Result = SLAdd(Item, Tag, Object, Size);
  }
  else
  {
    EndSeeker = *Item;
    while(EndSeeker->Next != NULL)
    {
      EndSeeker = EndSeeker->Next;
    }
    Result = SLAdd(&EndSeeker, Tag, Object, Size);
  }

  return Result;
}

int SLUpdate(SLLIST *Item,
             int NewTag,
             void *NewObject,
             size_t NewSize)
{
  int Result = SL_SUCCESS;

  void *p;
  
  if(NewSize > 0)
  {
    p = realloc(Item->Object, NewSize);
    if(NULL != p)
    {
      Item->Object = p;
      memmove(Item->Object, NewObject, NewSize);
      Item->Tag = NewTag;
      Item->Size = NewSize;
    }
    else
    {
      Result = SL_NO_MEM;
    }
  }
  else
  {
    Result = SL_ZERO_SIZE;
  }

  return Result;
}

void *SLGetData(SLLIST *Item,
                int *Tag,
                size_t *Size)
{
  void *p = NULL;

  if(Item != NULL)
  {
    if(Tag != NULL)
    {
      *Tag = Item->Tag;
    }
    if(Size != NULL)
    {
      *Size = Item->Size;
    }
    p = Item->Object;
  }

  return p;
}

SLLIST *SLDeleteThis(SLLIST *Item)
{
  SLLIST *NextNode = NULL;

  if(Item != NULL)
  {
    NextNode = Item->Next;

    if(Item->Object != NULL)
    {
      free(Item->Object);
    }
    free(Item);
  }

  return NextNode;
}

void SLDeleteNext(SLLIST *Item)
{
  if(Item != NULL && Item->Next != NULL)
  {
    Item->Next = SLDeleteThis(Item->Next);
  }
}

void SLDestroy(SLLIST **List)
{
  SLLIST *Next;
  if(*List != NULL)
  {
    Next = *List;
    do
    {
      Next = SLDeleteThis(Next);
    } while(Next != NULL);
    *List = NULL;
  }
}

int SLWalk(SLLIST *List,
           int(*Func)(int, void *, void *),
           void *Args)
{
  SLLIST *ThisItem;
  int Result = 0;

  for(ThisItem = List;
      0 == Result && ThisItem != NULL;
      ThisItem = ThisItem->Next)
  {
    Result = (*Func)(ThisItem->Tag,
                     ThisItem->Object,
                     Args);
  }

  return Result;
}

/* end of sllist.c */
