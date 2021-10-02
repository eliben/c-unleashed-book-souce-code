/*  cgi.c - CGI Utilities 
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cgi.h"

/* DupString()
 *
 * This code may remind you of the strdup() function
 * provided by many compilers; strdup() is not, however,
 * part of the ANSI C standard library. To be totally
 * portable, therefore, we should 'roll our own'.
 * This function returns NULL if the string cannot be
 * duplicated, either because the input pointer is NULL
 * or because there is insufficient memory to make a copy.
 * Otherwise it returns a pointer to the copy.
 * Note: if the function succeeds, we will need to
 * free() the pointer returned by DupString(), once we
 * are finished with that memory.
 */
char *DupString(const char *s)
{
  char *Dup = NULL;
  if(s != NULL)
  {
    Dup = malloc(strlen(s) + 1);
    if(Dup != NULL)
    {
      strcpy(Dup, s);
    }
  }
  return Dup;
}

/* ReadCGIData()
 *
 * This function assumes your data is coming
 * either from GET or POST, and returns a
 * pointer to a safe copy of that data.
 * You are responsible for freeing the
 * memory pointed to by its return value,
 * if the function succeeded.
 *
 * Error return:
 *    NULL
 *    *Error loaded with one value from
 *      CGI_SUCCESS
 *      CGI_NULL_REQ_METHOD
 *      CGI_UNKNOWN_METHOD
 *      CGI_NO_QUERY_STRING
 *      CGI_NO_MEMORY
 *      CGI_BAD_CONTENT_LENGTH
 *      CGI_NO_DATA
 *    all of which are defined in cgi.h
 *
 */

char *ReadCGIData(int *Error)
{
  char *Buffer = NULL;
  char *RequestMethod = NULL;
  char *ContentLength = NULL;
  char *CGIData = NULL;

  size_t Size = 0;

  *Error = CGI_SUCCESS;

  RequestMethod = getenv("REQUEST_METHOD");
  if(NULL == RequestMethod)
  {
    *Error = CGI_NULL_REQ_METHOD;
  }
  
  if(0 == *Error)
  {
    if(strcmp(RequestMethod,"GET") == 0)
    {
      /* GET */
      CGIData = getenv("QUERY_STRING");
      if(NULL == CGIData)
      {
        *Error = CGI_NO_QUERY_STRING;
      }
      else
      {
        Buffer = DupString(CGIData);
        if(NULL == Buffer) 
        {
          *Error = CGI_NO_MEMORY;
        }
      }
    }
    else if(strcmp(RequestMethod,"POST") == 0)
    {
      /* POST */
      ContentLength = getenv("CONTENT_LENGTH");
      if(NULL == ContentLength)
      {
        *Error = CGI_BAD_CONTENT_LENGTH;
      }
      if(0 == *Error)
      {
        Size = (size_t)atoi(ContentLength);
        if(Size <= 0)
        {
          *Error = CGI_BAD_CONTENT_LENGTH;
        }
      }
      if(0 == *Error)
      {
        ++Size;
        Buffer = malloc(Size);
        if(NULL == Buffer)
        {
          *Error = CGI_NO_MEMORY;
        }
        else
        {
          if(NULL == fgets(Buffer, Size, stdin))
          {
            *Error = CGI_NO_DATA;
            free(Buffer);
            Buffer = NULL;
          }
        }
      }
    }
    else
    {
      *Error = CGI_UNKNOWN_METHOD;
    }
  }

  return Buffer;
}

/* CGIHexToAscii()
 *
 * Converts CGI %xx values to their ASCII equivalents.
 *
 */

