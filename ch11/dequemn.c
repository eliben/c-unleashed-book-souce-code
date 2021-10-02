/*  dequemn.c - test driver for deque library
 *
 *  DEQUE - Deque Library
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
#include <time.h>

#include "deque.h"

/* This program simulates a railway transporter which
 * carries cars between cities. At each city, the
 * transporter 'docks' into a bay where cars may be
 * driven up onto it at each end, to speed the loading
 * and unloading process.
 *
 * The trick is to ensure that the cars which are going
 * furthest are loaded first.
 */

#define LNUM_CARS 37
#define RNUM_CARS 63

#define NUM_CITIES 12

typedef struct CAR
{
  char RegNumber[9];
  int Destination;
} CAR;

int Random(int n)
{
  double d;

  d = rand() / (RAND_MAX + 1.0);
  d *= n;

  return (int)d;
}

void RandomiseCar(CAR *c)
{
  const char *CarAlphabet =
    "ABCDEFGHJKLMNPRSTVWXY";

  sprintf(c->RegNumber,
          "%c%d%d%d %c%c%c",
          CarAlphabet[Random(21)],
          Random(9) + 1,
          Random(9) + 1,
          Random(9) + 1,
          CarAlphabet[Random(21)],
          CarAlphabet[Random(21)],
          CarAlphabet[Random(21)]);

  c->Destination = Random(NUM_CITIES);
}

/* A comparison function for qsort, in reverse
 * destination order (largest value first).
 * Note: We don't simply do
 *  return c1->Destination - c2->Destination;
 * because this leads to the possibility of
 * overflow (e.g. if c1->Destination were -10
 * and c2->Destination were INT_MAX).
 * In this case, of course, the data are such that it
 * doesn't really matter. But defensive programming
 * is a good habit to get into and, if we are not
 * careful, easy to get out of!
 */
int CompareCarsByDest(const void *p1, const void *p2)
{
  int Result = 0;

  const CAR *c1 = p1;
  const CAR *c2 = p2;

  if(c1->Destination < c2->Destination)
  {
    Result = 1;
  }
  else if(c1->Destination > c2->Destination)
  {
    Result = -1;
  }

  return Result;
}

int main(void)
{
  DEQUE Transporter = {0};

  char *City[NUM_CITIES] =
  {
    "London",
    "Watford",
    "Luton",
    "Milton Keynes",
    "Northampton",
    "Leicester",
    "Derby",
    "Chesterfield",
    "Sheffield",
    "Leeds",
    "Newcastle",
    "Edinburgh"
  };

  size_t NumCities = sizeof City / sizeof City[0];

  CAR LeftCar[LNUM_CARS] = {0};
  CAR RightCar[RNUM_CARS] = {0};
  CAR *Car;

  int i;

  srand((unsigned)time(NULL));

  for(i = 0; i < LNUM_CARS; i++)
  {
    RandomiseCar(LeftCar + i);
  }
  for(i = 0; i < RNUM_CARS; i++)
  {
    RandomiseCar(RightCar + i);
  }

  qsort(LeftCar,
        LNUM_CARS,
        sizeof LeftCar[0],
        CompareCarsByDest);
  qsort(RightCar,
        RNUM_CARS,
        sizeof RightCar[0],
        CompareCarsByDest);

  puts("Welcome to Dover. The automatic car-loading");
  puts("process is about to begin.\n");

  for(i = 0; i < LNUM_CARS; i++)
  {
    if(DEQUE_SUCCESS != DequeAddAtFront(&Transporter,
                                        0,
                                        LeftCar + i,
                                        sizeof LeftCar[i]))
    {
      puts("Car crash? Insufficient memory.");
      exit(EXIT_FAILURE);
    }
    printf("%s, bound for %s, added at front.\n",
           LeftCar[i].RegNumber,
           City[LeftCar[i].Destination]);

  }

  for(i = 0; i < RNUM_CARS; i++)
  {
    if(DEQUE_SUCCESS != DequeAddAtBack(&Transporter,
                                       0,
                                       RightCar + i,
                                       sizeof RightCar[i]))
    {
      puts("Crunch! Insufficient memory.");
      exit(EXIT_FAILURE);
    }
    printf("%s, bound for %s, added at back.\n",
           RightCar[i].RegNumber,
           City[RightCar[i].Destination]);

  }

  printf("Okay, we're on our way to %s!\n", City[0]);

  for(i = 0;
      DequeCount(&Transporter) > 0 && i < NUM_CITIES;
      i++)
  {
    puts("Deedle-dee-DEE, Deedle-dee-DAH...");
    printf("Okay, we've arrived at %s.\n", City[i]);

    Car = DequeGetDataFromFront(&Transporter, NULL, NULL);

    if(Car == NULL)
    {
      puts("We seem to have run out of cars,");
      puts("so I guess the journey is over.");
      exit(0);
    }
    
    while(Car != NULL &&
          DequeCount(&Transporter) > 0 &&
          Car->Destination == i)
    {
      printf("Unloading %s from front.\n", Car->RegNumber);
      DequeRemoveFromFront(NULL, &Transporter);
      Car = DequeGetDataFromFront(&Transporter, NULL, NULL);
    }

    Car = DequeGetDataFromBack(&Transporter, NULL, NULL);

    while(Car != NULL &&
          DequeCount(&Transporter) > 0 &&
          Car->Destination == i)
    {
      printf("Unloading %s from back.\n", Car->RegNumber);
      DequeRemoveFromBack(NULL, &Transporter);
      Car = DequeGetDataFromBack(&Transporter, NULL, NULL);
    }

    if(i < NUM_CITIES - 1)
    {
      printf("All done, so we're off to %s!\n",
             City[i + 1]);
    }
  }

  printf("That's it - journey's end.\n");

  return 0;
}
