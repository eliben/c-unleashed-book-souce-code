/*  dllistmn.c - Test driver for double linked list lib
 *
 *  DLLIST - Double-Linked List Library
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
#include <time.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>

#include "dllist.h"

#define PI 3.14159265358979323846

typedef struct CITY
{
  char Nation[30];
  char CityName[25];
  double Latitude;
  double Longitude;
} CITY;

int CompCities(const void *p1, const void *p2)
{
  const CITY *c1 = p1, *c2 = p2;

  return strcmp(c1->CityName, c2->CityName);
}

/* This function returns 1 on success,
 * or 0 on error. If an error is detected,
 * 0 is returned immediately, to simplify
 * the code.
 */
int ParseCity(CITY *c, char *Buffer)
{
  char *Token;
  char *endp;

  assert(c != NULL);

  Token = strtok(Buffer, ",\n");

  if(Token == NULL)
    return 0;

  strcpy(c->Nation, Token);
  
  Token = strtok(NULL, ",\n");
  if(Token == NULL)
    return 0;

  strcpy(c->CityName, Token);

  Token = strtok(NULL, ",\n");
  if(Token == NULL)
    return 0;

  c->Latitude = strtod(Token, &endp);

  Token = strtok(NULL, ",\n");
  if(Token == NULL)
    return 0;

  c->Latitude += strtod(Token, &endp) / 60.0;

  Token = strtok(NULL, ",\n");
  if(Token == NULL)
    return 0;

  if('S' == toupper(*Token))
  {
    c->Latitude *= -1.0;
  }

  c->Latitude *= PI;
  c->Latitude /= 180.0;

  Token = strtok(NULL, ",\n");
  if(Token == NULL)
    return 0;

  c->Longitude = strtod(Token, &endp);

  Token = strtok(NULL, ",\n");
  if(Token == NULL)
    return 0;

  c->Longitude += strtod(Token, &endp) / 60.0;

  Token = strtok(NULL, ",\n");
  if(Token == NULL)
    return 0;

  if('E' == toupper(*Token))
  {
    c->Longitude *= -1.0;
  }

  c->Longitude *= PI;
  c->Longitude /= 180.0;

  return 1;
}

/* Calculate the distance between two points on the earth's
 * surface (accurate to within around +/- 30 miles).
 *
 * Strictly speaking, this is nothing to do with data
 * structures, classical or otherwise. Never mind -
 * consider it a free gift. :-)
 */
int CalcGreatCircleDistance(CITY *City1, CITY *City2)
{
  return (int)(3956.934132687       *
    acos((sin(City1->Latitude)      *
          sin(City2->Latitude))     +
        ((cos(City1->Latitude)      *
          cos(City2->Latitude))     *
        ((cos(City1->Longitude)     *
          cos(City2->Longitude))    + 
         (sin(City1->Longitude)     *
          sin(City2->Longitude))))));
}

int Random(int n)
{
  double d;

  d = rand() / (RAND_MAX + 1.0);
  d *= n;

  return (int)d;
}

/* The test file cityloc.txt may be passed as argv[1]. */
int main(int argc, char *argv[])
{
  DLLIST *List = NULL;
  DLLIST *Safe = NULL;
  CITY *City = NULL;
  CITY ThisCity = {0};
  CITY *TempCity = NULL;
  CITY *First;
  CITY *Second;
  long TotalDistance;
  int Distance;

  int i;
  int j;
  int k;

  int NumCities = 0;
  int MaxCities = 0;

  char Buffer[80] = {0};

  FILE *fp = NULL;

  srand((unsigned)time(NULL));
  if(argc > 1)
  {
    fp = fopen(argv[1], "r");

    if(NULL == fp)
    {
      printf("Can't open %s for reading.\n", argv[1]);
      exit(EXIT_FAILURE);
    }
  }
  else
  {
    puts("Please specify a cities file.");
    exit(EXIT_FAILURE);
  }

  while(NULL != fgets(Buffer,
                      sizeof Buffer,
                      fp))
  {
    if(ParseCity(&ThisCity, Buffer))
    {
      if(++NumCities >= MaxCities)
      {
        ++MaxCities;
        MaxCities *= 3;
        MaxCities /= 2;

        TempCity = realloc(City,
                           MaxCities * sizeof *TempCity);
        if(NULL == TempCity)
        {
          free(City);
          fclose(fp);
          puts("Out of memory.");
          exit(EXIT_FAILURE);
        }

        City = TempCity;
      }
      memcpy(City + NumCities - 1,
             &ThisCity,
             sizeof *City);
    }
  }

  fclose(fp);

  TempCity = realloc(City, NumCities * sizeof *TempCity);
  if(NULL == TempCity)
  {
    puts("Something odd is happening. realloc returned");
    puts("NULL for a /reduction/ in storage.");
  }
  else
  {
    City = TempCity;
  }

  j = Random(NumCities - 6) + 6;

  /* Build a random list of cities */
  for(i = 0; i < j; i++)
  {
    k = Random(NumCities);

    if(DL_SUCCESS != DLAddAfter(&List,
                                0,
                                City + k,
                                sizeof *City))
    {
      puts("Out of memory. Aborting.");
      free(City);
      DLDestroy(&List);

      exit(EXIT_FAILURE);
    }
  }

  Safe = List;

  while(j > 1)
  {
    TotalDistance = 0;

    First = DLGetData(List, NULL, NULL);

    while(DLGetNext(List) != NULL)
    {
      List = DLGetNext(List);
      Second = DLGetData(List, NULL, NULL);

      Distance =
        CalcGreatCircleDistance(First, Second);

      printf("%s - %s : %d miles.\n",
        First->CityName, Second->CityName, Distance);

      TotalDistance += Distance;

      First = Second;
    }

    printf("Total distance for this route: %ld miles.\n",
           TotalDistance);

    if(j > 2)
    {
      printf("---- Removing one city ----\n");
    }

    k = Random(j - 1);
    for(i = 0; i < k; i++)
    {
      List = DLGetPrev(List);
    }
    DLDelete(List);

    List = Safe;
    --j;
  }

  /* Destroy the list */
  DLDestroy(&List);

  free(City);

  return EXIT_SUCCESS;
}