int CGIHexToAscii(char *s)
{
  /* Set Error to non-zero on error */
  int Error = 0;

  /* It's going to be very convenient to
   * have all the hexadecimal digits in
   * a contiguous, ordered string
   */
  static const char *Hex = "0123456789ABCDEF";

  /* We'll use this to store an offset
   * into the Hex array.
   */
  unsigned int Ascii = 0;

  /* This is basically a spare pointer.
   * We walk it along the list, using
   * it as the current place for storing
   * characters.
   */
  char *p;

  /* non-NULL if a valid hexadecimal digit was found */
  char *Match;

  /* For each character in the string... */
  for(p = s; !Error && *s != '\0'; s++)
  {
    /* Is it a hex-coded character? */
    if(*s == '%')
    {
      /* Yes. Leaving p behind, move s on to
       * the first byte of the encoding.
       */
      s++;

      /* It had better be in the Hex string.
       * If not, that's an error in the data.
       */
      if((Match = strchr(Hex, *s)) != NULL)
      {
        /* We can now calculate the high nybble of the
         * ASCII equivalent of the coded character.
         */
        Ascii = (unsigned int)(Match - Hex);

        /* And now we do the whole thing
         * again for the low nybble.
         */
        s++;
        if((Match = strchr(Hex, *s)) != NULL)
        {
          /* Move the high nybble out of the way */
          Ascii <<= 4;
          Ascii |= (unsigned int)(Match - Hex);

          /* Now we can update the string.
           * Note that p lags behind s by two characters
           * for each hex-encoded character in the data.
           */
          *p++ = (char)Ascii;
        }
        else
        {
          Error = 1;
        }
      }
      else
      {
        Error = 1;
      }
    }
    /* If it's not a hex-encoded character, could it
     * be a +, which needs to become a space?
     */
    else if(*s == '+')
    {
      *p++ = ' ';
    }
    /* No? In which case we just copy the character. */
    else
    {
      *p++ = *s;
    }
  }
  if(!Error)
  {
    /* Let's not forget to zero-terminate the string. */
    *p = '\0';
  }
  return Error;
}

/* CGIDestroyList()
 *
 * Destroys a list built by CGICreateList.
 *
 */
void CGIDestroyList(CGI_LIST *List)
{
  CGI_LIST *Next = NULL;

  while(List != NULL)
  {
    Next = List->Next;
    if(List->Variable != NULL)
    {
      free(List->Variable);
    }
    /* Note: we don't free Value because
     * it is pointing into the Variable
     * area anyway.
     */
    free(List);
    List = Next;
  }
}

/* CGICreateList()
 *
 * Creates a list of CGI variable/value pairs.
 *
 */
CGI_LIST *CGICreateList(char *Data)
{
  /* The list base pointer */
  CGI_LIST *CGIList;
  /* Our current position within the list */
  CGI_LIST *CurrItem = NULL;

  /* If we get any error, however slight, ditch
   * the whole list and walk off the pitch.
   */
  int Error = 0;

  /* One field. Tokenised from Data using '=' */
  char *Field;

  /* Get some memory for the base node */
  CGIList = malloc(sizeof *CGIList);
  if(CGIList != NULL)
  {
    /* Point travelling pointer to base of list */
    CurrItem = CGIList;

    /* Get the first field. Because strtok modifies its
     * input, we can't use the query string directly in
     * case it was stored in the environment variable
     * (GET method) which is why we made a copy earlier.
     */
    Field = strtok(Data, "&\n");
    if(NULL == Field)
    {
      Error = 1; /* No data! */
    }

    /* For each field */
    while(0 == Error && Field != NULL)
    {
      /* Convert encoded characters (eg %21 to '!') */
      if(CGIHexToAscii(Field) != 0)
      {
        Error = 1;
      }
      else
      {
        /* Ensure that default values are in place
         * NB can't use memset because we have no
         * assurance from the ANSI Standard that
         * NULL has a bit pattern of all bits zero.
         */
        CurrItem->Value    = NULL;
        CurrItem->Next     = NULL;

        /* Copy the field data for safe keeping */
        CurrItem->Variable = DupString(Field);
        if(NULL == CurrItem->Variable)
        {
          Error = 2;
        }
        else
        {
          /* Start at Variable */
          CurrItem->Value = CurrItem->Variable;

          /* Keep going unti we hit '=' */
          while(*CurrItem->Value != '\0' &&
                *CurrItem->Value != '=')
          {
            ++CurrItem->Value;
          }
          if('\0' == *CurrItem->Value)
          {
            Error = 3;
          }
          else
          {
            /* Zero-terminate Variable */
            *CurrItem->Value = '\0';
            /* Point to first byte of value */
            ++CurrItem->Value;
          }
        }
      }

      if(0 == Error)
      {
        /* Get next token */
        Field = strtok(NULL, "&\n");
        if(Field != NULL)
        {
          /* allocate memory for next node */
          CurrItem->Next = malloc(sizeof *CurrItem->Next);
          if(NULL == CurrItem->Next)
          {
            Error = 1;
          }
          else
          {
            CurrItem = CurrItem->Next;
          }
        }
      }
    }
    /* If anything went wrong, bin the whole list */
    if(Error != 0)
    {
      CGIDestroyList(CGIList);
      CGIList = NULL;
    }
  }
  return CGIList;
}

/* end of cgi.c */
