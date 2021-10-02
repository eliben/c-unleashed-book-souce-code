/*  clist.c - source code for circular list library
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


#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "clist.h"

CL_ITEM *CLCreate(int Tag, void *Object, size_t Size)
{
  CL_ITEM *NewItem = NULL;

  if(Size > 0)
  {
    NewItem = malloc(sizeof *NewItem);
    if(NewItem != NULL)
    {
      NewItem->Prev = NewItem->Next = NULL;
      NewItem->Tag = Tag;
      NewItem->Size = Size;
      NewItem->Object = malloc(Size);
      if(NULL == NewItem->Object)
      {
        free(NewItem);
        NewItem = NULL;
      }
      else
      {
        memcpy(NewItem->Object, Object, Size);
      }
    }
  }

  return NewItem;
}

int CLAddItem(CLIST *List,
              int Tag,
              void *Object,
              size_t Size)
{
  CL_ITEM *NewItem;
  int Result = CL_SUCCESS;

  assert(List != NULL);

  NewItem = CLCreate(Tag, Object, Size);
  if(NULL == NewItem)
  {
    Result = CL_NO_MEM;
  }
  else
  {
    ++List->NumItems;

    if(NULL == List->CurrentItem)
    {
      /* First item in the list, so
       * it points to itself in
       * both directions.
       */
      List->CurrentItem = NewItem;
      List->CurrentItem->Next = NewItem;
      List->CurrentItem->Prev = NewItem;
    }
    else
    {
      /* We'll place this item just
       * behind the current item
       */
      NewItem->Prev = List->CurrentItem->Prev;
      NewItem->Next = List->CurrentItem;
      List->CurrentItem->Prev->Next = NewItem;
      List->CurrentItem->Prev = NewItem;
    }
  }

  return Result;
}

int CLUpdate(CLIST *List,
             int NewTag,
             void *NewObject,
             size_t NewSize)
{
  int Result = CL_SUCCESS;
  void *p;
  
  assert(List != NULL);

  if(NewSize > 0)
  {
    if(List->NumItems > 0)
    {
      p = realloc(List->CurrentItem->Object, NewSize);
      if(NULL != p)
      {
        List->CurrentItem->Object = p;
        memmove(List->CurrentItem->Object, NewObject, NewSize);
        List->CurrentItem->Tag = NewTag;
        List->CurrentItem->Size = NewSize;
      }
      else
      {
        Result = CL_NO_MEM;
      }
    }
    else
    {
      Result = CL_EMPTY;
    }
  }
  else
  {
    Result = CL_ZERO_SIZE;
  }

  return Result;
}

void *CLGetData(CLIST *List,
                int *Tag,
                size_t *Size)
{
  void *p = NULL;

  assert(List != NULL);

  if(List->CurrentItem != NULL)
  {
    if(Tag != NULL)
    {
      *Tag = List->CurrentItem->Tag;
    }
    if(Size != NULL)
    {
      *Size = List->CurrentItem->Size;
    }
    p = List->CurrentItem->Object;
  }

  return p;
}

void CLRotate(CLIST *List, int Places)
{
  int Multiple;
  int i;

  assert(List != NULL);

  if(List->NumItems > 0)
  {
    if(Places < 0)
    {
      Multiple = (List->NumItems - 1 - Places) /
                  List->NumItems;
      Places += Multiple * List->NumItems;
    }

    Places %= List->NumItems;

    if(Places > (int)List->NumItems / 2)
    {
      Places = List->NumItems - Places;
      for(i = 0; i < Places; i++)
      {
        List->CurrentItem = List->CurrentItem->Prev;
      }
    }
    else
    {
      for(i = 0; i < Places; i++)
      {
        List->CurrentItem = List->CurrentItem->Next;
      }
    }
  }
}

int CLDelete(CLIST *List)
{
  int Deleted = 0;
  CL_ITEM *PrevItem;
  CL_ITEM *ThisItem;
  CL_ITEM *NextItem;

  assert(List != NULL);

  if(List->NumItems > 0)
  {
    Deleted = 1;

    ThisItem = List->CurrentItem;
    free(ThisItem->Object);
    NextItem = ThisItem->Next;
    PrevItem = ThisItem->Prev;

    if(1 == List->NumItems)
    {
      List->CurrentItem = NULL;
    }
    else
    {
      List->CurrentItem = NextItem;
      NextItem->Prev = PrevItem;
      PrevItem->Next = NextItem;
    }

    free(ThisItem);

    --List->NumItems;
  }

  return Deleted;
}

void CLDestroy(CLIST *List)
{
  assert(List != NULL);

  while(CLDelete(List))
  {
    continue;
  }
}

int CLWalk(CLIST *List,
           int(*Func)(int, void *, void *),
           void *Args)
{
  CL_ITEM *ThisItem;
  int Result = 0;
  int i;

  assert(List != NULL);

  for(ThisItem = List->CurrentItem, i = 0;
      0 == Result && i < (int)List->NumItems;
      ThisItem = ThisItem->Next, i++)
  {
    Result = (*Func)(ThisItem->Tag,
                     ThisItem->Object,
                     Args);
  }

  return Result;
}

