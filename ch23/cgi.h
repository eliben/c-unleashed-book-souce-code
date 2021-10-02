/*  cgi.h - CGI Utilities 
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
*  Chad Dixon may be contacted by email at:
*     http://www.loopy.org
*
*/

#ifndef CGI_H__
#define CGI_H__

#define CGI_SUCCESS                     0
#define CGI_NULL_REQ_METHOD             1
#define CGI_UNKNOWN_METHOD              2
#define CGI_NO_QUERY_STRING             3
#define CGI_NO_MEMORY                   4
#define CGI_BAD_CONTENT_LENGTH          5
#define CGI_NO_DATA                     6

typedef struct CGI_LIST
{
  char *Variable;
  char *Value;
  struct CGI_LIST *Next;
} CGI_LIST;

char *DupString(const char *s);
char *ReadCGIData(int *Error);
int CGIHexToAscii(char *s);
CGI_LIST *CGICreateList(char *Data);
void CGIDestroyList(CGI_LIST *List);

#endif
