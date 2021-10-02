/*  queue.h - header for queue library
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

#ifndef QUEUE_H__
#define QUEUE_H__

#include "sllist.h"

#define QUEUE_SUCCESS       0
#define QUEUE_ADD_FAILURE   1
#define QUEUE_DEL_FAILURE   2
#define QUEUE_EMPTY         3

typedef struct QUEUE
{
#ifndef NDEBUG
  int CheckInit1;
#endif

  SLLIST *HeadPtr;
  SLLIST *TailPtr;
  size_t NumItems;

#ifndef NDEBUG
  int CheckInit2;
#endif
} QUEUE;

int QueueAdd(QUEUE *Queue,
             int Tag,
             void *Object,
             size_t Size);
int QueueRemove(void *Object, QUEUE *Queue);

void *QueueGetData(QUEUE *Queue, int *Tag, size_t *Size);
size_t QueueCount(QUEUE *Queue);

void QueueDestroy(QUEUE *Queue);

#endif
