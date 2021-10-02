/*  c11_017.c - demo of dynamically allocated 2D array
 *
 *  C11_017 - dynamic allocation of arrays
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

typedef int T;

void Release(T **a, size_t m)
{
  size_t Row;

  for(Row = 0; Row < m; Row++)
  {
    if(a[Row] != NULL)
    {
      free(a[Row]);
    }
  }
  free(a);
}



T **Allocate(size_t m, size_t n)
{
  T **a;
  size_t Row;
  int Success = 1;

  a = malloc(m * sizeof *a);
  if(a != NULL)
  {
    for(Row = 0; Row < m; Row++)
    {
      a[Row] = malloc(n * sizeof *a[Row]);
      if(NULL == a[Row])
      {
        Success = 0;
      }
    }
    /* If any inner allocation failed,
     * we should clean up
     */
    if(1 != Success)
    {
      Release(a, m);
      a = NULL;
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
    Release(array, row);
  }

  return 0;
}
