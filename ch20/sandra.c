/*  sandra.c - simple byte substitution
 *
 *  SANDRA - Search AND ReplAce bytes
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
#include <ctype.h>

int HexToInt(char *s)
{
  int IntVal = 0;
  char Hex[] = "0123456789ABCDEF";
  char *p = NULL;

  while(*s != '\0')
  {
    /* Caution! Undefined behaviour for some inputs */
    IntVal <<= 4;
    p = strchr(Hex, toupper(*s));
    if(NULL != p)
    {
      IntVal |= (p - Hex);
    }
    else
    {
      printf("Can't convert hex value %s.\n", s);
      exit(EXIT_FAILURE);
    }
    ++s;
  }
  return IntVal;
}

void Sandra(char *filename, int in, int out)
{
  FILE *fp = NULL;
  unsigned long count = 0;
  unsigned char c;

  fp = fopen(filename, "r+b");
  if(NULL == fp)
  {
    printf("Can't open file %s for update.\n", filename);
    exit(EXIT_FAILURE);
  }
  while(fread(&c, 1, 1, fp))
  {
    if(c == (unsigned char)in)
    {
      c = (unsigned char)out;
      fseek(fp, -1L, SEEK_CUR);
      count++;
      if(1 != fwrite(&c, 1, 1, fp))
      {
        puts("Can't write to file.");
        fclose(fp);
        exit(EXIT_FAILURE);
      }
      /* This flush is /essential/ */
      fflush(fp);
    }
  }
  fclose(fp);
  printf("The byte was replaced %lu time%s.\n",
         count,
         count == 1UL ? "" : "s");
}

int main(int argc, char *argv[])
{
  if(argc < 4)
  {
    printf("Usage: %s filename searchbyte replacebyte\n",
           argc > 0 ? argv[0] : "sandra");
  }
  else
  {
    Sandra(argv[1], HexToInt(argv[2]), HexToInt(argv[3]));
  }
  return 0;
}

