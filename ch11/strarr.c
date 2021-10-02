/*  strarr.c - resizeable string array
 *
 *  STRARR - dynamic allocation of arrays
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

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "strarr.h"

void FreeStrArray(char **Array, size_t NumRows)
{
  size_t Row;

  if(Array != NULL)
  {
    for(Row = 0; Row < NumRows; Row++)
    {
      if(Array[Row] != NULL)
      {
        free(Array[Row]);
      }
    }
    free(Array);
  }
}

char **AllocStrArray(size_t NumRows, size_t Width)
{
  char **Array = NULL;
  size_t Row;
  int Success = 1;

  /* allocating 0 bytes is not a great idea, and
   * represents a logic error.
   */
  assert(NumRows > 0);
  assert(Width > 0);

  /* Just in case the zero allocation is NOT caught
   * in testing, we'll check for it here.
   */
  if(NumRows > 0 && Width > 0)
  {
    Array = malloc(NumRows * sizeof *Array);
    if(Array != NULL)
    {
      for(Row = 0; Row < NumRows; Row++)
      {
        Array[Row] = malloc(Width * sizeof *Array[Row]);
        if(NULL == Array[Row])
        {
          Success = 0;
        }
        else
        {
          /* Making this into an empty string is a quick
           * op which will almost invariably be The Right
           * Thing and can never be The Wrong Thing, so
           * we might as well do it.
           */
          Array[Row][0] = '\0';
        }
      }
      /* If any inner allocation failed,
       * we should clean up.
       */
      if(1 != Success)
      {
        FreeStrArray(Array, NumRows);
        Array = NULL;
      }
    }
  }

  return Array;
}

int ResizeOneString(char **Array,
                    size_t Row,
                    size_t NewSize)
{
  char *p;
  int Success = 1;

  assert(Array != NULL);

  p = realloc(Array[Row], NewSize);
  if(p != NULL)
  {
    Array[Row] = p;
  }
  else
  {
    Success = 0;
  }

  return Success;
}

int AddRowsToStrArray(char ***ArrayPtr,
                      size_t OldNumRows,
                      int NumRowsToAdd,
                      size_t InitWidth)
{
  char **p;
  int Success = 1;
  int Row;
  int OldRows;

  OldRows = (int)OldNumRows;
  if(NumRowsToAdd < 0)
  {
    for(Row = OldRows - 1;
        Row >= OldRows + NumRowsToAdd;
        Row--)
    {
      free((*ArrayPtr)[Row]);
    }
  }

  p = realloc(*ArrayPtr,
              (OldRows + NumRowsToAdd) *
               sizeof(**ArrayPtr));

  if(p != NULL)
  {
    *ArrayPtr = p;

    for(Row = OldRows;
        Success && Row < OldRows + NumRowsToAdd;
        Row++)
    {
      (*ArrayPtr)[Row] = malloc(InitWidth);
      if((*ArrayPtr)[Row] != NULL)
      {
        (*ArrayPtr)[Row][0] = '\0';
      }
      else
      {
        Success = 0;
      }
    }
  }
  else
  {
    Success = 0;
  }
  return Success;
}

int ConsolidateStrArray(char **ArrayPtr,
                        size_t NumRows)
{
  size_t Row;
  size_t Len;
  int NumFailures = 0;

  for(Row = 0; Row < NumRows; Row++)
  {
    /* If the library has been correctly used, no
     * row pointer will ever be NULL, so we should
     * assert that this is the case.
     */
    assert(ArrayPtr[Row] != NULL);
    Len = 1 + strlen(ArrayPtr[Row]);
    if(0 == ResizeOneString(ArrayPtr, Row, Len))
    {
      ++NumFailures;
    }
  }
  return NumFailures;
}

/* end of strarr.c */
