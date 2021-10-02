/*  memtrack.c - source code for memory tracker
 *
 *  MEMTRACK - Memory Tracking Library
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
#include <time.h>

#include <assert.h>

#include "memtrack.h"

/* portable implementation of strdup().
 * Allocates sufficient memory to make a copy
 * of a string, and then copies that string.
 * Returns a pointer to the new string, or
 * NULL (when there is insufficient memory or
 * when the parameter is NULL).
 */
char *CopyString(char *InString)
{
  char *p = NULL;

  if(InString != NULL)
  {
    p = malloc(strlen(InString) + 1);
    if(NULL != p)
    {
      strcpy(p, InString);
    }
  }

  return p;
}

#ifdef MEMTRACK

/* Tracking versions */

void *DebugAllocMemory(size_t Size,
                       char *FileName,
                       int LineNumber)
{
  void *ptr;
  char *p;

  ptr = malloc(Size + sizeof(ALIGN));
  if(ptr != NULL)
  {
    TrackMemory(MEMTRK_MEMALLOC,
                0,
                ptr,
                Size,
                FileName,
                LineNumber);

    /* To disguise our little hideyhole, we need to report
     * an address sizeof(ALIGN) bytes higher than the one
     * returned by malloc. But we can't do pointer
     * arithmetic on void pointers. So we use a temporary
     * char *.
     */
    p = ptr;
    p += sizeof(ALIGN);
    ptr = p;
  }

  return ptr;
}

char *DebugAllocCopyString(char *String,
                           char *FileName,
                           int   LineNumber)
{
  char *p = NULL;
  int ErrorStatus = 0;
  size_t Length = strlen(String) + 1;

  p = malloc(Length + sizeof(ALIGN));
  if(0 == ErrorStatus)
  {
    strcpy(p + sizeof(ALIGN), String);
    TrackMemory(MEMTRK_MEMALLOC,
                0,
                p,
                Length,
                FileName,
                LineNumber);

    p += sizeof(ALIGN);
  }
  return p;
}


void *DebugReAllocMemory(void *pOldMem,
                         size_t NewSize,
                         char *FileName,
                         int LineNumber)
{
  void *NewPtr = NULL;
  int ItWasntNull = 0;
  char *p;

  ALIGN KeyStore;

  if(pOldMem != NULL)
  {
    ItWasntNull = 1;

    p = pOldMem;

    p -= sizeof(ALIGN);

    memcpy(&KeyStore.lu, p, sizeof(ALIGN));
  }

  NewPtr = realloc(pOldMem,
                   NewSize +
                   (NewSize > 0  ?
                   sizeof(ALIGN) :
                   0));

  if(NULL != NewPtr)
  {
    if(ItWasntNull)
    {
      TrackMemory(MEMTRK_MEMFREE,
                  KeyStore.lu,
                  NULL,
                  0,
                  FileName,
                  LineNumber);
    }

    if(NewSize > 0)
    {
      TrackMemory(MEMTRK_MEMALLOC,
                  0,
                  NewPtr,
                  NewSize,
                  FileName,
                  LineNumber);

      p = NewPtr;
      p += sizeof(ALIGN);
      NewPtr = p;
    }
  }

  return NewPtr;
}

void DebugReleaseMemory(void *pSource,
                        char *FileName,
                        int LineNumber)
{
  char *p;
  ALIGN KeyStore;

  if(pSource != NULL)
  {
    p = pSource;
    p -= sizeof(ALIGN);
    memcpy(&KeyStore.lu, p, sizeof(ALIGN));

    TrackMemory(MEMTRK_MEMFREE,
                KeyStore.lu,
                NULL,
                0,
                FileName,
                LineNumber);

    free(p);
  }
}

/* Our first hash just gives us a /relatively/
 * well-balanced tree. It isn't going to
 * give us much uniqueness.
 */
unsigned long hash1(unsigned long value)
{                  /* N1         P1      K1 */
  return ((value * 179424601UL + 71UL) % 167UL);
}

/* This second hash gives us a guarantee of uniqueness
 * over a relatively large number of keys -
 * 2147483647 keys, in fact. If this isn't enough for
 * you, consider using a C99 compiler, specifying K1
 * as long long, and setting it to some prime number
 * about halfway between 2^63 and 2^64.
 */
unsigned long hash2(unsigned long value)
{                  /* N1         P1       K1 */
  return ((value * 179424673UL + 257UL) % 2147483647UL);
}

int MemTrkCmp(unsigned long key1,
              unsigned long key2)
{
  int diff = 0;

  unsigned long hv1, hv2;

  hv1 = hash1(key1);
  hv2 = hash1(key2);

  if(hv1 > hv2)
  {
    diff = 1;
  }
  else if(hv1 < hv2)
  {
    diff = -1;
  }
  else
  {
    hv1 = hash2(key1);
    hv2 = hash2(key2);
    if(hv1 > hv2)
    {
      diff = 1;
    }
    else if(hv1 < hv2)
    {
      diff = -1;
    }
    else
    {
      assert(key1 == key2);
    }
  }

  return diff;
}

