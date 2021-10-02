/*  encode.c - contains code to convert binary data to
 *             Group 3 Facsimilie T.4 encoding
 *
 *  Copyright (C) 2000  Jack Klein
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
 *  Jack Klein may be contacted by email at:
 *     The_C_Guru@yahoo.com
 *
 */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include "fax.h"

/* the T4_ENCODE structure type is used to generate the
 * variable bit length T.4 output stream which corresponds
 * to the binary image pixel data
 */
typedef struct
{
  unsigned char code_length;
  unsigned char bit_pattern;
} T4_ENCODE;

static const T4_ENCODE white_terminate[] =
{
  {  8, 0XAC }, /*    0 white pixels */
  {  6, 0XE0 }, /*    1 white pixels */
  {  4, 0XE0 }, /*    2 white pixels */
  {  4, 0X10 }, /*    3 white pixels */
  {  4, 0XD0 }, /*    4 white pixels */
  {  4, 0X30 }, /*    5 white pixels */
  {  4, 0X70 }, /*    6 white pixels */
  {  4, 0XF0 }, /*    7 white pixels */
  {  5, 0XC8 }, /*    8 white pixels */
  {  5, 0X28 }, /*    9 white pixels */
  {  5, 0XE0 }, /*   10 white pixels */
  {  5, 0X10 }, /*   11 white pixels */
  {  6, 0X10 }, /*   12 white pixels */
  {  6, 0XC0 }, /*   13 white pixels */
  {  6, 0X2C }, /*   14 white pixels */
  {  6, 0XAC }, /*   15 white pixels */
  {  6, 0X54 }, /*   16 white pixels */
  {  6, 0XD4 }, /*   17 white pixels */
  {  7, 0XE4 }, /*   18 white pixels */
  {  7, 0X30 }, /*   19 white pixels */
  {  7, 0X10 }, /*   20 white pixels */
  {  7, 0XE8 }, /*   21 white pixels */
  {  7, 0XC0 }, /*   22 white pixels */
  {  7, 0X20 }, /*   23 white pixels */
  {  7, 0X14 }, /*   24 white pixels */
  {  7, 0XD4 }, /*   25 white pixels */
  {  7, 0XC8 }, /*   26 white pixels */
  {  7, 0X24 }, /*   27 white pixels */
  {  7, 0X18 }, /*   28 white pixels */
  {  8, 0X40 }, /*   29 white pixels */
  {  8, 0XC0 }, /*   30 white pixels */
  {  8, 0X58 }, /*   31 white pixels */
  {  8, 0XD8 }, /*   32 white pixels */
  {  8, 0X48 }, /*   33 white pixels */
  {  8, 0XC8 }, /*   34 white pixels */
  {  8, 0X28 }, /*   35 white pixels */
  {  8, 0XA8 }, /*   36 white pixels */
  {  8, 0X68 }, /*   37 white pixels */
  {  8, 0XE8 }, /*   38 white pixels */
  {  8, 0X14 }, /*   39 white pixels */
  {  8, 0X94 }, /*   40 white pixels */
  {  8, 0X54 }, /*   41 white pixels */
  {  8, 0XD4 }, /*   42 white pixels */
  {  8, 0X34 }, /*   43 white pixels */
  {  8, 0XB4 }, /*   44 white pixels */
  {  8, 0X20 }, /*   45 white pixels */
  {  8, 0XA0 }, /*   46 white pixels */
  {  8, 0X50 }, /*   47 white pixels */
  {  8, 0XD0 }, /*   48 white pixels */
  {  8, 0X4A }, /*   49 white pixels */
  {  8, 0XCA }, /*   50 white pixels */
  {  8, 0X2A }, /*   51 white pixels */
  {  8, 0XAA }, /*   52 white pixels */
  {  8, 0X24 }, /*   53 white pixels */
  {  8, 0XA4 }, /*   54 white pixels */
  {  8, 0X1A }, /*   55 white pixels */
  {  8, 0X9A }, /*   56 white pixels */
  {  8, 0X5A }, /*   57 white pixels */
  {  8, 0XDA }, /*   58 white pixels */
  {  8, 0X52 }, /*   59 white pixels */
  {  8, 0XD2 }, /*   60 white pixels */
  {  8, 0X4C }, /*   61 white pixels */
  {  8, 0XCC }, /*   62 white pixels */
  {  8, 0X2C }, /*   63 white pixels */
};

