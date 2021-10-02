/*  cups.c - Windows CUPS Library Code
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

/* C Unleashed Portable Sockets library - Windows */

#include "cups.h"


void CUPSInit(CUPS_INFO *pInfo)
{
  pInfo->VersionRequested = MAKEWORD(1, 1);
}

int CUPSStartup(CUPS_INFO *pInfo)
{
  int Result = 0; /* 0 = success */
  WSADATA wsaData;
  WSAStartup(pInfo->VersionRequested, &wsaData);

  if(wsaData.wVersion != pInfo->VersionRequested)
  {
    Result = 1;
  }

  return Result;
}

int CUPSGetHostByName(CONNECTION *Connection, char *Host)
{
  int Result=0;

   if ((Connection->he=gethostbyname(Host)) == NULL) 
    {  
      Result = 1; 
    }
   return Result;
}



int CUPSInitTCPConnection(CONNECTION *Connection,
                          unsigned long Address,
				          unsigned short Port)
{
  int Result = 0;
 if(!Connection->he)
 { 
  Connection->my_addr.sin_family = AF_INET;
  Connection->my_addr.sin_addr.s_addr = Address;
  Connection->my_addr.sin_port = htons(Port);
 }
 else
 {
  Connection->their_addr.sin_family = AF_INET;
  Connection->their_addr.sin_addr = *((LPIN_ADDR)*Connection->
	                                he->h_addr_list);
  Connection->their_addr.sin_port = htons(Port);	
 }
  Connection->Socket = socket(AF_INET, SOCK_STREAM, 0);
  if(Connection->Socket == INVALID_SOCKET)
  {
    Result = 1;
  }
  return Result;
}


int CUPSInitUDPConnection(CONNECTION *Connection,
                          unsigned long Address,
				          unsigned short Port)
{
 int Result = 0;
 Connection->address_length = sizeof(struct sockaddr_in);
 
 if(!Connection->he)
 { 
  Connection->my_addr.sin_family = AF_INET;
  Connection->my_addr.sin_addr.s_addr = Address;
  Connection->my_addr.sin_port = htons(Port);
 }
 else
 {
  Connection->their_addr.sin_family = AF_INET;
  Connection->their_addr.sin_addr = *((LPIN_ADDR)*Connection->
	                                he->h_addr_list);
  Connection->their_addr.sin_port = htons(Port);	
 }
  Connection->Socket = socket(AF_INET, SOCK_DGRAM, 0);
  if(Connection->Socket == INVALID_SOCKET)
  {
    Result = 1;
  }
  return Result;
}




int CUPSConnect(CONNECTION *Connection)
{
 return connect(Connection->Socket, 
     	       (LPSOCKADDR)&Connection->their_addr, 
	    	   sizeof (struct sockaddr));
 }



int CUPSBind(CONNECTION *Connection)
{
  int Result = 0;

  if(bind(Connection->Socket,
          (LPSOCKADDR)&Connection->my_addr,
	  sizeof(struct sockaddr)) == SOCKET_ERROR)
  {
    Result = 1;
  }

  return Result;
}

int CUPSListen(CONNECTION *Connection)
{
  int Result = 0;

  if(listen(Connection->Socket, SOMAXCONN) == SOCKET_ERROR)
  {
    Result = 1;
  }

  return Result;
}

int CUPSShutdown(CONNECTION *Connection, int ShutdownType)
{
  if(ShutdownType != CUPS_SHUTDOWN_RECEIVE &&
     ShutdownType != CUPS_SHUTDOWN_SEND)
  {
    ShutdownType = CUPS_SHUTDOWN_BOTH;
  }

  shutdown(Connection->Socket, ShutdownType);

  return 0;
}

int CUPSAcceptConnection(CONNECTION *NewSocket, 
						 CONNECTION *ServerConnection)
{
  int Result = 0;
  *NewSocket = *ServerConnection;
  NewSocket->Socket = accept(ServerConnection->Socket, 
							 NULL, NULL);
  if(NewSocket->Socket == INVALID_SOCKET)
  {
    Result = 1;
  }
  return Result;
}

int CUPSRecv(CONNECTION *Connection, char *data, size_t size)
{
  int Result = 0;

  if(recv(Connection->Socket, 
	      data, (int)size, 0) == SOCKET_ERROR)
  {
    Result = 1;
  }

  return Result;
}


int CUPSRecvFrom(CONNECTION *Connection, 
				 char *data, 
				 size_t size)
{
 int Result = 0;
  
  if(recvfrom(Connection->Socket, 
	          data, 
			  size, 
			  0, 
	          (struct sockaddr *)&Connection->their_addr, 
	          &Connection->address_length) == SOCKET_ERROR)
  {
    Result = 1;
  }

  return Result;
}


int CUPSSend(CONNECTION *Connection, char *data, size_t size)
{
  int Result = 0;

  if(send(Connection->Socket, 
	      data, 
		  (int)size, 
		  0) == SOCKET_ERROR)
  {
    Result = 1;
  }

  return Result;
}


int CUPSSendTo(CONNECTION *Connection, char *data, size_t size)
{
  int Result = 0;


  if(sendto(
	  Connection->Socket, 
	  data, 
	  size, 
	  0, 
	  (struct sockaddr *)&Connection->their_addr, 
	  (int) sizeof (struct sockaddr)) == SOCKET_ERROR)
  {
    Result = 1;
  }

  return Result;
}





int CUPSShutdownClientConnection(CONNECTION *Connection, int ShutdownType)
{
  if(ShutdownType != CUPS_SHUTDOWN_RECEIVE &&
     ShutdownType != CUPS_SHUTDOWN_SEND)
  {
    ShutdownType = CUPS_SHUTDOWN_BOTH;
  }

  shutdown(Connection->Socket, ShutdownType);

  return 0;
}

int CUPSCleanup(CUPS_INFO *CupsInfo)
{
  WSACleanup();
  return 0;
}


int CUPSCheckForData(CONNECTION *Connection)
{
  struct timeval tv;
  fd_set read_fd;
  tv.tv_sec=0;
  tv.tv_usec=0;
  FD_ZERO(&read_fd);
  FD_SET(Connection->Socket, &read_fd);
  if(select(Connection->Socket+1, 
	        &read_fd,
			NULL, 
			NULL, 
			&tv)== -1)
  {
    return 0;
  }
  if(FD_ISSET(Connection->Socket, &read_fd))
  {
    return 1;
  }
 return 0;
}

