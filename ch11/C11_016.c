/*  c11_016.c - demo of dynamically allocated 2D array
 *
 *  C11_016 - dynamic reallocation of arrays
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

#error Don't use this code. It invokes undefined behaviour. See comment.

/* Please check the comment in c11_015.c for a full (ish) description
 * of why you shouldn't use this code.
 *
 * RJH 8/6/2000
 */

#include <stdlib.h>

typedef int T;

T **Reallocate(T **OldP,
               size_t oldm,
               size_t oldn,
               size_t newm,
               size_t newn)
{
  T **NewP = NULL;
  T *p;
  size_t Row;

  /* Do we need more memory? */
  if(newm * newn * sizeof **NewP + newm * sizeof *NewP >
     oldm * oldn * sizeof **OldP + oldm * sizeof *OldP)
  {
    /* Yes, so let's go get some */
    NewP = realloc(OldP,
                   newm * newn * sizeof **NewP +
                   newm * sizeof *NewP);
  }
  else
  {
    NewP = OldP;
  }

  if(NewP != NULL)
  {
    /* Now we have to set up the pointer table again */
    for(Row = 0, p = (T *)NewP + newm;
        Row < newm;
        Row++, p += newn)
    {
      NewP[Row] = p;
    }
  }

  return NewP;
}