int MemPrintAllocs(const PAYLOAD *p1, void *p2)
{
  FILE *fp = p2;

  fprintf(fp,
          "\n%8p allocated %7u byt%s "
          "at Line %5d of File %s.",
          p1->Ptr,
          (unsigned int)p1->Size,
          p1->Size == 1 ? "e " : "es",
          p1->LineNumber,
          p1->FileName);

  return 0;
}

/* If we are allocating, we use Ptr. If we are
 * freeing, we should be given a key by the
 * calling Debugxxx memory allocation function.
 */
int TrackMemory(MEMTRK_MSG    Msg,
                unsigned long Key,
                void *        Ptr,
                int           Size,
                char *        FileName,
                int           LineNumber)
{
  int ErrorStatus = 0;
  static FILE *fp = NULL;
  static unsigned long MemTrackIdx = 0;

  PAYLOAD EntryBuilder = {0};
  MEMTREE *NodePtr = NULL;
  unsigned long ThisKey = 0;
  PAYLOAD *EntryFinder = NULL;
  ALIGN KeyStore = {0};

  static MEMTREE *MemTree = NULL;
  static int IveBeenInitialised = 0;
  static unsigned long MaxAlloc = 0;
  static unsigned long CurrAlloc = 0;

  time_t tt = {0};
  struct tm *tmt = NULL;

  if(!IveBeenInitialised)
  {
    fp = fopen(MEMTRACK_FILENAME, "w");
    if(NULL == fp)
    {
      fprintf(stderr,
              "Can't create file %s\n",
              MEMTRACK_FILENAME);
      fprintf(stderr,
              "Using stdout instead.\n");
      fp = stdout;
    }
    IveBeenInitialised = 1;
  }

  EntryBuilder.FileName = FileName;
  EntryBuilder.LineNumber = LineNumber;

  switch(Msg)
  {
    case MEMTRK_MEMALLOC:
      EntryBuilder.Ptr = (char *)Ptr + sizeof(ALIGN);
      EntryBuilder.Size = Size;
      ThisKey = MemTrackIdx++;
      KeyStore.lu = ThisKey;
      memcpy(Ptr, &KeyStore, sizeof KeyStore);

      if(NULL == AddMemNode(&MemTree,
                            ThisKey,
                            &EntryBuilder))
      {
        fprintf(fp,
                "ERROR in debugging code - "
                "failed to add node to memory tree.\n");
        fflush(fp);
      }
      else
      {
        CurrAlloc += Size;
        if(CurrAlloc > MaxAlloc)
        {
          MaxAlloc = CurrAlloc;
        }
      }

      break;
    case MEMTRK_MEMFREE:

      NodePtr = FindMemNode(MemTree, Key);
      if(NULL != NodePtr)
      {
        EntryFinder = &NodePtr->Payload;
        CurrAlloc -= EntryFinder->Size;
        if(CurrAlloc < 0)
        {
          fprintf(fp,
                  "ERROR: More memory released "
                  "than allocated!\n");
          fflush(fp);
        }

        DeleteMemNode(&MemTree, Key);
      }
      else
      {
        /* Tried to free an entry
         * that was never allocated.
         */
        fprintf(fp,
                "Attempted to free unallocated "
                "block %p at Line %d of File %s.\n",
                EntryBuilder.Ptr,
                EntryBuilder.LineNumber,
                EntryBuilder.FileName);
        fflush(fp);
      }

      break;

    case MEMTRK_REPORT:

      fprintf(fp,
              "\nMemory Tracker Report\n");
      fprintf(fp,
              "---------------------\n\n");

      tt = time(NULL);
      tmt = localtime(&tt);
      if(tmt != NULL)
      {
        char timebuffer[64] = {0};

        strftime(timebuffer,
                 sizeof timebuffer,
                 "%H:%M:%S %Z on %A %d %B %Y",
                 tmt);
        fprintf(fp, "\n%s\n\n", timebuffer);
      }
              
      fprintf(fp,
              "Current Allocation: %lu byt%s.\n",
              CurrAlloc,
              CurrAlloc == 1 ? "e" : "es");

      fprintf(fp,
              "Maximum Allocation: %lu byt%s.\n",
              MaxAlloc,
              MaxAlloc == 1 ? "e" : "es");

      fprintf(fp,
              "Nodes currently allocated:\n\n");

      WalkMemTree(MemTree, MemPrintAllocs, fp);
      if(CurrAlloc == 0)
      {
        fprintf(fp, "None! (Well done!)");
      }
      fprintf(fp, "\n");
      fflush(fp);
      break;
    case MEMTRK_DESTROY:
      DestroyMemTree(&MemTree);
      if(ferror(fp))
      {
        fprintf(stderr, "Error writing to log file.\n");
      }
      fclose(fp);
      break;
    default:
      break;
  }

  return ErrorStatus;
}

