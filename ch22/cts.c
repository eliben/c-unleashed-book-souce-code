/*  cts.c - CUPS TCP Server Code
*
*
*  Copyright (C) 1999   Chad Dixon
*                       Macmillan Computer Publishing
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
*  Chad Dixon may be contacted at:
*     http://www.loopy.org
*
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "cups.h"

#define DEFAULT_FILE_NAME "sales.dat"

volatile sig_atomic_t done;

void inthandler(int Sig)
{
 done = 1;
}

int main(int argc, char **argv)
{
  CUPS_INFO CupsInfo = {0};

  CONNECTION ServerConnection = {0};
  CONNECTION ConnectionToClient = {0};

  char data[255] = {0};
  char *endp;
  FILE *fp;
  char Filename[FILENAME_MAX] = DEFAULT_FILE_NAME;
  unsigned long currenttotal = 0;
  unsigned long  newtotal = 0;

  if(argc < 2)
  {
    printf("No file specified. Using \"%s\"\n", DEFAULT_FILE_NAME);
  }
  else
  {
    strcpy(Filename, argv[1]);
  }

  fp = fopen(Filename, "r");
  if(fp != NULL)
  {
    if(NULL != fgets(data, sizeof data, fp))
    {
      currenttotal = strtoul(data, &endp, 10);
    }

    fclose(fp);
  }
  else
  {
    printf("Couldn't read file %s\n", Filename);
  }

  if(signal(SIGINT, SIG_IGN) != SIG_IGN)
   {
    signal(SIGINT, inthandler);
   }

  if(signal(SIGTERM, SIG_IGN) != SIG_IGN)
   {
    signal(SIGTERM, inthandler); 
   }

  srand(time(NULL));
  CUPSInit(&CupsInfo);
  if(CUPSStartup(&CupsInfo) != 0)
  {
    printf("Initialization failed.\n");
    fgets(data, sizeof data, stdin);
    return EXIT_FAILURE;
  }

  if(0 != CUPSInitTCPConnection(&ServerConnection, INADDR_ANY, 1091))
  {
    printf("Call to socket() failed\n");
    return EXIT_FAILURE;
  }

  if(0 != CUPSBind(&ServerConnection))
  {
    printf("Can't bind().\n");
    return EXIT_FAILURE;
  }

  if(0 != CUPSListen(&ServerConnection))
  {
    printf("Unexpected error while calling listen().\n");
    CUPSShutdown(&ServerConnection, CUPS_SHUTDOWN_BOTH);
    return EXIT_FAILURE;
  }

  while(!done)
  {
  if(0 != CUPSCheckForData(&ServerConnection))
    {
    if(0 != CUPSAcceptConnection(&ConnectionToClient, &ServerConnection))
    {
      printf("Unexpected error on accept()\n");
    }
	else
    {
    memset(data, 0, sizeof data);
	if(0 != CUPSRecv(&ConnectionToClient, data, sizeof data))
      {
        printf("Unexpected error on recv()\n");
      }
      else
      {
        newtotal += strtoul(data, &endp, 10);

        printf("Received data: %s\n", data);
      }
      sprintf(data, "%lu\n", currenttotal);  
      if(0 != CUPSSend(&ConnectionToClient, data, sizeof data))
      {
        printf("Unexpected error on send()\n");
      }

      CUPSShutdownClientConnection(&ConnectionToClient, CUPS_SHUTDOWN_BOTH);
    }
  }
 }
  CUPSShutdown(&ServerConnection, CUPS_SHUTDOWN_BOTH);

  CUPSCleanup(&CupsInfo);

  printf("User requested program to halt.\n");

  fp = fopen(Filename, "w");

  if(fp != NULL)
  {
    fprintf(fp, "%lu\n", newtotal);

    fclose(fp);
  }
  else
  {
    printf("Couldn't write total %lu to file %s\n", newtotal, Filename);
  }
  return 0;
}
