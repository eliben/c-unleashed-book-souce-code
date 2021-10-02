/*  stackmn.c - rudimentary syntax checker for
 *              HTML pages. Demonstrates stack library.
 *
 *  STACK - stack library. Depends on SLLIST.
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
#include <stdlib.h>
#include <ctype.h>

#include "stack.h"

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

char *DownString(char *s)
{
  char *t = s;

  while(*s)
  {
    *s = tolower((unsigned char)*s);
    ++s;
  }
  return t;
}

int Exempt(char *HTMLTag)
{
  char *Exceptions[] =
  {
    "p",
    "img",
    "br",
    "hr",
    "dd",
    "dt",
    "li"
  };

  size_t NumEx = sizeof Exceptions / sizeof Exceptions[0];
  size_t ThisEx;
  int IsExempt = 0;

  for(ThisEx = 0; !IsExempt && ThisEx < NumEx; ThisEx++)
  {
    if(0 == strcmp(HTMLTag, Exceptions[ThisEx]))
    {
      IsExempt = 1;
    }
  }

  return IsExempt;
}


int main(int argc, char *argv[])
{
  STACK Stack = {0};
  char Filename[FILENAME_MAX] = {0};
  char *HTMLTag = NULL;
  char *Temp = NULL;
  size_t CurrSize = 0;
  char *p; /* find < */
  char *q; /* find > */

  char **array = NULL; /* HTML file */

  int Count = 0;
  int Error;
  int rows;
  int thisrow;

  int Status = EXIT_SUCCESS;

  if(argc > 1)
  {
    strcpy(Filename, argv[1]);
  }
  else
  {
    printf("HTML syntax checker\n");
    printf("Please type in the name of the file\n");
    printf("to check or EOF (e.g. ^Z in DOS/Win, ^D\n");
    printf("in Unix) to quit: ");

    fflush(stdout);

    if(NULL == fgets(Filename, sizeof Filename, stdin))
    {
      printf("Exiting program.\n");
      Status = EXIT_FAILURE;
    }
    else
    {
      p = strchr(Filename, '\n');
      if(NULL != p)
      {
        *p = '\0';
      }
    }
  }

  if(EXIT_SUCCESS == Status)
  {
    Error = ReadFile(Filename, &array, &rows);

    if(Error != 0)
    {
      printf("Couldn't read file %s.\n", Filename);
      Status = EXIT_FAILURE;
    }
    else
    {
      printf("\nChecking file %s\n\n", Filename);
      for(thisrow = 0;
          EXIT_SUCCESS == Status && thisrow < rows;
          thisrow++)
      {
        p = strchr(array[thisrow], '<');

        while(EXIT_SUCCESS == Status && p != NULL)
        {
          q = strpbrk(p + 1, "\t >\n");

          if(NULL == q)
          {
            printf("Syntax error, line %d"
                   " (no tag closure).\n", thisrow);
            ++Count;
          }
          else
          {
            if(q - p > (int)CurrSize)
            {
              CurrSize = q - p;
              Temp = realloc(HTMLTag, CurrSize);
              if(Temp != NULL)
              {
                HTMLTag = Temp;
              }
              else
              {
                printf("Memory loss.\n");
                Status = EXIT_FAILURE;
              }
            }
            memcpy(HTMLTag, p + 1, q - 1 - p);
            HTMLTag[q - 1 - p] = '\0';
            DownString(HTMLTag);
          }

          if(p[1] != '/')
          {
            if(!Exempt(HTMLTag))
            {
              if(STACK_SUCCESS != StackPush(&Stack,
                                            0,
                                            HTMLTag,
                                            q - p))
              {
                printf("Stack failure: %s on line %d\n",
                        HTMLTag,
                        thisrow);
                Status = EXIT_FAILURE;
              }
            }
          }
          else
          {
            Temp = StackGetData(&Stack,
                                NULL,
                                NULL);

            if(0 != strcmp(HTMLTag + 1, Temp))
            {
              printf("%s closure expected,"
                     " %s closure found on line %d.\n",
                     Temp,
                     HTMLTag,
                     thisrow);
              ++Count;
            }

            StackPop(NULL, &Stack);
          }
          p = strchr(p + 1, '<');
        }
      }

      printf("%d syntax error%s found.\n",
             Count,
             Count == 1 ? "" : "s");

      FreeStrArray(array, rows);
    }
  }

  StackDestroy(&Stack);

  return Status;
}