static const T4_ENCODE white_makeup[] =
{
  {  5, 0XD8 }, /*   64 white pixels */
  {  5, 0X48 }, /*  128 white pixels */
  {  6, 0XE8 }, /*  192 white pixels */
  {  7, 0XEC }, /*  256 white pixels */
  {  8, 0X6C }, /*  320 white pixels */
  {  8, 0XEC }, /*  384 white pixels */
  {  8, 0X26 }, /*  448 white pixels */
  {  8, 0XA6 }, /*  512 white pixels */
  {  8, 0X16 }, /*  576 white pixels */
  {  8, 0XE6 }, /*  640 white pixels */
  {  9, 0X33 }, /*  704 white pixels */
  {  9, 0XB3 }, /*  768 white pixels */
  {  9, 0X4B }, /*  832 white pixels */
  {  9, 0XCB }, /*  896 white pixels */
  {  9, 0X2B }, /*  960 white pixels */
  {  9, 0XAB }, /* 1024 white pixels */
  {  9, 0X6B }, /* 1088 white pixels */
  {  9, 0XEB }, /* 1152 white pixels */
  {  9, 0X1B }, /* 1216 white pixels */
  {  9, 0X9B }, /* 1280 white pixels */
  {  9, 0X5B }, /* 1344 white pixels */
  {  9, 0XDB }, /* 1408 white pixels */
  {  9, 0X19 }, /* 1472 white pixels */
  {  9, 0X99 }, /* 1536 white pixels */
  {  9, 0X59 }, /* 1600 white pixels */
  {  6, 0X18 }, /* 1664 white pixels */
  {  9, 0XD9 }, /* 1728 white pixels */
};

static const T4_ENCODE black_terminate[] =
{
  { 10, 0XEC }, /*    0 black pixels */
  {  3, 0X40 }, /*    1 black pixels */
  {  2, 0XC0 }, /*    2 black pixels */
  {  2, 0X40 }, /*    3 black pixels */
  {  3, 0XC0 }, /*    4 black pixels */
  {  4, 0XC0 }, /*    5 black pixels */
  {  4, 0X40 }, /*    6 black pixels */
  {  5, 0XC0 }, /*    7 black pixels */
  {  6, 0XA0 }, /*    8 black pixels */
  {  6, 0X20 }, /*    9 black pixels */
  {  7, 0X20 }, /*   10 black pixels */
  {  7, 0XA0 }, /*   11 black pixels */
  {  7, 0XE0 }, /*   12 black pixels */
  {  8, 0X20 }, /*   13 black pixels */
  {  8, 0XE0 }, /*   14 black pixels */
  {  9, 0X18 }, /*   15 black pixels */
  { 10, 0XE8 }, /*   16 black pixels */
  { 10, 0X18 }, /*   17 black pixels */
  { 10, 0X10 }, /*   18 black pixels */
  { 11, 0XE6 }, /*   19 black pixels */
  { 11, 0X16 }, /*   20 black pixels */
  { 11, 0X36 }, /*   21 black pixels */
  { 11, 0XEC }, /*   22 black pixels */
  { 11, 0X14 }, /*   23 black pixels */
  { 11, 0XE8 }, /*   24 black pixels */
  { 11, 0X18 }, /*   25 black pixels */
  { 12, 0X53 }, /*   26 black pixels */
  { 12, 0XD3 }, /*   27 black pixels */
  { 12, 0X33 }, /*   28 black pixels */
  { 12, 0XB3 }, /*   29 black pixels */
  { 12, 0X16 }, /*   30 black pixels */
  { 12, 0X96 }, /*   31 black pixels */
  { 12, 0X56 }, /*   32 black pixels */
  { 12, 0XD6 }, /*   33 black pixels */
  { 12, 0X4B }, /*   34 black pixels */
  { 12, 0XCB }, /*   35 black pixels */
  { 12, 0X2B }, /*   36 black pixels */
  { 12, 0XAB }, /*   37 black pixels */
  { 12, 0X6B }, /*   38 black pixels */
  { 12, 0XEB }, /*   39 black pixels */
  { 12, 0X36 }, /*   40 black pixels */
  { 12, 0XB6 }, /*   41 black pixels */
  { 12, 0X5B }, /*   42 black pixels */
  { 12, 0XDB }, /*   43 black pixels */
  { 12, 0X2A }, /*   44 black pixels */
  { 12, 0XAA }, /*   45 black pixels */
  { 12, 0X6A }, /*   46 black pixels */
  { 12, 0XEA }, /*   47 black pixels */
  { 12, 0X26 }, /*   48 black pixels */
  { 12, 0XA6 }, /*   49 black pixels */
  { 12, 0X4A }, /*   50 black pixels */
  { 12, 0XCA }, /*   51 black pixels */
  { 12, 0X24 }, /*   52 black pixels */
  { 12, 0XEC }, /*   53 black pixels */
  { 12, 0X1C }, /*   54 black pixels */
  { 12, 0XE4 }, /*   55 black pixels */
  { 12, 0X14 }, /*   56 black pixels */
  { 12, 0X1A }, /*   57 black pixels */
  { 12, 0X9A }, /*   58 black pixels */
  { 12, 0XD4 }, /*   59 black pixels */
  { 12, 0X34 }, /*   60 black pixels */
  { 12, 0X5A }, /*   61 black pixels */
  { 12, 0X66 }, /*   62 black pixels */
  { 12, 0XE6 }, /*   63 black pixels */
};

