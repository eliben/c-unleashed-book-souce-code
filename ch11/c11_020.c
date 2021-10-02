/*  c11_020.c - do a random walk
 *
 *  C11_020 - illustrate array of pointers to function.
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
#include <time.h>
#include <stdlib.h>

int random(int i)
{
  double d;

  d = rand() / ((double)RAND_MAX + 1.0);
  d *= i;
  return (int)d;
}

int go_left(int *x, int *y)
{
  --*x;

  printf("Going left!\n");

  return random(5);

}

int go_right(int *x, int *y)
{
  ++*x;

  printf("Going right!\n");
  return random(5);

}

int go_down(int *x, int *y)
{
  --*y;

  printf("Going down!\n");

  return random(5);

}

int go_up(int *x, int *y)
{
  ++*y;

  printf("Going up!\n");

  return random(5);

}

int stop(int *x, int *y)
{
  printf("End of the road: (%d, %d)\n", *x, *y);
  
  return -1;
}

int main(void)
{
  int (*action[])(int *, int *) =
  {
    go_left, go_right, go_down, go_up, stop
  };

  int state = 0;
  int x = 0;
  int y = 0;

  srand((unsigned)time(NULL));

  do
  {
    printf("Currently at (%d, %d)\n", x, y);
    state = (*action[state])(&x, &y);
  } while(state != -1);

  return 0;
}

