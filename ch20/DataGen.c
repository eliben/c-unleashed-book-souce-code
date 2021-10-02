/*  datagen.c - data generator
 *
 *  DATAGEN - Data generator
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

void Help(void)
{
  printf("Usage: datagen outfile randomness numrecs\n");
  printf("Purpose: generates test data in CSV format\n\n");
  printf("Arguments:\n");
  printf("outfile: output filename\n");
  printf("randomness: sets random seed. T means "
         "randomize seed.\n");
  printf("numrecs: Number of records to create\n\n");
  printf("Examples:\n");
  printf("datagen foo 1     400\n");
  printf("datagen bar 234 10000\n");
  printf("datagen baz T      75\n");
}

void Disclaimer(FILE *fp)
{
  char *Disclm[] =
  {
    "Disclaimer",
    " ",
    "This file was automatically generated using",
    "random combinations of names and numbers. It is",
    "possible that - purely by chance - the",
    "name of an existing organisation has been",
    "stored in this file. If this has happened it",
    "does constitute endorsement or otherwise of that",
    "organisation's products or services and in fact",
    "is entirely a chance coincidence and should not",
    "be considered to have any world significance",
    "of any kind whatsoever be it favourable or",
    "otherwise.",
    " ",
    " ",
    NULL
  };

  int i;

  for(i = 0; Disclm[i] != NULL; i++)
  {
    fprintf(fp, "%s\n", Disclm[i]);
  }
}

double RandDbl(void)
{
  return rand() / ((double)RAND_MAX + 1.0);
}

int RandInt(int n)
{
  return (int)(n * RandDbl());
}

int GenerateData(FILE *fp, size_t MaxRecs)
{
  char *Lower = "abcdefghijklmnopqrstuvwxyz";
  char *Upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  char *Name[] =
  {
    "Arrow",          "Bizarre",
    "Complete",       "Drastic",
    "Eagle",          "Fiddleyew",
    "Gilbert",        "Havago",
    "Ingenious",      "J Random Loser",
    "Kludgit & Runn", "Lightheart",
    "Mouse",          "Neurosis",
    "Objective",      "Paradigm",
    "Quality",        "Runaway",
    "Systemic",       "Terrible",
    "Underwater",     "Value",
    "Wannabee",       "YesWeWill"
  };

  char *Business[] =
  {
    "Advertising",      "Building",
    "Computers",        "Deliveries",
    "Engineering",      "Foam Packing",
    "Garage",           "Hotels",
    "Industries",       "Janitorial",
    "Knitwear",         "Laser Printers",
    "Mills",            "Notaries",
    "Office Cleaning",  "Printers",
    "Questionnaires",   "Radio",
    "Systems",          "Talismans",
    "Upholstery",       "Van Hire",
    "Waste Disposal",   "Yo-yos"
  };

  size_t NumNames = sizeof Name / sizeof Name[0];
  size_t NumBuses = sizeof Business / sizeof Business[0];

  time_t date_time_t;
  struct tm date = {0};
  struct tm *pd;

  size_t ThisRec;

  date_time_t = time(NULL);

  pd = localtime(&date_time_t);
  if(pd != NULL)
  {
    memcpy(&date, pd, sizeof date);
  }
  else
  {
    date.tm_mday = 1;
    date.tm_mon = 0;
    date.tm_year = 100;
  }

  fprintf(fp, "Reference,Date,Account,Amount\n");

  for(ThisRec = 0; ThisRec < MaxRecs; ThisRec++)
  {
    fprintf(fp,
            "%c%c%04d/%c%c%c,",
            Upper[RandInt(26)],
            Upper[RandInt(26)],
            RandInt(10000),
            Lower[RandInt(26)],
            Lower[RandInt(26)],
            Lower[RandInt(26)]);

    fprintf(fp,
            "%d/%02d/%02d,",
            date.tm_year + 1900, /* NB: NOT a Y2K bug! */
            date.tm_mon + 1,
            date.tm_mday);

    fprintf(fp,
            "%s ",
            Name[RandInt(8)]);
    fprintf(fp,
            "%s Ltd,",
            Business[RandInt(8)]);

    fprintf(fp,
            "%.2f\n",
            (RandDbl() * 5000.0) - 2250.0);

    if(RandInt(2) == 0)
    {
      ++date.tm_mday;
      date_time_t = mktime(&date);
      if(date_time_t != (time_t)-1)
      {
        pd = localtime(&date_time_t);
        if(pd != NULL)
        {
          memcpy(&date, pd, sizeof date);
        }
      }
    }
  }

  return 0;
}

int main(int argc, char **argv)
{
  FILE *fp;
  int Status;

  if(argc < 4)
  {
    Help();
    Status = EXIT_FAILURE;
  }
  else
  {
    if(argv[2][0] == 'T')
    {
      srand(time(NULL));
    }
    else
    {
      srand((unsigned)strtol(argv[2], NULL, 10));
    }

    fp = fopen(argv[1], "w");
    if(fp != NULL)
    {
      Disclaimer(fp);
      Status = GenerateData(fp,
                            (size_t)strtol(argv[3],
                                           NULL,
                                           10));
      fclose(fp);
    }
  }

  return Status;
}
