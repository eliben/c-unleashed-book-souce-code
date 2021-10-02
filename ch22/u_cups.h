/*  cups.h - UNIX Version of the CUPS Library Header File
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


/* this is the Unix version of cups.h */
#ifndef CUPS_H__
#define CUPS_H__

#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netdb.h>
#include <stdlib.h>

#define CUPS_SHUTDOWN_RECEIVE	0
#define CUPS_SHUTDOWN_SEND	1
#define CUPS_SHUTDOWN_BOTH	2

typedef struct CUPS_INFO
{
 int Dummy; /* placeholder: Windows needs init info, Unix doesn't */
} CUPS_INFO;

typedef struct CONNECTION
{
  int address_length;
  struct sockaddr_in my_addr;
  struct sockaddr_in their_addr;
  struct hostent *he;  
  socklen_t sin_size;
  int Socket;
} CONNECTION;

void CUPSInit(CUPS_INFO *pInfo);
int CUPSStartup(CUPS_INFO *pInfo);
int CUPSGetHostByName(CONNECTION *Connection, char *);
int CUPSConnect(CONNECTION *Connection);
int CUPSCheckForData(CONNECTION *Connection);
int CUPSInitTCPConnection(CONNECTION *Connection,
						  unsigned long Address,
						  unsigned short Port);
int CUPSInitUDPConnection(CONNECTION *Connection,
						  unsigned long Address,
						  unsigned short Port);
int CUPSBind(CONNECTION *Connection);
int CUPSListen(CONNECTION *Connection);
int CUPSShutDown(CONNECTION *Connection, int ShutdownType);
int CUPSAcceptConnection(CONNECTION *NewSocket, 
						 CONNECTION *ServerConnection);
int CUPSRecv(CONNECTION *Connection, char *data, size_t size);
int CUPSSend(CONNECTION *Connection, char *data, size_t size);
int CUPSRecvFrom(CONNECTION *Connection, char *data, size_t size);
int CUPSSendTo(CONNECTION *Connection, char *data, size_t size);
int CUPSShutdownClientConnection(CONNECTION *Connection, 
								 int ShutdownType);
int CUPSCleanup(CUPS_INFO *CupsInfo);

#endif


