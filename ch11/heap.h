/* HEAP handling

   Copyright (C) 1997, 1998 Free Software Foundation, Inc.
   Written by Ben Pfaff <blp@gnu.org>.
   Modified 1999, with Ben's permission, by Richard
   Heathfield <binary@eton.powernet.co.uk> to handle
   arbitrary data structures. Hacked for 'style'
   too. ;-) (Purely for consistency with existing
   code base.)

   This program is free software; you can redistribute it
   and/or modify it under the terms of the GNU General
   Public License as published by the Free Software
   Foundation; either version 2 of the License, or (at
   your option) any later version.

   This program is distributed in the hope that it will
   be useful, but WITHOUT ANY WARRANTY; without even the
   implied warranty of MERCHANTABILITY or FITNESS FOR A
   PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General
   Public License along with this program; if not, write
   to the Free Software Foundation, Inc., 59 Temple
   Place - Suite 330, Boston, MA 02111-1307, USA. */

#ifndef HEAP_H__
#define HEAP_H__

/* This module implements a priority queue as a heap
 * as described in Knuth 5.2.3.  This is a first-in-
 * smallest-out priority queue.
 */

/* One element of a heap. */
typedef struct HEAP_ELEMENT
{
  int Tag;
  size_t Size;
  void *Object;
} HEAP_ELEMENT;

/* An entire heap. */
typedef struct HEAP
{
  size_t Count;		    /* Number of elements in heap. */
  size_t MaxCount;		/* No. of elems allocated. */
  HEAP_ELEMENT *Heap;	/* Heap elements. */
} HEAP;

typedef int (*HEAP_COMPARE)(const void *Left,
                            int LeftTag,
                            const void *Right,
                            int RightTag);
typedef int (*HEAP_PRINT)(const void *Object,
                          int Tag,
                          size_t Size,
                          FILE *fp);

HEAP *HeapCreate(size_t MaxCount);
void HeapDestroy(HEAP *Heap);
int HeapInsert(HEAP *Heap, 
               int Tag, 
               size_t Size, 
               void *Object, 
               HEAP_COMPARE Comp);
int HeapPeek(int *Tag, size_t *Size);
int HeapDelete(HEAP *Heap,
               int *Tag,
               size_t *Size,
               void *Object,
               HEAP_COMPARE Comp);

int HeapGetSize(HEAP *Heap);

int HeapVerify(HEAP *Heap, HEAP_COMPARE Comp, FILE *fp);
void HeapDump(const HEAP *, HEAP_PRINT Print, FILE *fp);

#endif /* HEAP_H__ */
