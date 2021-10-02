/*  dllist.c - Double linked list library source
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

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "dllist.h"


DLLIST *DLCreate(int Tag, void *Object, size_t Size)
{
  DLLIST *NewItem;

  NewItem = malloc(sizeof *NewItem);
  if(NewItem != NULL)
  {
    NewItem->Prev = NewItem->Next = NULL;
    NewItem->Tag = Tag;
    NewItem->Size = Size;
    NewItem->Object = malloc(Size);
    if(NULL != NewItem->Object)
    {
      memcpy(NewItem->Object, Object, Size);
    }
    else
    {
      free(NewItem);
      NewItem = NULL;
    }
  }

  return NewItem;
}

int DLInsertBefore(DLLIST *ExistingItem, DLLIST *NewItem)
{
  int Result = DL_SUCCESS;

  if(ExistingItem != NULL && NewItem != NULL)
  {
    NewItem->Next = ExistingItem;
    NewItem->Prev = ExistingItem->Prev;
    ExistingItem->Prev = NewItem;
    if(NewItem->Prev != NULL)
    {
      NewItem->Prev->Next = NewItem;
    }
  }
  else
  {
    Result = DL_NULL_POINTER;
  }

  return Result;
}

int DLInsertAfter(DLLIST *ExistingItem, DLLIST *NewItem)
{
  int Result = DL_SUCCESS;

  if(ExistingItem != NULL && NewItem != NULL)
  {
    NewItem->Prev = ExistingItem;
    NewItem->Next = ExistingItem->Next;
    ExistingItem->Next = NewItem;
    if(NewItem->Next != NULL)
    {
      NewItem->Next->Prev = NewItem;
    }
  }
  else
  {
    Result = DL_NULL_POINTER;
  }

  return Result;
}

int DLPrepend(DLLIST **Item,
              int Tag,
              void *Object,
              size_t Size)
{
  int Result = DL_SUCCESS;

  DLLIST *p;
  DLLIST *Start;

  assert(Item != NULL);

  p = DLCreate(Tag, Object, Size);

  if(p != NULL)
  {
    if(NULL == *Item)
    {
      *Item = p;
    }
    else
    {
      Start = DLGetFirst(*Item);
      DLInsertBefore(Start, p);
    }
  }
  else
  {
    Result = DL_NO_MEM;
  }

  return Result;
}

int DLAppend(DLLIST **Item,
             int Tag,
             void *Object,
             size_t Size)
{
  int Result = DL_SUCCESS;

  DLLIST *p;
  DLLIST *End;

  assert(Item != NULL);

  p = DLCreate(Tag, Object, Size);

  if(p != NULL)
  {
    if(NULL == *Item)
    {
      *Item = p;
    }
    else
    {
      End = DLGetLast(*Item);
      DLInsertAfter(End, p);
    }
  }
  else
  {
    Result = DL_NO_MEM;
  }

  return Result;
}


/* Add new item immediately after current item */
int DLAddAfter(DLLIST **Item,
               int Tag,
               void *Object,
               size_t Size)
{
  int Result = DL_SUCCESS;
  DLLIST *p;

  assert(Item != NULL);

  p = DLCreate(Tag, Object, Size);

  if(p != NULL)
  {
    if(NULL == *Item)
    {
      *Item = p;
    }
    else
    {
      DLInsertAfter(*Item, p);
    }
  }
  else
  {
    Result = DL_NO_MEM;
  }

  return Result;
}

/* Add new item immediately before current item */
int DLAddBefore(DLLIST **Item,
                int Tag,
                void *Object,
                size_t Size)
{
  int Result = DL_SUCCESS;
  DLLIST *p;

  assert(Item != NULL);

  p = DLCreate(Tag, Object, Size);

  if(p != NULL)
  {
    if(NULL == *Item)
    {
      *Item = p;
    }
    else
    {
      DLInsertBefore(*Item, p);
    }
  }
  else
  {
    Result = DL_NO_MEM;
  }

  return Result;
}

int DLUpdate(DLLIST *Item,
             int NewTag,
             void *NewObject,
             size_t NewSize)
{
  int Result = DL_SUCCESS;

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
      Result = DL_NO_MEM;
    }
  }
  else
  {
    Result = DL_ZERO_SIZE;
  }

  return Result;
}

