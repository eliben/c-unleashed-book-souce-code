/*  sllist.h - header for single linked list lib
 *
 *  SLLIST - Single-Linked List Library
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


#ifndef SLLIST_H__
#define SLLIST_H__

#define SL_SUCCESS    0
#define SL_NO_MEM     1
#define SL_ZERO_SIZE  2


typedef struct SLLIST
{
  int Tag;
  struct SLLIST *Next;
  void *Object;
  size_t Size;
} SLLIST;

/* Add new item immediately after current item */
int SLAdd(SLLIST **Item,
          int Tag,
          void *Object,
          size_t Size);

/* Add item to front of list. Care: if you pass
 * this function any node other than the first,
 * you will get Y-branches in your list:
 *
 *    oldroot-->-
 *               \  
 *                >->-passeditem-->-->--oldnext
 *               /
 *    newitem-->-
 *
 * This would be a Bad Thing.
 */
int SLFront(SLLIST **Item,
            int Tag,
            void *Object,
            size_t Size);

/* Add new item right at the end of the list */
int SLAppend(SLLIST **Item,
             int Tag,
             void *Object,
             size_t Size);

/* Replace existing data */
int SLUpdate(SLLIST *Item,
             int NewTag,
             void *NewObject,
             size_t NewSize);

/* Retrieve data from this node */
void *SLGetData(SLLIST *Item,
                int *Tag,
                size_t *Size);

/* Delete this item. Returns pointer to
 * next item - caller's responsibility
 * to maintain list integrity.
 */
SLLIST *SLDeleteThis(SLLIST *Item);

/* Delete item immediately following
 * the one passed in. List integrity
 * maintained automatically.
 */
void SLDeleteNext(SLLIST *Item);

/* Destroy the entire list */
void SLDestroy(SLLIST **List);

/* Call func(Tag, ThisItem, Args) for each item */
int SLWalk(SLLIST *List,
           int(*Func)(int, void *, void *),
           void *Args);
#endif
