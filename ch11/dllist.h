/*  dllist.h
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

#ifndef DLLIST_H__
#define DLLIST_H__

#define DL_SUCCESS      0
#define DL_NO_MEM       1
#define DL_ZERO_SIZE    2
#define DL_NULL_POINTER 3

typedef struct DLLIST
{
  int Tag;
  struct DLLIST *Prev;
  struct DLLIST *Next;
  void *Object;
  size_t Size;
} DLLIST;

DLLIST *DLCreate(int Tag, void *Object, size_t Size);

/* Insert existing item into list, just
 * before the item provided.
 */
int DLInsertBefore(DLLIST *ExistingItem, DLLIST *NewItem);

/* Insert existing item into list, just
 * after the item provided.
 */
int DLInsertAfter(DLLIST *ExistingItem, DLLIST *NewItem);

/* Add item at start of list */
int DLPrepend(DLLIST **Item,
              int Tag,
              void *Object,
              size_t Size);

/* Add item at end of list */
int DLAppend(DLLIST **Item,
             int Tag,
             void *Object,
             size_t Size);

/* Add new item immediately after current item */
int DLAddAfter(DLLIST **Item,
               int Tag,
               void *Object,
               size_t Size);

/* Add new item immediately before current item */
int DLAddBefore(DLLIST **Item,
                int Tag,
                void *Object,
                size_t Size);

/* Update one item */
int DLUpdate(DLLIST *Item,
             int NewTag,
             void *NewObject,
             size_t NewSize);

/* Get a pointer to the data */
void *DLGetData(DLLIST *Item,
                int *Tag,
                size_t *Size);

/* Extract one item from the
 * list, without destroying it.
 */
DLLIST *DLExtract(DLLIST *Item);

/* Delete one item in the list */
void DLDelete(DLLIST *Item);

/* Exchange two items. Both must be non-NULL. */
int DLExchange(DLLIST *ItemA, DLLIST *ItemB);

/* Destroy the entire list */
void DLDestroy(DLLIST **List);

/* List navigation functions */
DLLIST *DLGetPrev(DLLIST *List);
DLLIST *DLGetNext(DLLIST *List);
DLLIST *DLGetFirst(DLLIST *List);
DLLIST *DLGetLast(DLLIST *List);

DLLIST *DLJoin(DLLIST *Left, DLLIST *Right);
int DLCount(DLLIST *List);

/* Walk the list */
int DLWalk(DLLIST *List,
           int(*Func)(int, void *, void *),
           void *Args);

#endif
