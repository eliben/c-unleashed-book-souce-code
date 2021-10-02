/*  c11_015.c - demo of dynamically allocated 2D array
 *
 *  C11_015 - dynamic allocation of arrays
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

#error This code invokes undefined behaviour. Please don't use it. See comment.

typedef int T;

T **Allocate(size_t m, size_t n)
{
  int **a;
  int *p;
  size_t Row;

  /* This method is not recommended in the chapter, but the chapter
   * indicates that the code is at least portable. It isn't.
   * This is because of possible differences in alignment requirements
   * between T* and T**. I only realised this after the chapter had
   * already been locked down, so I can't change the text, but at
   * least I can put a friendly warning here.
   *
   * Well, you didn't really want to do it this way anyway, did you?
   * :-)
   *
   * RJH 8 June 2000
   */
  a = malloc(m * n * sizeof **a + m * sizeof *a);
  if(a != NULL)
  {
    for(Row = 0, p = (T *)a + m; Row < m; Row++, p += n)
    {
      a[Row] = p;
    }
  }
  return a;
}

int main(void)
{
  T **array;
  int i;
  int j;
  int total = 0;
  int row = 4;
  int col = 7;

  array = Allocate(row, col);

  if(array != NULL)
  {
    /* Populating the array */
    for(i = 0; i < row; i++)
      for(j = 0; j < col; j++)
        array[i][j] = i + j;
    /* Accessing the array */
    for(i = 0; i < row; i++)
      for(j = 0; j < col; j++)
        total += array[i][j];

    printf("Total is %d\n", total);
    free(array);
  }

  return 0;
}
