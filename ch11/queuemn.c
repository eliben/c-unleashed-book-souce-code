/*  queuemn.c - supermarket simulation. Test driver for
 *              queue library.
 *
 *  QUEUE - Queue Library
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
#include <limits.h>
#include <assert.h>

#include "queue.h"

#define CASHIER_WAGE  75.0

#define SECONDS_PER_ITEM  3.5

#define PROFIT_PER_ITEM   0.04

#define CHANCE_OF_CUSTOMER 0.85

#define BUY_PROBABILITY    0.75

#define GOODWILL_PENALTY   1.25

#define OPENING_TIME 32400L /* 9am */
#define CLOSING_TIME 61200L /* 5pm */

typedef struct CUSTOMER
{
  int Index;
  int NumItems;
  long JoinedQueue;
} CUSTOMER;

typedef struct CASHIER
{
  int TimeRemaining;
  int NumCustomersSeen;
  int NumItemsProcessed;
  int CustomerIndex;
} CASHIER;

double RandomP(void)
{
  double r;

  r = rand();
  r /= (RAND_MAX + 1.0);

  return r;
}

int DoShopping(CUSTOMER *c)
{
  double p;

  assert(c != NULL);

  c->NumItems = 1; /* everyone wants _something_ ! */

  while((p = RandomP()) < BUY_PROBABILITY)
  {
    ++c->NumItems;
  }

  return c->NumItems;
}

char *GetTime(long Seconds)
{
  static char TheTime[] = "  :  :  ";

  TheTime[0] = Seconds / 36000 + '0';
  Seconds %= 36000;
  TheTime[1] = Seconds / 3600 + '0';
  Seconds %= 3600;
  TheTime[3] = Seconds / 600 + '0';
  Seconds %= 600;
  TheTime[4] = Seconds / 60 + '0';
  Seconds %= 60;
  TheTime[6] = Seconds / 10 + '0';
  Seconds %= 10;
  TheTime[7] = Seconds + '0';

  return TheTime;
}