static const T4_ENCODE black_makeup[] =
{
  { 10, 0XF0 }, /*   64 black pixels */
  { 12, 0X13 }, /*  128 black pixels */
  { 12, 0X93 }, /*  192 black pixels */
  { 12, 0XDA }, /*  256 black pixels */
  { 12, 0XCC }, /*  320 black pixels */
  { 12, 0X2C }, /*  384 black pixels */
  { 12, 0XAC }, /*  448 black pixels */
  { 13, 0X36 }, /*  512 black pixels */
  { 13, 0XB6 }, /*  576 black pixels */
  { 13, 0X52 }, /*  640 black pixels */
  { 13, 0XD2 }, /*  704 black pixels */
  { 13, 0X32 }, /*  768 black pixels */
  { 13, 0XB2 }, /*  832 black pixels */
  { 13, 0X4E }, /*  896 black pixels */
  { 13, 0XCE }, /*  960 black pixels */
  { 13, 0X2E }, /* 1024 black pixels */
  { 13, 0XAE }, /* 1088 black pixels */
  { 13, 0X6E }, /* 1152 black pixels */
  { 13, 0XEE }, /* 1216 black pixels */
  { 13, 0X4A }, /* 1280 black pixels */
  { 13, 0XCA }, /* 1344 black pixels */
  { 13, 0X2A }, /* 1408 black pixels */
  { 13, 0XAA }, /* 1472 black pixels */
  { 13, 0X5A }, /* 1536 black pixels */
  { 13, 0XDA }, /* 1600 black pixels */
  { 13, 0X26 }, /* 1664 black pixels */
  { 13, 0XA6 }, /* 1728 black pixels */
};

/* the PIXEL_BITS enumeration is used to specify the color of */
/* pixel run to look for, and to select the proper T4_ENCODE  */
/* array containing the code word to output                   */
typedef enum
{
  BLACK_PIXEL     =   0x00,
  WHITE_PIXEL     =   0x80,
  PIXEL_TEST      =   0x80
} PIXEL_BITS;

static const char wr_err[] = "encode: error writing %s\n";
static const unsigned char EOL[2] = { 0x00, 0x80 };
static const unsigned char EOP[9] =
{ 0x00, 0x08, 0x80, 0x00, 0x08, 0x80, 0x00, 0x08, 0x80 };

/* Function:    OutputCodeWord
 *
 * Remarks:     only referenced from inside this source
 *              file so defined with the static keyword
 *              for internal linkage
 *
 * Inputs:      T4_DECODE *code, points to the appropriate
 *              structure with the run length code word to
 *              be output, or NULL to indicate that an EOL
 *              code word should be output
 *
 *              unsigned char *t4_ptr, a pointer to a suitable
 *              array of unsigned chars where the output code
 *              words should be stored
 *
 * Returns:     pointer to unsigned char, indicating the next
 *              location where the next octet to be output
 *              will be stored
 *
 * Description: this function inserts the variable length
 *              code words into an output buffer
 *
 * Note:        at any given time this function may hold a
 *              partial octet containing less than 8 bits
 *              which have not yet been stored in the output
 *              array
 *
 *              outputting an EOL code word flushes any partial
 *              octet output and synchronizes the next code
 *              word to start on an byte boundary
 */
