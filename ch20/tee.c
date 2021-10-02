/*  tee.c - T-pipe for stdout
 *
 *  TEE - tee
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

void tee(FILE *fp)
{
  char buffer[2048];
  while(fgets(buffer, sizeof buffer, stdin))
  {
    fprintf(stdout, "%s", buffer);
    if(NULL != fp)
    {
      fprintf(fp, "%s", buffer);
    }
  }
}

int main(int argc, char *argv[])
{
  FILE *fp = NULL;
  if(argc > 1)
  {
    fp = fopen(argv[1], "w");
    if(NULL == fp)
    {
      fputs("Error opening output stream.", stderr);
    }
  }
  tee(fp);
  if(NULL != fp)
  {
    fclose(fp);
  }

  return 0;
}

