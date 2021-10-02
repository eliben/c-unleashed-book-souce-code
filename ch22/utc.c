/*  utc.c - UNIX TCP Client Code
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
#include <errno.h> 
#include <string.h> 
#include <netdb.h> 
#include <netinet/in.h>
#include <sys/socket.h> 


int Random(int n)
{
  return (int)(n * (rand() / (1.0 + (double)RAND_MAX)));
}


int main(int argc, char *argv[])
 {
  int sockfd=0;  
  char data[255]={0};
  struct hostent *he;
  struct sockaddr_in their_addr; 

  srand((unsigned)time(NULL));
  
   if (argc < 2) 
    {
     fprintf(stderr,"usage: application hostname\n");
     return EXIT_FAILURE; 
    }

   if ((he=gethostbyname(argv[1])) == NULL) 
    {  
      printf("Error with gethostbyname()\n");
      return EXIT_FAILURE; 
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
    {
      printf("Error with call to socket()\n");
      return EXIT_FAILURE; 
    }


    their_addr.sin_family = AF_INET;   
    their_addr.sin_port = htons(1091); 
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);

    memset(their_addr.sin_zero, 0, sizeof their_addr.sin_zero); 

    if (connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) ==-1) 
    {
      printf("Error with connect()\n");
      return EXIT_FAILURE; 
    }

    sprintf(data, "%lu Branch %d", (unsigned long)Random(10000), Random(100));

    if (send(sockfd, data, sizeof data, 0) == -1) 
    {
      printf("Error with send()\n");
      return EXIT_FAILURE; 
    }

    memset(data, 0, sizeof data);

    if (recv(sockfd, data, sizeof data, 0) == -1) 
    {
      printf("Error with recv()\n");
      return EXIT_FAILURE; 
    }

    printf("Received: %s\n", data);

    shutdown(sockfd,2);
    return EXIT_SUCCESS; 
}









