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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "heap.h"

/* Creates and returns a heap with an initial capacity
 * of MaxCount elements.  Returns non-NULL only if
 * successful.
 */
HEAP *HeapCreate(size_t MaxCount)
{
  HEAP *Heap = malloc(sizeof *Heap);
  if(Heap != NULL)
  {
    Heap->Count = 0;
    Heap->MaxCount = MaxCount;
    Heap->Heap = malloc(Heap->MaxCount *
                        sizeof *Heap->Heap);
    if(Heap->Heap == NULL)
    {
      free(Heap);
      Heap = NULL;
    }
  }
  return Heap;
}

/* Destroy the heap. */
void HeapDestroy(HEAP *Heap)
{
  assert(Heap != NULL);
  free(Heap->Heap);
  free(Heap);
}

/* Insert an object into the heap.
 * Returns nonzero only if successful.
 */
int HeapInsert(HEAP *Heap, 
               int Tag, 
               size_t Size, 
               void *Object, 
               HEAP_COMPARE Comp)
{
  int i, j;

  int Done = 0;
  int Okay = 1;

  void *NewObject = NULL;

  assert (Heap != NULL);

  NewObject = malloc(Size);
  if(NULL == NewObject)
  {
    Okay = 0;
  }
  else
  {
    memcpy(NewObject, Object, Size);
  }

  if(Okay && Heap->Count >= Heap->MaxCount)
  {
    Heap->Heap = realloc(Heap->Heap,
                         2 * Heap->MaxCount *
                             sizeof *Heap->Heap);
    if(Heap->Heap != NULL)
    {
      Heap->MaxCount *= 2;
    }
    else
    {
      Okay = 0;
      free(NewObject);
    }
  }

  if(Okay)
  {
    /* Knuth's Algorithm 5.2.3-16.  Step 1. */
    j = Heap->Count + 1;

    while(!Done)
    {
      /* Step 2. */
      i = j / 2;

      /* Step 3. */
      if (i == 0 || (*Comp)(Heap->Heap[i - 1].Object,
                            Heap->Heap[i - 1].Tag,
                            Object,
                            Tag) <= 0)
      {
        Heap->Heap[j - 1].Tag = Tag;
        Heap->Heap[j - 1].Size = Size;
        Heap->Heap[j - 1].Object = NewObject;
        Heap->Count++;
        Done = 1;
      }
      else
      {
        /* Step 4. */
        Heap->Heap[j - 1] = Heap->Heap[i - 1];
        j = i;
      }
    }
  }

  return Okay;
}

/* Deletes the first element in the heap (the one
 * with the greatest index) and populates Tag, Size
 * and Object (if non-NULL). Note: Object is, if
 * non-NULL, assumed to point to an area of at
 * least Size bytes.
 */
int HeapDelete(HEAP   *Heap,
               int    *pTag,
               size_t *pSize,
               void   *pObject,
               HEAP_COMPARE Comp)
{
  /* Knuth's Algorithm 5.2.3H-19. */
  int l, r, i, j;
  int Done;
  int KeyTag;
  void *KeyObject = NULL;

  void *OldItem;

  
  if (Heap->Count == 0)
    return -1;
  if(pTag != NULL)
  {
    *pTag = Heap->Heap[0].Tag;
  }
  if(pSize != NULL)
  {
    *pSize = Heap->Heap[0].Size;
  }
  if(pObject != NULL)
  {
    memcpy(pObject,
           Heap->Heap[0].Object,
           Heap->Heap[0].Size);
  }

  OldItem = Heap->Heap[0].Object;

  KeyTag = Heap->Heap[Heap->Count - 1].Tag;
  KeyObject = Heap->Heap[Heap->Count - 1].Object;

  l = 1;
  r = Heap->Count - 1;

  j = 1;

  Done = 0;
  while(!Done)
  {
    i = j;
    j *= 2;
    if (j > r)
    {
      Done = 1;
    }
    else
    {
      if (j != r)
      {
        if((*Comp)(Heap->Heap[j - 1].Object,
                   Heap->Heap[j - 1].Tag,
                   Heap->Heap[j    ].Object,
                   Heap->Heap[j    ].Tag) > 0)
        {
          j++;
        }
      }
      if((*Comp)(KeyObject,
                 KeyTag,
                 Heap->Heap[j - 1].Object,
                 Heap->Heap[j - 1].Tag) <= 0)
      {
        Done = 1;
      }
      else
      {
        Heap->Heap[i - 1] = Heap->Heap[j - 1];
      }
    }
  }

  Heap->Heap[i - 1].Object = KeyObject;
  Heap->Heap[i - 1].Tag = KeyTag;

  free(OldItem);

  --Heap->Count;

  return 0;
}

/* Returns the number of elements in the heap. */
int HeapGetSize(HEAP *Heap)
{
  return Heap->Count;
}

int HeapVerify(HEAP *Heap, HEAP_COMPARE Comp, FILE *fp)
{
  size_t j;
  int BadCount = 0;

  for (j = 1; j <= Heap->Count; j++)
  {
    if(j / 2 >= 1 &&
       (*Comp)(Heap->Heap[j / 2 - 1].Object,
               Heap->Heap[j / 2 - 1].Tag,
               Heap->Heap[j     - 1].Object,
               Heap->Heap[j     - 1].Tag) > 0)
    {
      ++BadCount;
      if(fp != NULL)
      {
        fprintf(fp,
                "bad ordering of keys %d and %d\n",
                j / 2 - 1,
                j - 1);
      }
    }
  }

  return BadCount;
}

/* Dumps out the heap to an open stream. */
void HeapDump(const HEAP *Heap,
              HEAP_PRINT Print,
              FILE *fp)
{
  size_t j;

  if(fp != NULL)
  {
    fprintf(fp, "Heap contents:\n");
    for(j = 1; j <= Heap->Count; j++)
    {
      fprintf(fp, "Data %d:\n", j - 1);
      (*Print)(Heap->Heap[j - 1].Object,
               Heap->Heap[j - 1].Tag,
               Heap->Heap[j - 1].Size,
               fp);

      fprintf(fp, "Partner %d:\n", j / 2 - 1);
      if(j / 2 >= 1)
      {
        (*Print)(Heap->Heap[j / 2 - 1].Object,
                 Heap->Heap[j / 2 - 1].Tag,
                 Heap->Heap[j / 2 - 1].Size,
                 fp);
      }
      else
      {
        fprintf(fp, "----\n");
      }
    }
  }
}

