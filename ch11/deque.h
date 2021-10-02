/*  deque.h - header for deque library
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

#ifndef DEQUE_H__
#define DEQUE_H__

#include "dllist.h"

#define DEQUE_SUCCESS       0
#define DEQUE_ADD_FAILURE   1
#define DEQUE_DEL_FAILURE   2
#define DEQUE_EMPTY         3

typedef struct DEQUE
{
#ifndef NDEBUG
  int CheckInit1;
#endif

  DLLIST *HeadPtr;
  DLLIST *TailPtr;
  size_t NumItems;

#ifndef NDEBUG
  int CheckInit2;
#endif
} DEQUE;

int DequeAddAtFront(DEQUE *Deque,
                    int Tag,
                    void *Object,
                    size_t Size);
int DequeAddAtBack(DEQUE *Deque,
                   int Tag,
                   void *Object,
                   size_t Size);
int DequeRemoveFromFront(void *Object, DEQUE *Deque);
int DequeRemoveFromBack(void *Object, DEQUE *Deque);

void *DequeGetDataFromFront(DEQUE *Deque,
                            int *Tag,
                            size_t *Size);
void *DequeGetDataFromBack(DEQUE *Deque,
                           int *Tag,
                           size_t *Size);
size_t DequeCount(DEQUE *Deque);

#endif
