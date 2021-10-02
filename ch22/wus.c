/*  wus.c - Windows UDP Server Code
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
#include <winsock.h>
#include <signal.h>

volatile sig_atomic_t done;


void inthandler(int Sig)
{
 done = 1;
}

int CheckForData(int sockfd)
 {
  struct timeval tv;
  fd_set read_fd;
  tv.tv_sec=0;
  tv.tv_usec=0;
  FD_ZERO(&read_fd);
  FD_SET(sockfd, &read_fd);
  if(select(sockfd+1, &read_fd,NULL, NULL, &tv)== -1)

   return 0;
  if(FD_ISSET(sockfd, &read_fd))
   {
    return 1; 
   }
 return 0;
}          


int main(int argc, char **argv)
 { 
  WORD wVersionRequested = MAKEWORD(1,1);
  WSADATA wsaData;
  SOCKET	sock;
  SOCKADDR_IN   my_addr;
  SOCKADDR_IN   their_addr;
  int address_length;
  char data[32]= {0};
  int celc=0;
  int farh=0;


  if(signal(SIGINT, SIG_IGN) != SIG_IGN)
   {
    signal(SIGINT, inthandler);
   }

  if(signal(SIGTERM, SIG_IGN) != SIG_IGN)
   {
    signal(SIGTERM, inthandler); 
   }

  my_addr.sin_family = AF_INET;
  my_addr.sin_addr.s_addr = INADDR_ANY;	
  my_addr.sin_port = htons(1092);		

 
  WSAStartup(wVersionRequested, &wsaData);
  if (wsaData.wVersion != wVersionRequested)
   {	
    printf("\n Wrong version of Winsock\n");
    return EXIT_FAILURE;
   }

   
  if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
   {
    printf("Unexpected error on socket()");
    return EXIT_FAILURE;
   }
   


  if (bind(sock, (LPSOCKADDR)&my_addr,sizeof(struct sockaddr)) == SOCKET_ERROR)
   {
    printf("Unexpected error on bind()");
    shutdown(sock,2);
    return EXIT_FAILURE;
   }

  address_length = sizeof(struct sockaddr);

  
  while(!done) 
  {  
   if(0 != CheckForData(sock))
   {
    memset(data, 0, sizeof data);

	if (recvfrom(sock, data, 
		         sizeof data, 
		         0,   (struct sockaddr *)&their_addr, 
		         &address_length) == SOCKET_ERROR) 
	{
     printf("Error on recvfrom\n");  
	}
  

    celc=atoi(data);
	farh=(celc*2)+32;
    
	memset(data, 0, sizeof data);
	
	sprintf(data, "%d", farh);
        
   if (sendto(sock, data, strlen(data), 0, 
	         (struct sockaddr *)&their_addr, 
			 sizeof(struct sockaddr)) == SOCKET_ERROR) 
   {
    printf("Error on sendto\n");  
   }
  
	printf("%s\n", data);	   

   
   
   }    
  }



   shutdown(sock,2);
   WSACleanup();
   printf("User requested program to halt.\n");
   
   return EXIT_SUCCESS; 
}



