/*  cups.h - Windows Version of the CUPS Library Header File
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

/* this is the Windows version of cups.h */
#ifndef CUPS_H__
#define CUPS_H__

#include <stdarg.h>
#include <time.h>
#include "winsock.h"

#define CUPS_SHUTDOWN_RECEIVE	0
#define CUPS_SHUTDOWN_SEND	1
#define CUPS_SHUTDOWN_BOTH	2

#define CUPS_MAX_OUT        256


typedef struct CUPS_INFO
{
  WORD VersionRequested;
} CUPS_INFO;


typedef struct CONNECTION
{
  int address_length;
  SOCKADDR_IN my_addr;
  SOCKADDR_IN their_addr;
  SOCKET Socket;
  LPHOSTENT he;
} CONNECTION;


void CUPSInit(CUPS_INFO *pInfo);
int CUPSEndServer(void);
int CUPSStartup(CUPS_INFO *pInfo);
int CUPSGetHostByName(CONNECTION *Connection, char *Host);
int CUPSInitTCPConnection(CONNECTION *Connection,
						  unsigned long Address,
						  unsigned short Port);
int CUPSInitUDPConnection(CONNECTION *Connection,
						  unsigned long Address,
						  unsigned short Port);
int CUPSCheckForData(CONNECTION *Connection);
int CUPSConnect(CONNECTION *Connection);
int CUPSBind(CONNECTION *Connection);
int CUPSListen(CONNECTION *Connection);
int CUPSShutdown(CONNECTION *Connection, int ShutdownType);
int CUPSAcceptConnection(CONNECTION *NewSocket, 
						 CONNECTION *ServerConnection);
int CUPSCheckForData(CONNECTION *Connection);
int CUPSRecv(CONNECTION *Connection, char *data, size_t size);
int CUPSSend(CONNECTION *Connection, char *data, size_t size);
int CUPSRecvFrom(CONNECTION *Connection, char *data, size_t size);
int CUPSSendTo(CONNECTION *Connection, char *data, size_t size);
int CUPSShutdownClientConnection(CONNECTION *Connection, 
								 int ShutdownType);
int CUPSCleanup(CUPS_INFO *CupsInfo);

#endif

