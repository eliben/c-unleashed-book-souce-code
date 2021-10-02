/*  memtrack.h - header for memory tracker
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
#ifndef MEMTRACK_H__
#define MEMTRACK_H__

#define MEMTRACK_FILENAME "MEMTRACK.TXT"

#ifdef MEMTRACK

#define AllocMemory(size)    \
DebugAllocMemory(size, __FILE__, __LINE__)

#define AllocCopyString(s)        \
DebugAllocCopyString(s, __FILE__, __LINE__)

#define ReAllocMemory(p, newsize) \
DebugReAllocMemory(p, newsize, __FILE__, __LINE__)

#define ReleaseMemory(p)          \
DebugReleaseMemory(p, __FILE__, __LINE__)

#define MEMTRK_MEMALLOC   1
#define MEMTRK_MEMFREE    2
#define MEMTRK_REPORT     3
#define MEMTRK_DESTROY    4

#ifndef TYP_MEMTRK_MSG
#define TYP_MEMTRK_MSG

typedef int MEMTRK_MSG;

#endif

#ifndef TYP_ALIGN
#define TYP_ALIGN

typedef union
{
  long l; /* if C99, consider using intmax_t instead */
  unsigned long lu;
  double f;
  long double lf;
  void *vp;
  void (*fp)(void);
} ALIGN;

#endif

#ifndef TYP_PAYLOAD
#define TYP_PAYLOAD

typedef struct PAYLOAD
{
  void *Ptr;
  size_t Size;
  char *FileName;
  int LineNumber;
} PAYLOAD;

#endif

#define MEM_LEFTCHILD     0
#define MEM_RIGHTCHILD    1
#define MEM_MAX_CHILDREN  2

#ifndef TYP_MEMTREE
#define TYP_MEMTREE

typedef struct MEMTREE
{
  struct MEMTREE *Child[MEM_MAX_CHILDREN];
  unsigned long Key;
  PAYLOAD Payload;
} MEMTREE;

#endif

void *DebugAllocMemory(size_t Size,
                       char *FileName,
                       int LineNumber);

char *DebugAllocCopyString(char *String,
                           char *FileName,
                           int LineNumber);

void *DebugReAllocMemory(void *pOldMem,
                         size_t NewSize,
                         char *FileName,
                         int LineNumber);

void DebugReleaseMemory(void *ppSource,
                        char *FileName,
                        int LineNumber);

int MemPrintAllocs(const PAYLOAD *p1,
                   void *p2);

int TrackMemory(MEMTRK_MSG Msg,
                unsigned long Key,
                void *Ptr,
                int Size,
                char *FileName,
                int LineNumber);


MEMTREE *AddMemNode(MEMTREE **Node,
                    unsigned long Key,
                    PAYLOAD *Payload);

MEMTREE *FindMemNode(MEMTREE *Node,
                     unsigned long Key);

int DeleteMemNode(MEMTREE **Node,
                  unsigned long Key);

int WalkMemTree(MEMTREE *Node,
                int (*Func)(const PAYLOAD *, void *),
                void *Args);

void DestroyMemTree(MEMTREE **Node);

#else
#define AllocMemory     malloc
#define AllocCopyString CopyString
#define ReAllocMemory   realloc
#define ReleaseMemory   free
#define TrackMemory(a, b, c, d, e, f)

#endif

char *CopyString(char *InString);

#endif


