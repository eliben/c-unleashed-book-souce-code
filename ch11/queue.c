/*  queue.c - source code for queue library
 *
 *  QUEUE - Queue Library
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

#include "queue.h"

int QueueAdd(QUEUE *Queue,
             int Tag,
             void *Object,
             size_t Size)
{
  int Result = QUEUE_ADD_FAILURE;
  int ListResult;
  assert(Queue != NULL);
  assert(0 == Queue->CheckInit1 && 0 == Queue->CheckInit2);

  ListResult = SLAdd(&Queue->TailPtr, Tag, Object, Size);

  if(SL_SUCCESS == ListResult)
  {
    if(0 == Queue->NumItems)
    {
      Queue->HeadPtr = Queue->TailPtr;
    }
    else
    {
      Queue->TailPtr = Queue->TailPtr->Next;
    }

    Result = QUEUE_SUCCESS;
    ++Queue->NumItems;
  }

  return Result;
}

int QueueRemove(void *Object, QUEUE *Queue)
{
  size_t Size;
  void *p;
  int Result = QUEUE_SUCCESS;

  assert(Queue != NULL);
  assert(0 == Queue->CheckInit1 && 0 == Queue->CheckInit2);

  if(Queue->NumItems > 0)
  {
    p = SLGetData(Queue->HeadPtr, NULL, &Size);
    if(p != NULL)
    {
      if(Object != NULL)
      {
        memcpy(Object, p, Size);
      }
    }
    else
    {
      Result = QUEUE_DEL_FAILURE;
    }
    Queue->HeadPtr = SLDeleteThis(Queue->HeadPtr);
    --Queue->NumItems;
    if(0 == Queue->NumItems)
    {
      Queue->TailPtr = NULL;
    }
  }
  else
  {
    Result = QUEUE_EMPTY;
  }

  return Result;
}

void *QueueGetData(QUEUE *Queue, int *Tag, size_t *Size)
{
  assert(Queue != NULL);
  assert(0 == Queue->CheckInit1 && 0 == Queue->CheckInit2);

  return SLGetData(Queue->HeadPtr, Tag, Size);
}

size_t QueueCount(QUEUE *Queue)
{
  assert(Queue != NULL);
  assert(0 == Queue->CheckInit1 && 0 == Queue->CheckInit2);
  return Queue->NumItems;
}

void QueueDestroy(QUEUE *Queue)
{
  while(QueueCount(Queue) > 0)
  {
    QueueRemove(NULL, Queue);
  }
}
