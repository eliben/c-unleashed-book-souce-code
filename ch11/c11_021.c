/*  c11_021.c - illustrate heterogenous object arrays
 *
 *  C11_021 - heterogenous object arrays
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

typedef struct HETEROBJECT
{
  int tag;
  void *obj;
  int (*func)();
} HETEROBJECT;

enum
{
  INVALID_TAG,
  FOO_TAG,
  BAR_TAG,
  BAZ_TAG
} TAG;

typedef struct FOO
{
  char data;
  char foo[80];
} FOO;

typedef struct BAR
{
  char bar[80];
  long data;
} BAR;

typedef struct BAZ
{
  double data_a;
  char baz[80];
  double data_b;
} BAZ;

void BagInsert(HETEROBJECT *bag,
               size_t Item,
               int Tag,
               void *Address,
               int (*Function)())
{
  bag[Item].tag  = Tag;
  bag[Item].obj  = Address;
  bag[Item].func = Function;
}

int DoFoo(FOO *foo)
{
  printf("%s [%c]\n", foo->foo, foo->data);
  return 0;
}
int DoBar(BAR *bar)
{
  printf("%s [%ld]\n", bar->bar, bar->data);
  return 0;
}
int DoBaz(BAZ *baz)
{
  printf("%s [%f, %f]\n", baz->baz, baz->data_a, baz->data_b);
  return 0;
}

int main(void)
{
  FOO fa = {'a', "I'm the first foo"};
  FOO fb = {'b', "I'm the second foo"};
  BAR ba = {"I'm the first bar", 6 };
  BAR bb = {"I'm the second bar", 42 };
  BAZ za = { 1.414, "I'm the first baz", 1.618 };
  BAZ zb = { 2.718, "I'm the second baz", 3.141 };

  HETEROBJECT Bag[6] = {0};

  int i;

  BagInsert(Bag, 0, BAR_TAG, &bb, DoBar);
  BagInsert(Bag, 1, BAZ_TAG, &za, DoBaz);
  BagInsert(Bag, 2, BAR_TAG, &ba, DoBar);
  BagInsert(Bag, 3, FOO_TAG, &fb, DoFoo);
  BagInsert(Bag, 4, BAZ_TAG, &zb, DoBaz);
  BagInsert(Bag, 5, FOO_TAG, &fa, DoFoo);

  for(i = 0; i < sizeof Bag / sizeof Bag[0]; i++)
  {
    /* Note: some good compilers will issue a warning here,
     * as a function with no prototype is being called
     * (that is, func is a pointer to a function but the
     * parameters of the kind of function to which it
     * points are not specified.
     *
     * This is necessary for the sake of the example, but
     * is a good indication that you should be wary.
     */

    (*Bag[i].func)(Bag[i].obj);
  }

  return 0;
}

