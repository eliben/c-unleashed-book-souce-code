/*  ctc.c - CUPS TCP Client Code
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

#include "cups.h"

int Random(int n)
{
  return (int)(n * (rand() / (1.0 + (double)RAND_MAX)));
}

int main(int argc, char *argv[])
 {
   CUPS_INFO CupsInfo = {0};
   CONNECTION ClientConnection = {0};

   char data[255]={0};
 
   if (argc != 2) 
    {
     printf("usage: application hostname\n");
     return EXIT_FAILURE; 
    }
   
   srand(time(NULL));
   CUPSInit(&CupsInfo);
   if(CUPSStartup(&CupsInfo) != 0)
    {    
     printf("Initialization failed.\n");
     fgets(data, sizeof data, stdin);
     return EXIT_FAILURE;
    }

   if (CUPSGetHostByName(&ClientConnection, argv[1]) != 0)
    {
     printf("Failure to resolve Hostname. %s\n", argv[1]);    
     return EXIT_FAILURE; 
    }

   if(0 != CUPSInitTCPConnection(&ClientConnection, INADDR_ANY, 1091))
    {
     printf("Error on call to socket().\n");
     return EXIT_FAILURE;
    }


   if (0 != CUPSConnect(&ClientConnection))
    {
     printf("Error on connect().\n");
     return EXIT_FAILURE;
    }


   sprintf(data, "%lu Branch %d", (unsigned long)Random(10000), Random(100));   
   if(0 != CUPSSend(&ClientConnection, data, sizeof data))
      {
        printf("Unexpected error on send()\n");
      }
    
    memset(data, 0, sizeof data);

    
    if(0 != CUPSRecv(&ClientConnection, data, sizeof data))
     {
      printf("Unexpected error on recv()\n");
     }

    printf("Received: %s\n", data);

    CUPSShutdown(&ClientConnection, CUPS_SHUTDOWN_BOTH);    
    return EXIT_SUCCESS; 
}









