/*  cus.c - CUPS UDP Server Code
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
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#include "cups.h"

volatile sig_atomic_t done;


void inthandler(int Sig)
{
 done = 1;
}

int main(int argc, char **argv)
 { 
  CUPS_INFO CupsInfo = {0};  
  CONNECTION ServerConnection = {0};  
  char data[32]= {0};
  int celc=0;
  int farh=0;


  CUPSInit(&CupsInfo);
  if(CUPSStartup(&CupsInfo) != 0)
  {
    printf("Initialisation failed.\n");
    fgets(data, sizeof data, stdin);
    return EXIT_FAILURE;
  }

  
  if(signal(SIGINT, SIG_IGN) != SIG_IGN)
   {
    signal(SIGINT, inthandler);
   }

  if(signal(SIGTERM, SIG_IGN) != SIG_IGN)
   {
    signal(SIGTERM, inthandler); 
   }

 if(0 != CUPSInitUDPConnection(&ServerConnection, INADDR_ANY, 1092))  
  {
   printf("Can't open a socket!\n");    
   return EXIT_FAILURE;  
  }
  
  if(0 != CUPSBind(&ServerConnection))  
  {   
   printf("Can't bind.\n");
   return EXIT_FAILURE;  
  }

  

  
  while(!done) 
  {  
   if(0 != CUPSCheckForData(&ServerConnection))
   {
    memset(data, 0, sizeof data);
    
	if (0 != CUPSRecvFrom(&ServerConnection, data, sizeof data))
    {
     printf("Error on recvfrom\n");  
	}
  	
	
    celc=atoi(data);
    farh=(celc*2)+32;
	memset(data, 0, sizeof data);
	sprintf(data, "%d", farh);
        

	if (0 != CUPSSendTo(&ServerConnection, data, sizeof data))
    {
     printf("Error on sendto\n");  
	}
	   
	printf("%s\n", data);	   
   }    
  }

  CUPSShutdownClientConnection(&ServerConnection, 2);
  CUPSCleanup(&CupsInfo);

  printf("User requested program to halt.\n");
   
   return EXIT_SUCCESS; 
}



