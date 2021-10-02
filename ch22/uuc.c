/*  uuc.c - UNIX UDP Client Code
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
#include <sys/types.h>     
#include <netinet/in.h>     
#include <netdb.h> 
#include <sys/socket.h> 


int main(int argc, char *argv[])
{
  struct sockaddr_in their_addr;
  struct sockaddr_in my_addr;
  struct hostent *he;
  int sockfd;
  int addr_len;
  char data[32]={0};
  
  int nRet=0;
	
  if (argc != 3) 
   {
    printf("usage: application celsius hostname\n");
    return EXIT_FAILURE; 
   }
  
  sprintf(data, "%s", argv[1]); 
  
  he = gethostbyname(argv[2]);
  if (he == NULL)
   {
    printf("Error in gethostbyname\n");
    return EXIT_FAILURE;
   }

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);	
  if (sockfd == -1)
   {
    printf("Error Creating Socket\n");
    return EXIT_FAILURE;
   }
  their_addr.sin_family = AF_INET;
  their_addr.sin_addr = *((struct in_addr *)he->h_addr);
  their_addr.sin_port = htons(1092);	
  
  
   if (sendto(sockfd, data, strlen(data), 
	0, (struct sockaddr *)&their_addr, 
	sizeof(struct sockaddr)) == -1) 
	{
     printf("Error in sendto\n");
 	 return EXIT_FAILURE;
	}


    memset(data, 0, sizeof data);

    addr_len = sizeof(struct sockaddr);
    if (recvfrom(sockfd, data, sizeof data, 
		0, (struct sockaddr *)&their_addr, &addr_len) == -1) 
	{
     printf("Error in recvfrom\n");
 	 return EXIT_FAILURE;
	}


  printf("%s Celsius = %s Fahrenheight\n",argv[1],  data);


  shutdown(sockfd,2);
  return EXIT_SUCCESS;
}