/* Binary search tree functions
 *
 * Many thanks to Ben Pfaff, for the help he gave me on
 * node deletions - his name will be forever etched into
 * the bark of any tree you care to grow using
 * this source. If you want /real/ trees, though, have a
 * look at Ben's chapter on AVL and red-black trees.
 *
 * I'd also like to thank Chad Dixon for the all-night
 * debugging session he suffered in a (successful!)
 * attempt to get this code to work.
 *
 */

MEMTREE *AddMemNode(MEMTREE **Node,
                    unsigned long Key,
                    PAYLOAD *Payload)
{
  int Diff = 0;
  int ChildIdx = 0;
  MEMTREE *Twig = NULL;

  assert(Node != NULL);
  assert(Payload != NULL);

  if(*Node == NULL)
  {
    *Node = malloc(sizeof **Node);
    if(*Node != NULL)
    {
      Twig = *Node;
      Twig->Child[MEM_LEFTCHILD] = NULL;
      Twig->Child[MEM_RIGHTCHILD] = NULL;
      Twig->Key = Key;
      Twig->Payload = *Payload;
    }
  }
  else
  {
    Twig = *Node;

    Diff = MemTrkCmp(Key, Twig->Key);

    if(Diff == 0)
    {
      /* Duplicate Key - error. */
      assert(0);
    }
    else
    {
      ChildIdx = (Diff > 0);

      Twig = AddMemNode(&Twig->Child[ChildIdx],
                        Key,
                        Payload);
    }
  }

  return Twig;
}

MEMTREE *FindMemNode(MEMTREE *Node, unsigned long Key)
{
  int Diff = 0;
  int ChildIdx = 0;

  if(Node != NULL)
  {
    Diff = MemTrkCmp(Key, Node->Key);

    if(0 != Diff)
    {
      ChildIdx = (Diff > 0);
      Node = FindMemNode(Node->Child[ChildIdx], Key);
    }
  }

  return Node;
}

int WalkMemTree(MEMTREE *Node,
                int (*Function)(const PAYLOAD *, void *),
                void *Args)
{
  int error = 0;

  if(Node != NULL)
  {
    error = WalkMemTree(Node->Child[MEM_LEFTCHILD],
                        Function,
                        Args);
    if(error == 0)
    {
      error = (*Function)(&Node->Payload, Args);
    }
    if(error == 0)
    {
      error = WalkMemTree(Node->Child[MEM_RIGHTCHILD],
                          Function,
                          Args);
    }
  }

  return error;
}


MEMTREE *FindSmallest(MEMTREE *Node)
{
 if (Node == NULL)
   return NULL;
 else if(Node->Child[MEM_LEFTCHILD] == NULL)
   return Node;
 else
   return FindSmallest(Node->Child[MEM_LEFTCHILD]);
}




int DeleteMemNode(MEMTREE **Node, unsigned long Key)
{
  int Deleted = 0;
  int Diff = 0;
  MEMTREE *Successor = NULL;
  MEMTREE *NewTop = NULL;
  MEMTREE *TopNode = NULL;

  if(*Node != NULL)
  {
    if((*Node)->Key == Key)
    {
      /* We need to delete the top node. */

      Deleted = 1;
      TopNode = *Node;

      if(TopNode->Child[MEM_LEFTCHILD] == NULL)
      {
        if(TopNode->Child[MEM_RIGHTCHILD] == NULL)
        {
          /* Last entry in tree. Just delete it */
          free(TopNode);
          *Node = NULL;
        }
        else
        {
          /* Just one child, a right child */
          *Node = TopNode->Child[MEM_RIGHTCHILD];
          free(TopNode);
        }
      }
      else
      {
        if(TopNode->Child[MEM_RIGHTCHILD] == NULL)
        {
          /* Just one child, a left child */
          *Node = TopNode->Child[MEM_LEFTCHILD];
          free(TopNode);
        }
        else
        {
          PAYLOAD Payload;
          unsigned long LocalKey;

          /* Two children. This is the awkward case. */
          Successor =
            FindSmallest(TopNode->Child[MEM_RIGHTCHILD]);
          Payload = Successor->Payload;
          LocalKey = Successor->Key;

          DeleteMemNode(Node, LocalKey);

          memcpy(&TopNode->Payload,
                 &Payload,
                 sizeof Payload);

          TopNode->Key = LocalKey;
        }
      }
    }
    else
    {
      Diff = (MemTrkCmp(Key, (*Node)->Key) > 0);
      Deleted = DeleteMemNode(&(*Node)->Child[Diff], Key);
    }
  }

  return Deleted;
}

void DestroyMemTree(MEMTREE **Node)
{
  int i;
  if(Node != NULL)
  {
    if(*Node != NULL)
    {
      for(i = 0; i < 2; i++)
      {
        DestroyMemTree(&(*Node)->Child[i]);
      }
      free(*Node);
      *Node = NULL;
    }
  }
}

#endif