static unsigned char
*OutputCodeWord(const T4_ENCODE *code, unsigned char *t4_ptr)
{
  int length;
  static int free_bits = 8;
  static unsigned long current_output = 0;

  /* if the pointer argument is NULL, output EOL code word  */
  if (NULL == code)
  {
    current_output &= OCTET_MASK;
    current_output >>= free_bits;
    *t4_ptr++ = (unsigned char)(current_output & OCTET_MASK);
    if (free_bits < 4)
    {
      *t4_ptr++ = 0;
    }
    *t4_ptr++ = 0x80;
    free_bits = PIXELS_PER_OCTET;
    current_output = 0;
  }
  /* otherwise output the code word in the structure        */
  else
  {
    length = code->code_length;
    current_output |=
      ((unsigned long)code->bit_pattern << length);

    while (length >= free_bits)
    {
      current_output >>= free_bits;
      *t4_ptr++ = (unsigned char)(current_output & OCTET_MASK);
      length -= free_bits;
      free_bits = PIXELS_PER_OCTET;
    }

    if (length != 0)
    {
      current_output >>= length;
      free_bits -= length;
    }
  }
  return t4_ptr;
}

/* Function:    CountPixelRun
 *
 * Remarks:     only referenced from inside this source
 *              file so defined with the static keyword
 *              for internal linkage
 *
 * Inputs:      PIXEL_BITS wanted, indicating whether a run
 *              of black or white pixels is wanted
 *
 *              int maximum, indicating the maximum number
 *              of pixels in the current scan_line
 *
 *              FILE *fin, source file to read additional
 *              binary image pixel octets, as needed
 *
 * Returns:     an int, either 0 through 1728 indicating the
 *              length of the run of the specified color pixel
 *              or EOF if end of file or error encountered
 *
 * Description: this function extracts continuous runs of the
 *              selected pixel color from the input file
 */
static int
CountPixelRun(PIXEL_BITS wanted, int maximum, FILE *fin)
{
  int run_count = 0;
  int input;
  static int raw_pixels = 0;
  static int bits_used = 8;
  static int EOF_flag = 0;

  for ( ; ; )
  {
    if (bits_used >= PIXELS_PER_OCTET)
    {
      if (maximum >= 8)
      {
        if (EOF_flag)
        {
          return EOF;
        }
        else if ((input = fgetc(fin)) == EOF)
        {
          EOF_flag = 1;
          break;
        }
        raw_pixels = input & OCTET_MASK;
        bits_used = 0;
      }
      else
      {
        break;
      }
    }

    if (wanted == (raw_pixels & PIXEL_TEST))
    {
      ++run_count;
      ++bits_used;
      --maximum;
      raw_pixels <<= 1;
    }
    else
    {
       break;
    }
  }

  return run_count;
}

/* Function:    EncodeLine
 *
 * Remarks:     only referenced from inside this source
 *              file so defined with the static keyword
 *              for internal linkage
 *
 * Inputs:      unsigned char *t4_out, pointer to at least
 *              T4_BUFFER_SIZE bytes for holding a full row
 *              of T.4 code words generated from the binary
 *              image pixels
 *
 *              FILE *fin, pointer to the source file
 *              of the T.4 encoded image beind decoded
 *
 * Returns:     int:
 *                0 through 975 indicating the number of
 *                unsigned chars in the buffer holding
 *                the T.4 code words for the scan line, or
 *                the standard macro EOF if any errors occur
 *
 * Description: this function is used to construct T.4 code
 *              representing a single 1,728 pixels per scan line
 */