void *DLGetData(DLLIST *Item,
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

DLLIST *DLExtract(DLLIST *Item)
{
  if(Item != NULL)
  {
    if(Item->Prev != NULL)
    {
      Item->Prev->Next = Item->Next;
    }
    if(Item->Next != NULL)
    {
      Item->Next->Prev = Item->Prev;
    }
    Item->Prev = Item->Next = NULL;
  }
  return Item;
}

void DLDelete(DLLIST *Item)
{
  if(Item != NULL)
  {
    DLExtract(Item);

    if(Item->Object != NULL)
    {
      free(Item->Object);
    }
    free(Item);
  }
}

/* Exchange two items. Both must be non-NULL. */
int DLExchange(DLLIST *ItemA, DLLIST *ItemB)
{
  int Result = DL_SUCCESS;
  DLLIST *t0;
  DLLIST *t1;
  DLLIST *t2;
  DLLIST *t3;

  if(ItemA != NULL && ItemB != NULL)
  {
    if(ItemA->Next == ItemB)
    {
      DLExtract(ItemA);
      DLInsertAfter(ItemB, ItemA);
    }
    else if(ItemB->Next == ItemA)
    {
      DLExtract(ItemB);
      DLInsertAfter(ItemA, ItemB);
    }
    else
    {
      t0 = ItemA->Prev;
      t1 = ItemA->Next;
      t2 = ItemB->Prev;
      t3 = ItemB->Next;

      DLExtract(ItemA);
      DLExtract(ItemB);

      if(t2 != NULL)
      {
        DLInsertAfter(t2, ItemA);
      }
      else
      {
        DLInsertBefore(t3, ItemA);
      }

      if(t0 != NULL)
      {
        DLInsertAfter(t0, ItemB);
      }
      else
      {
        DLInsertBefore(t1, ItemB);
      }
    }
  }
  else
  {
    Result = DL_NULL_POINTER;
  }

  return Result;
}

void DLDestroy(DLLIST **List)
{
  DLLIST *Marker;
  DLLIST *Prev;
  DLLIST *Next;

  if(*List != NULL)
  {
    /* First, destroy all previous items */
    Prev = (*List)->Prev;
    while(Prev != NULL)
    {
      Marker = Prev->Prev;
      DLDelete(Prev);
      Prev = Marker;
    }

    Next = *List;
    do
    {
      Marker = Next->Next;
      DLDelete(Next);
      Next = Marker;
    } while(Next != NULL);
    *List = NULL;
  }
}

DLLIST *DLGetPrev(DLLIST *List)
{
  if(List != NULL)
  {
    List = List->Prev;
  }

  return List;
}

DLLIST *DLGetNext(DLLIST *List)
{
  if(List != NULL)
  {
    List = List->Next;
  }

  return List;
}

DLLIST *DLGetFirst(DLLIST *List)
{
  if(List != NULL)
  {
    while(List->Prev != NULL)
    {
      List = List->Prev;
    }
  }
  return List;
}

DLLIST *DLGetLast(DLLIST *List)
{
  if(List != NULL)
  {
    while(List->Next != NULL)
    {
      List = List->Next;
    }
  }
  return List;
}


DLLIST *DLJoin(DLLIST *Left, DLLIST *Right)
{
  if(Left != NULL && Right != NULL)
  {
    Left = DLGetLast(Left);
    Right = DLGetFirst(Right);

    Left->Next = Right;
    Right->Prev = Left;
  }

  return DLGetFirst(Left);
}

int DLCount(DLLIST *List)
{
  int Items = 0;

  DLLIST *Prev = List;
  DLLIST *Next = List;

  if(List != NULL)
  {
    ++Items;
    while((Prev = DLGetPrev(Prev)) != NULL)
    {
      ++Items;
    }
    while((Next = DLGetNext(Next)) != NULL)
    {
      ++Items;
    }
  }

  return Items;
}

int DLWalk(DLLIST *List,
           int(*Func)(int, void *, void *),
           void *Args)
{
  DLLIST *ThisItem = List;
  int Result = 0;

  if(List != NULL)
  {
    for(ThisItem = DLGetFirst(List);
        0 == Result && ThisItem != NULL;
        ThisItem = ThisItem->Next)
    {
      Result = (*Func)(ThisItem->Tag,
                       ThisItem->Object,
                       Args);
    }
  }
  return Result;
}

/* end of dllist.c */
