/*  cuc.c - CUPS UDP Client Code
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

#include "cups.h"


int main(int argc, char *argv[])
{
  CUPS_INFO CupsInfo = {0};  
  CONNECTION ClientConnection = {0};  
  char data[32]={0};
  
  int nRet=0;
	
  if (argc != 3) 
   {
    printf("usage: application celcius hostname\n");
    return EXIT_FAILURE; 
   }
  
  sprintf(data, "%s", argv[1]); 
  
  CUPSInit(&CupsInfo);
  if(CUPSStartup(&CupsInfo) != 0)
  {
    printf("Initialisation failed.\n");
    fgets(data, sizeof data, stdin);
    return EXIT_FAILURE;
  }

  if (CUPSGetHostByName(&ClientConnection, argv[2]) != 0)
  {
   printf("Failure to resolve Hostname. %s\n", argv[2]);    
   return EXIT_FAILURE; 
  }

  if(0 != CUPSInitUDPConnection(&ClientConnection, INADDR_ANY, 1092))  
  {
   printf("Can't open a socket!\n");    
   return EXIT_FAILURE;  
  }
 
   if (0 != CUPSSendTo(&ClientConnection, data, sizeof data))
   {
    printf("Error on sendto\n");  
   }
	
   memset(data, 0, sizeof data);

   if (0 != CUPSRecvFrom(&ClientConnection, data, sizeof data))
   {
    printf("Error on recvfrom\n");  
   }


   printf("%s Celcius = %s Fahrenheight\n",argv[1],  data);

   CUPSShutdownClientConnection(&ClientConnection, 2);
   CUPSCleanup(&CupsInfo);

  return EXIT_SUCCESS;
}


