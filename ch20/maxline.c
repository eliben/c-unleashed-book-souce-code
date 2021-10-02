/*  maxline.c - line length violation reporting
 *
 *  MAXLINE - reports bytes per line on long lines
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

#define SWITCH_CHAR '-'

typedef struct ARG
{
  char   FileName[255];
  long   MaxLen;
} ARG;

int GetArgs(int argc, char **argv, ARG *argp)
{
  int ThisArg;
  int CompArg;
  char *EndPtr;

  if(argc <= 2)
  {
    return -1;
  }
  if(argc > 3)
  {
    return -2;
  }
  for(ThisArg = 1, CompArg = 0; ThisArg < argc; ThisArg++)
  {
    switch(CompArg)
    {
      case 0:
        if(strlen(argv[ThisArg]) > 255)
        {
          return ThisArg;
        }
        strcpy(argp->FileName, argv[ThisArg]);
        break;
      case 1:
        argp->MaxLen = strtol(argv[ThisArg], &EndPtr, 10);
        if(EndPtr == argv[ThisArg])
        {
          return ThisArg;
        }
        break;
    }

    ++CompArg;
  }
  return 0;
}


int main(int argc, char **argv)
{
  int Status = EXIT_SUCCESS;
  int ArgResult;
  ARG ArgList = {0};

  ArgResult = GetArgs(argc, argv, &ArgList);
  if(ArgResult != 0)
  {
    Status = EXIT_FAILURE;
  }
  else
  {
    FILE *fp;
    char Line[1024];
    int LineNo = 0;
    size_t LineLen;

    fp = fopen(ArgList.FileName, "r");
    if(fp == NULL)
    {
      printf("couldn't open %s\n", ArgList.FileName);
      Status = EXIT_FAILURE;
    }
    else
    {
      while(fgets(Line, sizeof Line, fp))
      {
        ++LineNo;
        LineLen = strlen(Line) - 1;
        if(LineLen > (size_t)ArgList.MaxLen)
        {
          printf("Line %d (%u chars)\n", LineNo, (unsigned)LineLen);
        }
      }
      fclose(fp);
    }
  }

  return Status;
}
