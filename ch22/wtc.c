/*  wtc.c - Windows TCP Client Code
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
#include <time.h>
#include <winsock.h>


int Random(int n)
{
  return (int)(n * (rand() / (1.0 + (double)RAND_MAX)));
}


int main(int argc, char *argv[])
{
  WORD wVersionRequested = MAKEWORD(1,1);
  SOCKADDR_IN their_addr;
  WSADATA wsaData;
  SOCKET sockfd;
  LPHOSTENT he;
  char data[255]={0};
  
  int nRet=0;

  srand(time(NULL));
  if (argc < 2) 
   {
    printf("usage: application hostname\n");
    return EXIT_FAILURE; 
   }
   
  nRet = WSAStartup(wVersionRequested, &wsaData);
  if (wsaData.wVersion != wVersionRequested)
   {	
    printf("\n Wrong version of Winsock\n");
    return EXIT_FAILURE;
   }
    
     
  he = gethostbyname(argv[1]);
  if (he == NULL)
   {
    printf("Error in gethostbyname().\n");
    return EXIT_FAILURE;
   }

  sockfd = socket(AF_INET,	SOCK_STREAM, IPPROTO_TCP);	
  if (sockfd == INVALID_SOCKET)
   {
    printf("Error on call to socket()\n");
    return EXIT_FAILURE;
   }
  
  their_addr.sin_family = AF_INET;
  their_addr.sin_addr = *((LPIN_ADDR)*he->h_addr_list);
  their_addr.sin_port = htons(1091);	
  
  nRet = connect(sockfd, (LPSOCKADDR)&their_addr, sizeof(struct sockaddr));
  if (nRet == SOCKET_ERROR)
   {
    printf("Error on connect().\n");
    return EXIT_FAILURE;
   }
  
  sprintf(data, "%lu Branch %d", (unsigned long)Random(10000), Random(100));
  
  nRet = send(sockfd,	data, strlen(data), 0);
  if (nRet == SOCKET_ERROR)
   {
    printf("Err on send()\n");
    return EXIT_FAILURE;
   }

  memset(data, 0, sizeof data);

  nRet = recv(sockfd,	data, sizeof data, 0);
  if (nRet == SOCKET_ERROR)
   {
    printf("Error on recv()\n");
  	return EXIT_FAILURE;
   }

  printf("Recieved: %s\n", data);


  shutdown(sockfd,2);
  WSACleanup();
  return EXIT_SUCCESS;
}


