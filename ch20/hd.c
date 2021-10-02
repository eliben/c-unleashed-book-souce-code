/*  hd.c - Hex dump code
 *
 *  HD - Hexadecimal Dump program
 *
 *  Copyright (C) 2000  Richard Heathfield
 *                      Eton Computer Systems Ltd
 *                      Macmillan Computer Publishing
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
 *  Richard Heathfield may be contacted by email at:
 *     binary@eton.powernet.co.uk
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

void Help(void)
{
  char *HelpMsg[] =
  {
    "hd",
    "Usage: hd [-c<cols>] filename",
    "Examples: hd foo.bin",
    "          hd -c32 core.dump",
    "Reads filename, writes hex dump to",
    "standard output. cols, if supplied, is the",
    "number of characters displayed on each line.",
    "The default is 16."
  };

  size_t ThisMsg;
  size_t NumMsgs;
  
  NumMsgs = sizeof HelpMsg / sizeof HelpMsg[0];

  for(ThisMsg = 0; ThisMsg < NumMsgs; ThisMsg++)
  {
    puts(HelpMsg[ThisMsg]);
  }
}

int HexDump(char *Filename, size_t Width)
{
  int Status = EXIT_SUCCESS;

  FILE *InFilePtr;

  /* Next line relies on hexadecimal digits being
   * representable in 4 bits (which it is), and
   * ensures that we do not rely on there being
   * only 2 nybbles in a byte. (On some systems,
   * CHAR_BIT is /not/ 8.)
   */
  int HexitsPerByte = (CHAR_BIT + 3) / 4;

  unsigned char *TextBuffer;
  unsigned char *HexBuffer;

  size_t BytesRead;
  size_t ThisByte;
  size_t HexBuffWidth;

  /* First, we acquire the necessary external resources.
   * If any of these acquisitions fails, we display an
   * error message and return to the caller.
   *
   * The required resources are: a file pointer to the
   * file from which we wish to read, a text buffer for
   * reading the data into, and a hex buffer for storing
   * the hex dump itself.
   */
  InFilePtr = fopen(Filename, "rb");
  if(NULL == InFilePtr)
  {
    printf("Can't open file %s for reading.\n",
           Filename);
    Status = EXIT_FAILURE;
  }
  else
  {
    HexBuffWidth = Width * (HexitsPerByte + 1);

    TextBuffer = malloc(Width + 1);
    if(NULL == TextBuffer)
    {
      printf("Couldn't allocate %u bytes.\n",
             (unsigned)(Width + 1));
      Status = EXIT_FAILURE;
    }
    else
    {
      HexBuffer = malloc(HexBuffWidth + 1);
      if(NULL == HexBuffer)
      {
        printf("Couldn't allocate %u bytes.\n",
               (unsigned)(HexBuffWidth + 1));
        Status = EXIT_FAILURE;
      }
      else
      {
        /* We process 'Width' bytes at a time.
         * By storing the number of bytes actually
         * read (which we obtain by specifying
         * a block size of 1 and a count of 'Width'),
         * we can handle the last few bytes of a
         * file that isn't an exact multiple of the
         * specified width, in the same loop as the
         * bulk of the file.
         */
        while((BytesRead = fread(TextBuffer,
                                 1,
                                 Width,
                                 InFilePtr)) > 0)
        {
          for(ThisByte = 0;
              ThisByte < BytesRead;
              ThisByte++)
          {
            /* Insert hex code into hex buffer
             * at the appropriate offset
             */
            sprintf((char *)(HexBuffer +
                             ThisByte *
                             (HexitsPerByte + 1)),
                    "%0*X ",
                    HexitsPerByte,
                    (unsigned)TextBuffer[ThisByte]);
            /* We don't want to print unprintable
             * characters, so we'll replace them with
             * a '.' character (a common convention).
             */
            if(!isprint(TextBuffer[ThisByte]))
            {
              TextBuffer[ThisByte] = '.';
            }
          }

          /* null-terminating a string buffer is
           * commonly regarded as a good move :-)
           */
          TextBuffer[ThisByte] = '\0';

          printf("%-*.*s | %s\n",
                 HexBuffWidth,
                 HexBuffWidth,
                 HexBuffer,
                 TextBuffer);
        }
        if(ferror(InFilePtr))
        {
          printf("Warning: read error on file %s.\n",
                 Filename);
          Status = EXIT_FAILURE;
        }

        /* release memory resource */
        free(HexBuffer);
      }

      /* release memory resource */
      free(TextBuffer);
    }

    /* release file resource */
    fclose(InFilePtr);
  }

  return Status;
}


int main(int argc, char **argv)
{
  int Status = EXIT_SUCCESS;
  size_t NumChars = 16;
  char *EndPointer;
  int FilenameArg = 1;

  if(argc < 2 ||  strcmp("-?", argv[1]) == 0)
  {
    Help();
    Status = EXIT_FAILURE;
  }
  else
  {
    if(memcmp(argv[1], "-c", 2) == 0)
    {
      FilenameArg = 2;
      NumChars = (size_t)strtoul(argv[1] + 2,
                                 &EndPointer,
                                 10);

      if(EndPointer == argv[1] + 2 || NumChars < 1)
      {
        Help();
        puts("\nInvalid or missing argument.");
        Status = EXIT_FAILURE;
      }
    }
  }

  if(EXIT_SUCCESS == Status)
  {
    Status = HexDump(argv[FilenameArg], NumChars);
  }

  return Status;
}
