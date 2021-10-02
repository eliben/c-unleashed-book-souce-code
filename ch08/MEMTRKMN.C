/*  memtrkmn.c - driver for memory tracker
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

#include "memtrack.h"

#define N 128

typedef struct FOO
{
  char str[128];
  double doub[8];
  int i[16];
} FOO;

int main(void)
{
  char   *a[N] = {0};
  int    *b[N] = {0};
  long   *c[N] = {0};
  double *d[N] = {0};
  FOO    *f[N] = {0};

  int i;

  for(i = 0; i < N; i++)
  {
    a[i] = AllocMemory(sizeof *a[i]);
    b[i] = AllocMemory(4 * sizeof *b[i]);
    c[i] = AllocMemory(4 * sizeof *c[i]);
    d[i] = AllocMemory(8 * sizeof *d[i]);
    f[i] = AllocMemory(sizeof *f[i]);
  }

  TrackMemory(MEMTRK_REPORT,
              0, NULL, 0, NULL, 0);

  for(i = 0; i < N; i++)
  {
    ReleaseMemory(a[i]);
    ReleaseMemory(b[i]);
    ReleaseMemory(c[i]);
    ReleaseMemory(d[i]);
    ReleaseMemory(f[i]);
  }

  TrackMemory(MEMTRK_REPORT,
              0, NULL, 0, NULL, 0);

  TrackMemory(MEMTRK_DESTROY,
              0, NULL, 0, NULL, 0);
  return 0;
}