int
EncodeLine(unsigned char *t4_out, FILE *fin)
{
  int scan_count = PIXELS_PER_ROW;
  int run_count;
  unsigned char *t4_ptr = t4_out;
  PIXEL_BITS color = WHITE_PIXEL;

  while (scan_count > 0)
  {
    run_count = CountPixelRun(color, scan_count, fin);
    if (run_count == EOF)
    {
      return EOF;
    }

    if (color == BLACK_PIXEL)
    {
      if (run_count > 63)
      {
        t4_ptr = OutputCodeWord(
          black_makeup + (run_count / 64) - 1,
          t4_ptr);
      }
      t4_ptr = OutputCodeWord(
        black_terminate + (run_count % 64),
        t4_ptr);
      color = WHITE_PIXEL;
    }
    else
    {
      if (run_count > 63)
      {
        t4_ptr = OutputCodeWord(
          white_makeup + (run_count / 64) - 1,
          t4_ptr);
      }
      t4_ptr = OutputCodeWord(
        white_terminate + (run_count % 64),
        t4_ptr);
      color = BLACK_PIXEL;
    }

    scan_count -= run_count;
  }

  t4_ptr = OutputCodeWord(NULL, t4_ptr);
  return (int)(t4_ptr - t4_out);
}

/* Function:    EncodePage
 *
 * Remarks:     only referenced from inside this source
 *              file so defined with the static keyword
 *              for internal linkage
 *
 * Inputs:      FILE *fin, pointer to the source file
 *              of the T.4 encoded image beind decoded
 *
 *              FILE *fout, pointer to destination file
 *              for the decoded binary image
 *
 *              unsigned char *buff, pointer to at least
 *              T4_BUFFER_SIZE bytes for holding a full row
 *              of T.4 code words generated from the binary
 *              image pixels
 *
 *              char *output_name, a copy of argv[2], which
 *              contains the name of the output file, for use
 *              in messages in the event of error
 *
 * Returns:     int:
 *                0 through MAXIMUM_ROWS indicating the
 *                number of T.4 encoded lines written
 *                to the output file if no errors occurred, or
 *                the standard macro EOF if any errors occur
 *
 * Description: this function is used to construct T.4 code
 *              representing an encoded page image,
 *              consisting of 1,728 pixels per scan line
 *              and up to MAXIMUM_ROWS scan lines per
 *              page
 */
static int
EncodePage(FILE *fin, FILE *fout,
           unsigned char *buff,
           char *output_name)
{
  int size;
  int scan_lines;
  size_t written;

  /* write required initial EOL code word to file */
  if (sizeof EOL != fwrite(EOL, 1, sizeof EOL, fout))
  {
     printf(wr_err, output_name);
     return EOF;
  }

  for (scan_lines = 0;  scan_lines< MAXIMUM_ROWS; )
  {
    size = EncodeLine(buff, fin);

    if (size < 0)
    {
       break;
    }
    ++scan_lines;

    written = fwrite(buff, sizeof *buff, size, fout);
    if ((int)written != size)
    {
      printf(wr_err, output_name);
      return EOF;
    }
  }

  /* write the required six consecutive EOL code words  */
  /* to indicate end of page                            */
  if (sizeof EOP != fwrite(EOP, 1, sizeof EOP, fout))
  {
    printf(wr_err, output_name);
    return EOF;
  }

  return scan_lines;
}

int
main(int argc, char **argv)
{
  FILE *ifile, *ofile;
  unsigned char obuff[T4_BUFFER_SIZE];
  int scan_lines;

  /* check command line arguments and open files  */
  if (argc < 3)
  {
    puts("usage: encode binary-input-file, t4-output-file");
    return EXIT_FAILURE;
  }
  else if ((ifile = fopen(argv[1], "rb")) == NULL)
  {
    printf("encode: can't open %s\n", argv[1]);
    return EXIT_FAILURE;
  }
  else if ((ofile = fopen(argv[2], "wb")) == NULL)
  {
    printf("encode: can't make %s\n", argv[2]);
    fclose(ifile);
    return EXIT_FAILURE;
  }

  scan_lines = EncodePage(ifile, ofile, obuff, argv[2]);

  /* close the files  */
  fclose(ifile);
  fclose(ofile);

  if (scan_lines < 0)
  {
    return EXIT_FAILURE;
  }
  else
  {
    /* display results  */
    printf("encoded %d rows from %s to %s\n",
          scan_lines, argv[1], argv[2]);
    return EXIT_SUCCESS;
  }
}

