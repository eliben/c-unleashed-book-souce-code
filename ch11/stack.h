/*  stack.h - source code for stack library
 *
 *  STACK - Stack Library
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

#ifndef STACK_H__
#define STACK_H__

#include "sllist.h"

#define STACK_SUCCESS       0
#define STACK_PUSH_FAILURE  1
#define STACK_POP_FAILURE   2
#define STACK_EMPTY         3

typedef struct STACK
{
#ifndef NDEBUG
  int CheckInit1;
#endif

  SLLIST *StackPtr;
  size_t NumItems;

#ifndef NDEBUG
  int CheckInit2;
#endif
} STACK;

int StackPush(STACK *Stack,
              int Tag,
              void *Object,
              size_t Size);
int StackPop(void *Object, STACK *Stack);

void *StackGetData(STACK *Stack, int *Tag, size_t *Size);

size_t StackCount(STACK *Stack);

void StackDestroy(STACK *Stack);


#endif
