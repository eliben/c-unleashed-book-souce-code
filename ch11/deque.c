/*  deque.c - source code for deque library
 *
 *  DEQUE - Deque Library
 *
 *          Depends on DLLIST
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


#include <string.h>
#include <assert.h>

#include "deque.h"

int DequeAddAtFront(DEQUE *Deque,
                    int Tag,
                    void *Object,
                    size_t Size)
{
  int Result = DEQUE_ADD_FAILURE;
  int ListResult;
  assert(Deque != NULL);
  assert(0 == Deque->CheckInit1 && 0 == Deque->CheckInit2);

  ListResult = DLAddBefore(&Deque->HeadPtr,
                           Tag,
                           Object,
                           Size);

  if(DL_SUCCESS == ListResult)
  {
    if(0 == Deque->NumItems)
    {
      Deque->TailPtr = Deque->HeadPtr;
    }
    else
    {
      Deque->HeadPtr = Deque->HeadPtr->Prev;
    }

    Result = DEQUE_SUCCESS;
    ++Deque->NumItems;
  }

  return Result;
}

int DequeAddAtBack(DEQUE *Deque,
                   int    Tag,
                   void * Object,
                   size_t Size)
{
  int Result = DEQUE_ADD_FAILURE;
  int ListResult;
  assert(Deque != NULL);
  assert(0 == Deque->CheckInit1 && 0 == Deque->CheckInit2);

  ListResult = DLAddAfter(&Deque->TailPtr,
                          Tag,
                          Object,
                          Size);

  if(DL_SUCCESS == ListResult)
  {
    if(0 == Deque->NumItems)
    {
      Deque->HeadPtr = Deque->TailPtr;
    }
    else
    {
      Deque->TailPtr = Deque->TailPtr->Next;
    }

    Result = DEQUE_SUCCESS;
    ++Deque->NumItems;
  }

  return Result;
}

int DequeRemoveFromFront(void *Object, DEQUE *Deque)
{
  size_t Size;
  void *p;
  DLLIST *Temp;
  int Result = DEQUE_SUCCESS;

  assert(Deque != NULL);
  assert(0 == Deque->CheckInit1 && 0 == Deque->CheckInit2);

  if(Deque->NumItems > 0)
  {
    p = DLGetData(Deque->HeadPtr, NULL, &Size);
    if(p != NULL)
    {
      if(Object != NULL)
      {
        memcpy(Object, p, Size);
      }
      Temp = Deque->HeadPtr->Next;
      DLDelete(Deque->HeadPtr);
      Deque->HeadPtr = Temp;

      --Deque->NumItems;
      if(0 == Deque->NumItems)
      {
        Deque->TailPtr = NULL;
      }
    }
    else
    {
      Result = DEQUE_DEL_FAILURE;
    }
  }
  else
  {
    Result = DEQUE_EMPTY;
  }

  return Result;
}

int DequeRemoveFromBack(void *Object, DEQUE *Deque)
{
  size_t Size;
  void *p;
  DLLIST *Temp;
  int Result = DEQUE_SUCCESS;

  assert(Deque != NULL);
  assert(0 == Deque->CheckInit1 && 0 == Deque->CheckInit2);

  if(Deque->NumItems > 0)
  {
    p = DLGetData(Deque->TailPtr, NULL, &Size);
    if(p != NULL)
    {
      if(Object != NULL)
      {
        memcpy(Object, p, Size);
      }
      Temp = Deque->TailPtr->Prev;
      DLDelete(Deque->TailPtr);
      Deque->TailPtr = Temp;

      --Deque->NumItems;
      if(0 == Deque->NumItems)
      {
        Deque->HeadPtr = NULL;
      }
    }
    else
    {
      Result = DEQUE_DEL_FAILURE;
    }
  }
  else
  {
    Result = DEQUE_EMPTY;
  }

  return Result;
}

void *DequeGetDataFromFront(DEQUE *Deque,
                            int *Tag,
                            size_t *Size)
{
  assert(Deque != NULL);
  assert(0 == Deque->CheckInit1 && 0 == Deque->CheckInit2);

  return DLGetData(Deque->HeadPtr, Tag, Size);
}

void *DequeGetDataFromBack(DEQUE *Deque,
                           int *Tag,
                           size_t *Size)
{
  assert(Deque != NULL);
  assert(0 == Deque->CheckInit1 && 0 == Deque->CheckInit2);

  return DLGetData(Deque->TailPtr, Tag, Size);
}

size_t DequeCount(DEQUE *Deque)
{
  assert(Deque != NULL);
  assert(0 == Deque->CheckInit1 && 0 == Deque->CheckInit2);
  return Deque->NumItems;
}

void DequeDestroy(DEQUE *Deque)
{
  assert(Deque != NULL);
  assert(0 == Deque->CheckInit1 && 0 == Deque->CheckInit2);
  DLDestroy(&Deque->HeadPtr);
  Deque->TailPtr = NULL;
}


