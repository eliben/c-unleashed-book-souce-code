/*  cfollowd.c - test driver program
 *
 *  CFollow - Code Tracing Library
 *
 *  Copyright (C) 2000  Richard Heathfield
 *                      Eton Computer Systems Ltd
 *                      Macmillan Computer Publishing
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  Richard Heathfield may be contacted at binary@eton.powernet.co.uk
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "cfollow.h"

int CheckFileSystem(void)
{
  char CarryOn[8];
  int KeepGoing = 0;

  printf("Please ensure that your file system allows\n");
  printf("the filename format C%%7d.%%3d\n");
  printf("or modify GetOutputFileName() in cfollow.c\n");
  printf("before continuing.\n");
  printf("To continue, please type GO\n");
  printf("To abort now, please type STOP\n");
  if(fgets(CarryOn, sizeof CarryOn, stdin) != NULL)
  {
    /* Some compilers may warn about the following two
     * lines of code. Clearly, the conversion is
     * legitimate, so I have decided not to cast toupper's
     * int return value into a char. If you (justifiably)
     * prefer a warning-free compilation, please feel free
     * to add the casts yourself. - RJH
     */
    CarryOn[0] = toupper((unsigned char)CarryOn[0]);
    CarryOn[1] = toupper((unsigned char)CarryOn[1]);

    if(strcmp(CarryOn, "GO\n") == 0)
    {
      KeepGoing = 1;
    }
  }

  return KeepGoing;
}

int bar(int baz)
{
  int Result = baz / 2;
  CF_FUNCIN(bar);
  printf("Demonstrating I_FOLLOW\n");
  I_FOLLOW(baz);
  CF_FUNCOUT(bar, d, Result);
  return Result;
}

void foo(void)
{
  CF_FUNCIN(foo);
  printf("foo() doesn't do much. Just calls bar().\n");
  printf("bar() returned %d\n", bar(42));
  CF_FUNCOUT(foo, s, "void");
}

/* The quux of the matter ... */
void quux(void)
{
  unsigned u;
  short h;
  short unsigned hu;
  long l;
  long unsigned lu;
  double d;
  char *p = "quuxstring";

  CF_FUNCIN(quux);

  for(u = 1; u < 3; u++)
  {
    U_FOLLOW(u);

    for(h = 1; h < 3; h++)
    {
      HI_FOLLOW(h);

      for(hu = 1; hu < 3; hu++)
      {
        HU_FOLLOW(hu);
        for(l = 1; l < 3; l ++)
        {
          LI_FOLLOW(l);
          for(lu = 1; lu < 3; lu++)
          {
            LU_FOLLOW(lu);
            for(d = 0.1; d < 0.3; d += 0.1)
            {
              D_FOLLOW(d);
              printf("bignum = %f\n",
                     u + h * hu - l / lu + d);
            }
          }
        }
      }
    }
  }

  P_FOLLOW(p);
  S_FOLLOW(p);

  CF_FUNCOUT(quux, s, "void");
}

void quuux(void)
{
  CF_CODE(int i;)
  CF_CODE(int j;)
  CF_CODE(for(i = 0; i < 10; i++))
  CF_CODE({)
  CF_CODE(  j = i * i;)
  CF_CODE(  printf("%d\n", j);)
  CF_CODE(})

  CF_FUNCIN(quuux);
  CF_FUNCOUT(quuux, s, "void");
}

int TestCFollow(void)
{
  int Result = EXIT_SUCCESS;
  int i;
  char s[] = "Hello world";

  CF_FUNCIN(TestCFollow);

  printf("Calling another function, to demonstrate\n");
  printf("function tracing...\n");

  foo();

  printf("Eine kleine loopmusik\n");

  for(i = 0; i < sizeof s - 1; i++)
  {
    printf("i = %d, string = [%s]\n", i, s + i);
    I_FOLLOW(i);
    S_FOLLOW(s + i);
    F_FOLLOW1("First %d bytes of the string:", i);
    A_FOLLOW(s, i);
  }

  printf("Testing all the pernickety types\n");
  quux();

  printf("Testing trace-only code\n");
  quuux();

  CF_FUNCOUT(TestCFollow, d, Result);

  return Result;
}

int main(void)
{
  int Result;
  int KeepGoing;

  KeepGoing = CheckFileSystem();

  if(0 == KeepGoing)
  {
    Result = EXIT_FAILURE;
  }
  else
  {
    Result = TestCFollow();
    CFOLLOW_CLOSEDOWN;
  }

  return Result;
}

/* end cfollowd.c - the CFollow test driver */
