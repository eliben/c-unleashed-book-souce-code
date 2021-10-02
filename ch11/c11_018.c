/*  c11_018.c - read a text file into memory
 *
 *  C11_018 - dynamic allocation of arrays
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
#include <string.h>

#include "strarr.h"

#define DEFAULT_LINE_LEN        64
#define LINES_PER_ALLOC         16
#define ERR_ROWS_NOT_ADDED      1
#define ERR_STRING_NOT_RESIZED  2
#define ERR_FILE_OPEN_FAILED    3
#define ERR_ALLOC_FAILED        4

int ReadFile(char *Filename,
             char ***Array,
             int *NumRows)
{
  char Buffer[DEFAULT_LINE_LEN] = {0};
  char *NewLine = NULL;
  FILE *fp;
  int Error = 0;
  int Row = 0;
  size_t NumBlocks;

  *NumRows = 0;

  *Array = AllocStrArray(LINES_PER_ALLOC,
                         DEFAULT_LINE_LEN);
  if(NULL != *Array)
  {
    fp = fopen(Filename, "r");
    if(fp != NULL)
    {
      *NumRows = LINES_PER_ALLOC;
      NumBlocks = 1;

      /* fgets will give us no more than sizeof Buffer
       * bytes, including zero terminator and newline
       * if one is present within that number of bytes.
       * Therefore we need to cater for longer lines.
       * To do this, we call fgets again (and again
       * and again) until we encounter a newline.
       */
      while(0 == Error &&
            NULL != fgets(Buffer, sizeof Buffer, fp))
      {
        NewLine = strchr(Buffer, '\n');
        if(NewLine != NULL)
        {
          *NewLine = '\0';
        }
        /* This strcat relies on the AllocStrArray()
         * function initialising rows to empty strings.
         */
        strcat((*Array)[Row], Buffer);
        if(NewLine != NULL)
        {
          /* There was a newline, so the
           * next line is a new one.
           */
          NumBlocks = 1;
          ++Row;
          if(Row >= *NumRows)
          {
            /* Add another LINES_PER_ALLOC lines.
             * If it didn't work, give up.
             */
            if(0 == AddRowsToStrArray(Array,
                                      *NumRows,
                                      LINES_PER_ALLOC,
                                      DEFAULT_LINE_LEN))
            {
              Error = ERR_ROWS_NOT_ADDED;
            }
            else
            {
              *NumRows += LINES_PER_ALLOC;
            }
          }
        }
        else
        {
          ++NumBlocks;
          /* Make room for some more data on this line */
          if(0 ==
             ResizeOneString(*Array,
                             Row,
                             NumBlocks * DEFAULT_LINE_LEN))
          {
            Error = ERR_STRING_NOT_RESIZED;
          }
        }
      }
      fclose(fp);
      if(0 == Error && *NumRows > Row)
      {
        if(0 == AddRowsToStrArray(Array,
                                  *NumRows,
                                  Row - *NumRows,
                                  0))
        {
          Error = ERR_ALLOC_FAILED;
        }
        *NumRows = Row;
      }
    }
    else
    {
      Error = ERR_FILE_OPEN_FAILED; /* Can't open file */
    }
  }
  else
  {
    Error = ERR_ALLOC_FAILED; /* Can't allocate memory */
  }
  if(Error != 0)
  {
    /* If the original allocation failed,
     * *Array will be NULL. FreeStrArray()
     * correctly handles this possibility.
     */
    FreeStrArray(*Array, *NumRows);
    *NumRows = 0;
  }
  else
  {
    ConsolidateStrArray(*Array, *NumRows);
  }

  return Error;
}

int main(int argc, char **argv)
{
  char **array = NULL;

  int rows;
  int thisrow;
  int error;

  if(argc > 1)
  {
    error = ReadFile(argv[1], &array, &rows);
    switch(error)
    {
      case 0:
        for(thisrow = rows - 1; thisrow >= 0; thisrow--)
        {
          printf("%s\n", array[thisrow]);
        }

        FreeStrArray(array, rows);
        break;
      case ERR_STRING_NOT_RESIZED:
      case ERR_ALLOC_FAILED:
      case ERR_ROWS_NOT_ADDED:
        puts("Insufficient memory.");
        break;
      case ERR_FILE_OPEN_FAILED:
        printf("Couldn't open %s for reading\n", argv[1]);
        break;
      default:
        printf("Unknown error! Code %d.\n", error);
        break;
    }
  }
  else
  {
    puts("Please specify the text file name.");
  }

  return 0;
}
/* end of c11_018.c */