int main(void)
{
  QUEUE Queue = {0};

  CASHIER *Cashier = NULL;
  CASHIER EmptyCashier = {0};
  CUSTOMER Customer = {0};
  CUSTOMER EmptyCustomer = {0};

  int CustomerNumber = 0;
  int TotalItems = 0;
  int TotalItemsSold = 0;
  int SatisfiedCustomers = 0;
  long TotalWaitTime = 0;
  double Balance;

  int NumCashiers;
  unsigned long TempCashiers;
  long CurrentSecond;
  char Buff[10];
  char *endp;

  double p;

  int i;

  int NumCustomers = 0;

  srand((unsigned)time(NULL));

  /* get data from user */

  printf("Welcome to ShopSim!\n\n");
  printf("Your goal is to maximise profit.\n");
  printf("Income is number of items processed * %f.\n",
         PROFIT_PER_ITEM);
  printf("Customers wander in at random, on average %f"
         " per second.\n",
         CHANCE_OF_CUSTOMER);
  printf("Expenditure is %f per cashier.\n",
         CASHIER_WAGE);
  printf("You choose the number of cashiers.\n\n");
  printf("Dissatisfied customers cost you goodwill.\n");


  do
  {
    printf("How many cashiers today?\n");

    if(NULL == fgets(Buff, sizeof Buff, stdin))
    {
      printf("Program aborted.\n");
      exit(EXIT_FAILURE);
    }

    TempCashiers = strtoul(Buff, &endp, 10);

    /* On some platforms, unsigned long is longer than int,
     * so we use a temp to catch strtoul's value.
     */
    if(endp == Buff ||
       0 == TempCashiers ||
       TempCashiers > (unsigned long)INT_MAX)
    {
      printf("Ahem. %s? Try again!\n\n", Buff);
    }
    else
    {
      NumCashiers = (int)TempCashiers;
      Cashier = malloc(NumCashiers * sizeof *Cashier);
      if(NULL == Cashier)
      {
        printf("Hmm. Not enough RAM for "
               "that many cashiers. Try again.\n");
      }
    }
  } while(NULL == Cashier);

  for(i = 0; i < NumCashiers; i++)
  {
    Cashier[i] = EmptyCashier;
  }

  /* Run simulation from 9am till 5pm (28800 seconds) */

  assert(OPENING_TIME < CLOSING_TIME);

  for(CurrentSecond = OPENING_TIME;
      CurrentSecond < CLOSING_TIME;
      CurrentSecond++)
  {
    p = RandomP();
    if(p < CHANCE_OF_CUSTOMER)
    {
      Customer = EmptyCustomer;
      Customer.Index = CustomerNumber++;
      Customer.JoinedQueue = CurrentSecond;
      TotalItems += DoShopping(&Customer);

      /* Customer is now set up. Let's add
       * him to the queue.
       */
      if(QUEUE_SUCCESS !=
         QueueAdd(&Queue, 0, &Customer, sizeof Customer))
      {
        printf("Insufficient memory.\n");
        exit(EXIT_FAILURE);
      }
    }

    for(i = 0; i < NumCashiers; i++)
    {
      /* Deal with current customers */
      if(Cashier[i].TimeRemaining > 0)
      {
        --Cashier[i].TimeRemaining;
        if(0 == Cashier[i].TimeRemaining)
        {
          printf("%s: Cashier %d has served customer %d\n",
                 GetTime(CurrentSecond),
                 i,
                 Cashier[i].CustomerIndex);
        }
      }
      else
      {
        /* Deal with new customer, if any */
        if(QueueCount(&Queue) > 0)
        {
          QueueRemove(&Customer, &Queue);
          Cashier[i].TimeRemaining =
            (int)(Customer.NumItems * SECONDS_PER_ITEM);
          printf("Cashier %d busy for %d seconds\n",
            i, Cashier[i].TimeRemaining);
          Cashier[i].CustomerIndex = Customer.Index;
          TotalWaitTime += CurrentSecond;
          TotalWaitTime -= Customer.JoinedQueue;
          ++SatisfiedCustomers;
          ++Cashier[i].NumCustomersSeen;
          Cashier[i].NumItemsProcessed +=
            Customer.NumItems;
          TotalItemsSold += Customer.NumItems;
          printf("%s: Cashier %d is serving customer %d\n",
                 GetTime(CurrentSecond),
                 i,
                 Cashier[i].CustomerIndex);
          printf("Customer %d waiting time: %s\n",
                 Customer.Index,
                 GetTime(CurrentSecond -
                   Customer.JoinedQueue));
        }
      }
    }
  }

  printf("%s: Simulation ended.\n", GetTime(CurrentSecond));
  printf("Items sold     :%9d  ", TotalItemsSold);
  printf("Items picked up:%9d  ", TotalItems);
  printf("Difference     :%9d\n", TotalItems - TotalItemsSold);
  printf("Satisfied customers:%9d  ", SatisfiedCustomers);
  printf("Dissatisfied customers:%9d\n",
         CustomerNumber - SatisfiedCustomers);

  if(TotalWaitTime > 0)
  {
    printf("Average wait time "
           "(satisfied customers only): %s\n",
           GetTime((long)(TotalWaitTime /
           (double)SatisfiedCustomers)));
  }

  printf("\n\nCashier Report\n\n");
  for(i = 0; i < NumCashiers; i++)
  {
    printf("Cashier %2d saw %d customers ",
           i, Cashier[i].NumCustomersSeen);
    printf("and dealt with %d items.\n",
           Cashier[i].NumItemsProcessed);
  }

  Balance = TotalItemsSold * PROFIT_PER_ITEM;
  printf("Total profit      : %12.2f\n",
         Balance);
  printf("Less cashier wages: %12.2f\n",
         NumCashiers * CASHIER_WAGE);
  Balance -= NumCashiers * CASHIER_WAGE;

  printf("Balance           : %12.2f\n", Balance);

  printf("Goodwill Penalty  : %12.2f\n",
         (CustomerNumber - SatisfiedCustomers) *
         GOODWILL_PENALTY);

  Balance -= (CustomerNumber - SatisfiedCustomers) *
              GOODWILL_PENALTY;

  printf("End Balance       : %12.2f\n", Balance);

  printf("You %s.\n",
         Balance > 0 ?
         "win" : "lose");

  QueueDestroy(&Queue);

  free(Cashier);

  return 0;
}
