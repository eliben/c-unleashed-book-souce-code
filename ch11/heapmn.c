/*  heapmn.c - test driver for heap library
 *
 *  HEAP - Heap Library
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "heap.h"

typedef struct TASK
{
  char JobName[32];
  int Priority;
} TASK;

int PrintTasks(const void *Object,
               int Tag,
               size_t Size,
               FILE *fp)
{
  const TASK *Task = Object;

  fprintf(fp, "Task: %s\n", Task->JobName);
  fprintf(fp, "Priority: %d\n", Task->Priority);
  fprintf(fp, "Tag: %d\n", Tag);
  fprintf(fp, "Size: %u\n", (unsigned)Size);

  return 0;
}

int CompareTasks(const void *Left,
                 int LTag,
                 const void *Right,
                 int RTag)
{
  const TASK *L = Left;
  const TASK *R = Right;

  int diff = 0;

  if(L->Priority > R->Priority)
  {
    diff = 1;
  }
  else if(L->Priority < R->Priority)
  {
    diff = -1;
  }

  return diff;
}

int main(void)
{
  TASK TaskList[] =
  {
    {"Run for president", 30},
    {"Wash the dog", 20},
    {"Take children to school", 15},
    {"Write a sonnet", 16},
    {"Mow the lawn", 7},
    {"Drink coffee", 6},
    {"Do Usenet", 7},
    {"Read a good book", 17},
    {"Check email", 4},
    {"Buy flowers", 1},
    {"Install new OS", 9},
    {"Pour coffee", 5}
  };

  TASK ThisTask = {0};

  size_t NumTasks = sizeof TaskList / sizeof TaskList[0];
  size_t i;

  HEAP *Heap;
  int BadCount;

  Heap = HeapCreate(8);

  if(NULL != Heap)
  {

    for(i = 0; i < NumTasks; i++)
    {
      HeapInsert(Heap,
                 0,
                 sizeof TaskList[0],
                 TaskList + i,
                 CompareTasks);
    }

    /* Now let's check that the heap really is a heap. */
    printf("Is this a heap?\n");

    BadCount = HeapVerify(Heap, CompareTasks, stdout);
    if(BadCount > 0)
    {
      printf("Number of errors: %d\n", BadCount);
    }
    else
    {
      puts("Good heap.");
    }

    puts("Here's a heap dump.");
    HeapDump(Heap, PrintTasks, stdout);

    while(HeapGetSize(Heap) > 0)
    {
      HeapDelete(Heap,
                 NULL,
                 NULL,
                 &ThisTask,
                 CompareTasks);
      printf("Time to %s\n", ThisTask.JobName);
    }

    HeapDestroy(Heap);
  }
  return 0;
}

