/*  decode.c - contains code to decode Group 3 Facsimilie T.4
 *             encoding back to binary pixel image data
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
#include <stdlib.h>
#include "fax.h"

/* the T4_DECODE structure is the data type used in the
 * look up tables for matchine T.4 code words from the
 * input bit stream
 */
typedef struct
{
  unsigned char bit_pattern;
  unsigned char run_length;
} T4_DECODE;

static const T4_DECODE t4_white_04[] =
{
  { 0x07, 0x02 }, /*    2 white pixels */
  { 0x08, 0x03 }, /*    3 white pixels */
  { 0x0B, 0x04 }, /*    4 white pixels */
  { 0x0C, 0x05 }, /*    5 white pixels */
  { 0x0E, 0x06 }, /*    6 white pixels */
  { 0x0F, 0x07 }  /*    7 white pixels */
};

static const T4_DECODE t4_white_05[] =
{
  { 0x07, 0x0A }, /*   10 white pixels */
  { 0x08, 0x0B }, /*   11 white pixels */
  { 0x12, 0x82 }, /*  128 white pixels */
  { 0x13, 0x08 }, /*    8 white pixels */
  { 0x14, 0x09 }, /*    9 white pixels */
  { 0x1B, 0x81 }  /*   64 white pixels */
};

static const T4_DECODE t4_white_06[] =
{
  { 0x03, 0x0D }, /*   13 white pixels */
  { 0x07, 0x01 }, /*    1 white pixels */
  { 0x08, 0x0C }, /*   12 white pixels */
  { 0x17, 0x83 }, /*  192 white pixels */
  { 0x18, 0x9A }, /* 1664 white pixels */
  { 0x2A, 0x10 }, /*   16 white pixels */
  { 0x2B, 0x11 }, /*   17 white pixels */
  { 0x34, 0x0E }, /*   14 white pixels */
  { 0x35, 0x0F }  /*   15 white pixels */
};

static const T4_DECODE t4_white_07[] =
{
  { 0x03, 0x16 }, /*   22 white pixels */
  { 0x04, 0x17 }, /*   23 white pixels */
  { 0x08, 0x14 }, /*   20 white pixels */
  { 0x0C, 0x13 }, /*   19 white pixels */
  { 0x13, 0x1A }, /*   26 white pixels */
  { 0x17, 0x15 }, /*   21 white pixels */
  { 0x18, 0x1C }, /*   28 white pixels */
  { 0x24, 0x1B }, /*   27 white pixels */
  { 0x27, 0x12 }, /*   18 white pixels */
  { 0x28, 0x18 }, /*   24 white pixels */
  { 0x2B, 0x19 }, /*   25 white pixels */
  { 0x37, 0x84 }  /*  256 white pixels */
};

static const T4_DECODE t4_white_08[] =
{
  { 0x02, 0x1D }, /*   29 white pixels */
  { 0x03, 0x1E }, /*   30 white pixels */
  { 0x04, 0x2D }, /*   45 white pixels */
  { 0x05, 0x2E }, /*   46 white pixels */
  { 0x0A, 0x2F }, /*   47 white pixels */
  { 0x0B, 0x30 }, /*   48 white pixels */
  { 0x12, 0x21 }, /*   33 white pixels */
  { 0x13, 0x22 }, /*   34 white pixels */
  { 0x14, 0x23 }, /*   35 white pixels */
  { 0x15, 0x24 }, /*   36 white pixels */
  { 0x16, 0x25 }, /*   37 white pixels */
  { 0x17, 0x26 }, /*   38 white pixels */
  { 0x1A, 0x1F }, /*   31 white pixels */
  { 0x1B, 0x20 }, /*   32 white pixels */
  { 0x24, 0x35 }, /*   53 white pixels */
  { 0x25, 0x36 }, /*   54 white pixels */
  { 0x28, 0x27 }, /*   39 white pixels */
  { 0x29, 0x28 }, /*   40 white pixels */
  { 0x2A, 0x29 }, /*   41 white pixels */
  { 0x2B, 0x2A }, /*   42 white pixels */
  { 0x2C, 0x2B }, /*   43 white pixels */
  { 0x2D, 0x2C }, /*   44 white pixels */
  { 0x32, 0x3D }, /*   61 white pixels */
  { 0x33, 0x3E }, /*   62 white pixels */
  { 0x34, 0x3F }, /*   63 white pixels */
  { 0x35, 0x00 }, /*    0 white pixels */
  { 0x36, 0x85 }, /*  320 white pixels */
  { 0x37, 0x86 }, /*  384 white pixels */
  { 0x4A, 0x3B }, /*   59 white pixels */
  { 0x4B, 0x3C }, /*   60 white pixels */
  { 0x52, 0x31 }, /*   49 white pixels */
  { 0x53, 0x32 }, /*   50 white pixels */
  { 0x54, 0x33 }, /*   51 white pixels */
  { 0x55, 0x34 }, /*   52 white pixels */
  { 0x58, 0x37 }, /*   55 white pixels */
  { 0x59, 0x38 }, /*   56 white pixels */
  { 0x5A, 0x39 }, /*   57 white pixels */
  { 0x5B, 0x3A }, /*   58 white pixels */
  { 0x64, 0x87 }, /*  448 white pixels */
  { 0x65, 0x88 }, /*  512 white pixels */
  { 0x67, 0x8A }, /*  640 white pixels */
  { 0x68, 0x89 }  /*  576 white pixels */
};

static const T4_DECODE t4_white_09[] =
{
  { 0x98, 0x97 }, /* 1472 white pixels */
  { 0x99, 0x98 }, /* 1536 white pixels */
  { 0x9A, 0x99 }, /* 1600 white pixels */
  { 0x9B, 0x9B }, /* 1728 white pixels */
  { 0xCC, 0x8B }, /*  704 white pixels */
  { 0xCD, 0x8C }, /*  768 white pixels */
  { 0xD2, 0x8D }, /*  832 white pixels */
  { 0xD3, 0x8E }, /*  896 white pixels */
  { 0xD4, 0x8F }, /*  960 white pixels */
  { 0xD5, 0x90 }, /* 1024 white pixels */
  { 0xD6, 0x91 }, /* 1088 white pixels */
  { 0xD7, 0x92 }, /* 1152 white pixels */
  { 0xD8, 0x93 }, /* 1216 white pixels */
  { 0xD9, 0x94 }, /* 1280 white pixels */
  { 0xDA, 0x95 }, /* 1344 white pixels */
  { 0xDB, 0x96 }  /* 1408 white pixels */
};

static const T4_DECODE t4_black_02[] =
{
  { 0x02, 0x03 }, /*    3 black pixels */
  { 0x03, 0x02 }  /*    2 black pixels */
};

static const T4_DECODE t4_black_03[] =
{
  { 0x02, 0x01 }, /*   1 black pixels */
  { 0x03, 0x04 }  /*   4 black pixels */
};

static const T4_DECODE t4_black_04[] =
{
  { 0x02, 0x06 }, /*    6 black pixels */
  { 0x03, 0x05 }  /*    5 black pixels */
};

static const T4_DECODE t4_black_05[] =
{
  { 0x03, 0x07 }  /*    7 black pixels */
};

static const T4_DECODE t4_black_06[] =
{
  { 0x04, 0x09 }, /*    9 black pixels */
  { 0x05, 0x08 }  /*    8 black pixels */
};

static const T4_DECODE t4_black_07[] =
{
  { 0x04, 0x0A }, /*   10 black pixels */
  { 0x05, 0x0B }, /*   11 black pixels */
  { 0x07, 0x0C }  /*   12 black pixels */
};

static const T4_DECODE t4_black_08[] =
{
  { 0x04, 0x0D }, /*   13 black pixels */
  { 0x07, 0x0E }  /*   14 black pixels */
};

static const T4_DECODE t4_black_09[] =
{
  { 0x18, 0x0F }  /*   15 black pixels */
};

static const T4_DECODE t4_black_10[] =
{
  { 0x08, 0x12 }, /*   18 black pixels */
  { 0x0F, 0x81 }, /*   64 black pixels */
  { 0x17, 0x10 }, /*   16 black pixels */
  { 0x18, 0x11 }, /*   17 black pixels */
  { 0x37, 0x00 }  /*    0 black pixels */
};

static const T4_DECODE  t4_black_11[] =
{
  { 0x17, 0x18 }, /*   24 black pixels */
  { 0x18, 0x19 }, /*   25 black pixels */
  { 0x28, 0x17 }, /*   23 black pixels */
  { 0x37, 0x16 }, /*   22 black pixels */
  { 0x67, 0x13 }, /*   19 black pixels */
  { 0x68, 0x14 }, /*   20 black pixels */
  { 0x6C, 0x15 }  /*   21 black pixels */
};

static const T4_DECODE t4_black_12[] =
{
  { 0x24, 0x34 }, /*   52 black pixels */
  { 0x27, 0x37 }, /*   55 black pixels */
  { 0x28, 0x38 }, /*   56 black pixels */
  { 0x2B, 0x3B }, /*   59 black pixels */
  { 0x2C, 0x3C }, /*   60 black pixels */
  { 0x33, 0x85 }, /*  320 black pixels */
  { 0x34, 0x86 }, /*  384 black pixels */
  { 0x35, 0x87 }, /*  448 black pixels */
  { 0x37, 0x35 }, /*   53 black pixels */
  { 0x38, 0x36 }, /*   54 black pixels */
  { 0x52, 0x32 }, /*   50 black pixels */
  { 0x53, 0x33 }, /*   51 black pixels */
  { 0x54, 0x2C }, /*   44 black pixels */
  { 0x55, 0x2D }, /*   45 black pixels */
  { 0x56, 0x2E }, /*   46 black pixels */
  { 0x57, 0x2F }, /*   47 black pixels */
  { 0x58, 0x39 }, /*   57 black pixels */
  { 0x59, 0x3A }, /*   58 black pixels */
  { 0x5A, 0x3D }, /*   61 black pixels */
  { 0x5B, 0x84 }, /*  256 black pixels */
  { 0x64, 0x30 }, /*   48 black pixels */
  { 0x65, 0x31 }, /*   49 black pixels */
  { 0x66, 0x3E }, /*   62 black pixels */
  { 0x67, 0x3F }, /*   63 black pixels */
  { 0x68, 0x1E }, /*   30 black pixels */
  { 0x69, 0x1F }, /*   31 black pixels */
  { 0x6A, 0x20 }, /*   32 black pixels */
  { 0x6B, 0x21 }, /*   33 black pixels */
  { 0x6C, 0x28 }, /*   40 black pixels */
  { 0x6D, 0x29 }, /*   41 black pixels */
  { 0xC8, 0x82 }, /*  128 black pixels */
  { 0xC9, 0x83 }, /*  192 black pixels */
  { 0xCA, 0x1A }, /*   26 black pixels */
  { 0xCB, 0x1B }, /*   27 black pixels */
  { 0xCC, 0x1C }, /*   28 black pixels */
  { 0xCD, 0x1D }, /*   29 black pixels */
  { 0xD2, 0x22 }, /*   34 black pixels */
  { 0xD3, 0x23 }, /*   35 black pixels */
  { 0xD4, 0x24 }, /*   36 black pixels */
  { 0xD5, 0x25 }, /*   37 black pixels */
  { 0xD6, 0x26 }, /*   38 black pixels */
  { 0xD7, 0x27 }, /*   39 black pixels */
  { 0xDA, 0x2A }, /*   42 black pixels */
  { 0xDB, 0x2B }  /*   43 black pixels */
};

static const T4_DECODE t4_black_13[] =
{
  { 0x4A, 0x8A }, /*  640 black pixels */
  { 0x4B, 0x8B }, /*  704 black pixels */
  { 0x4C, 0x8C }, /*  768 black pixels */
  { 0x4D, 0x8D }, /*  832 black pixels */
  { 0x52, 0x94 }, /* 1280 black pixels */
  { 0x53, 0x95 }, /* 1344 black pixels */
  { 0x54, 0x96 }, /* 1408 black pixels */
  { 0x55, 0x97 }, /* 1472 black pixels */
  { 0x5A, 0x98 }, /* 1536 black pixels */
  { 0x5B, 0x99 }, /* 1600 black pixels */
  { 0x64, 0x9A }, /* 1664 black pixels */
  { 0x65, 0x9B }, /* 1728 black pixels */
  { 0x6C, 0x88 }, /*  512 black pixels */
  { 0x6D, 0x89 }, /*  576 black pixels */
  { 0x72, 0x8E }, /*  896 black pixels */
  { 0x73, 0x8F }, /*  960 black pixels */
  { 0x74, 0x90 }, /* 1024 black pixels */
  { 0x75, 0x91 }, /* 1088 black pixels */
  { 0x76, 0x92 }, /* 1152 black pixels */
  { 0x77, 0x93 }  /* 1216 black pixels */
};

/* the CODE_TABLE structure designates arrays of
 * T4_DECODE structures for each possible combination
 * of pixel color and code word length
 *
 * each structure contains a pointer to an array of
 * T4_DECODE structures and the length of the array
 */
typedef struct
{
  const T4_DECODE *token;
  int search_length;
} CODE_TABLE;

/* the two dimensional array of CODE_TABLE structures
 * is used in the actual decoding process, there are two
 * elements, one for black, the other for white, for each
 * possible code word length, 2 through 13 inclusive,
 * for a total of 12 possible code word lengths
 *
 * since the range of white code word lengths is only
 * 4 through 9 inclusive, the entries corresponding
 * to white code word lengths 2 and 3, and 10 through
 * 13, contain a null pointer and a search_length
 * parameter of 0
 */

static const CODE_TABLE code_table[12][2] =
{
  {
    { t4_black_02, sizeof t4_black_02/sizeof *t4_black_02 },
    { NULL       , 0                                      }
  },

  {
    { t4_black_03, sizeof t4_black_03/sizeof *t4_black_03 },
    { NULL       , 0                                      }
  },

  {
    { t4_black_04, sizeof t4_black_03/sizeof *t4_black_03 },
    { t4_white_04, sizeof t4_white_04/sizeof *t4_white_04 }
  },

  {
    { t4_black_05, sizeof t4_black_05/sizeof *t4_black_05 },
    { t4_white_05, sizeof t4_white_05/sizeof *t4_white_05 }
  },

  {
    { t4_black_06, sizeof t4_black_06/sizeof *t4_black_06 },
    { t4_white_06, sizeof t4_white_06/sizeof *t4_white_06 }
  },

  {
    { t4_black_07, sizeof t4_black_07/sizeof *t4_black_07 },
    { t4_white_07, sizeof t4_white_07/sizeof *t4_white_07 }
  },

  {
    { t4_black_08, sizeof t4_black_08/sizeof *t4_black_08 },
    { t4_white_08, sizeof t4_white_08/sizeof *t4_white_08 }
  },

  {
    { t4_black_09, sizeof t4_black_09/sizeof *t4_black_09 },
    { t4_white_09, sizeof t4_white_09/sizeof *t4_white_09 }
  },

  {
    { t4_black_10, sizeof t4_black_10/sizeof *t4_black_10 },
    { NULL       , 0                                      }
  },

  {
    { t4_black_11, sizeof t4_black_11/sizeof *t4_black_11 },
    { NULL       , 0                                      }
  },

  {
    { t4_black_12, sizeof t4_black_12/sizeof *t4_black_12 },
    { NULL       , 0                                      }
  },

  {
    { t4_black_13, sizeof t4_black_13/sizeof *t4_black_13 },
    { NULL       , 0                                      }
  }
};

/* the PIXEL_WANTED enumeration is passed to the code
 * word search and pixel output routines to specify the
 * current pixel color
 */
typedef enum
{
  BLACK_WANTED,
  WHITE_WANTED
} PIXEL_WANTED;

/* the T4_RESULTS enumeration is used to define special
 * return codes for the GetPixelRun() function
 *
 * all special return values are negative numbers, as 0
 * and positive values returned from this function represent
 * pixel runs
 */
typedef enum
{
  T4_EOF          = -1, /* end of input file encountered  */
  T4_EOL          = -2, /* end of line code word found    */
  T4_INVALID      = -3  /* invalid T.4 valid code word    */
} T4_RESULTS;

/* Function:    T4Compare
 *
 * Remarks:     only referenced from inside this source
 *              file so defined with the static keyword
 *              for internal linkage
 *
 * Inputs:      const void *x, an alias for a pointer
 *              to an unsigned int containing the numeric
 *              value of the code word under construction
 *
 *              const void *y, an alias for a pointer to
 *              a T4_DECODE structure
 *
 * Returns:     1 if the code word is greater numerically
 *              than the bit_pattern member of the
 *              T4_DECODE structure, -1 if the code word
 *              is less numerically, and 0 if the code word
 *              and the bit_pattern member of the T4_DECODE
 *              structure are numerically equal
 *
 * Description: this function is designed to be passed as
 *              the comparison function pointer argument to
 *              the standard library function bsearch
 *              prototyped in <stdlib.h>
 *
 * Note:        since both the int and the bit_pattern
 *              structure member are unsigned types, they
 *              are cast to signed ints prior to the
 *              subtraction that generates the return
 *              value
 *
 *              this cast is safe even if the value of
 *              UCHAR_MAX is greater than that of INT_MAX
 *              on an implementation because we know that
 *              the maximum values ever held in the unsigned
 *              types will never be greater than 255, well
 *              within the range of INT_MAX
 *
 *              if this cast were not possible at least two
 *              comparisons between the unsigned values
 *              would need to be made to determine the
 *              functions return value because subtraction
 *              of two unsigned integral types can never
 *              yield a negative result
 */
static int T4Compare(const void *x, const void *y)
{
  return *(int *)x - (int)((T4_DECODE *)y)->bit_pattern;
}

/* Function:    OutputPixels
 *
 * Remarks:     only referenced from inside this source
 *              file so defined with the static keyword
 *              for internal linkage
 *
 * Inputs:      int length, the number of pixels to insert
 *              into the output buffer
 *
 *              PIXEL_WANTED specifying the color of pixels
 *              to be inserted
 *
 *              unsigned char *out_ptr, where octets
 *              containing the generated image bits are
 *              to be stored
 *
 * Returns:     pointer to unsigned char where the next
 *              octet to be generated will be stored
 *
 * Description: this function is used to build the binary
 *              output image of arbitrary length sequences
 *              of white and black pixels
 *
 * Note:        this function does absolutely nothing, and
 *              returns the out_ptr value unchanged, if
 *              called with a length of 0 for either color
 *              pixel so no special test for terminating
 *              code words of 0 length is required by the
 *              caller
 */
static unsigned char
*OutputPixels(int length,
              PIXEL_WANTED wanted,
              unsigned char *out_ptr)
{
  unsigned int mask;
  static int outbits_left = PIXELS_PER_OCTET;
  static unsigned int pixel_out = 0;

  if (BLACK_WANTED == wanted)
  {
    mask = 0;
  }
  else
  {
    mask = 0xff;
  }

  while (length >= outbits_left)
  {
    pixel_out <<= outbits_left;
    *out_ptr++ = (unsigned char)(pixel_out
      | (mask >> (PIXELS_PER_OCTET - outbits_left)));
    pixel_out = 0;
    length -= outbits_left;
    outbits_left = PIXELS_PER_OCTET;
  }

  if (length)
  {
    pixel_out <<= length;
    pixel_out |=
      mask >> (PIXELS_PER_OCTET - length);
    outbits_left -= length;
  }

  return out_ptr;
}

/* Function:    GetNextBit
 *
 * Remarks:     only referenced from inside this source
 *              file so defined with the static keyword
 *              for internal linkage
 *
 * Inputs:      FILE *fin, pointer to the source file
 *              of the T.4 encoded image beind decoded
 *
 * Returns:     int:
 *                0 if the next bit from the source file
 *                is 0, 1 if the next bit is 1, EOF if
 *                end of file or error is encountered
 *                reading from fin
 *
 * Description: this function is used to extract bits one
 *              time from the input file
 *
 * Note:        the required left-to-right reversal is
 *              performed by this function
 */
static int
GetNextBit(FILE *fin)
{
  static int bits_used = PIXELS_PER_OCTET;
  static unsigned int t4_in;
  int input;

  /* see if there are bits remaining in the current octet */
  if (bits_used >= PIXELS_PER_OCTET)
  {
    /* none left, get a new octet from the source file    */
    if ((input = fgetc(fin)) == EOF)
    {
      return T4_EOF;
    }
    else
    {
      /* have new octet, mask to 8 least significant bits */
      t4_in = input & OCTET_MASK;
      /* haven't use any bits from this octet yet         */
      bits_used = 0;
    }
  }
  else
  {
    /* more bits in current octet, shift the last one     */
    /* off the right end and bring the next one to bit 0  */
    t4_in >>= 1;
  }

  ++bits_used;                  /* using a bit, count it  */
  return t4_in & 1;             /* return bit 0 of octet  */
}

/* Function:    GetPixelRun
 *
 * Remarks:     only referenced from inside this source
 *              file so defined with the static keyword
 *              for internal linkage
 *
 * Inputs:      PIXEL_WANTED wanted to specify the color,
 *              black or white, to be searched for
 *
 *              FILE *fin, pointer to the source file
 *              of the T.4 encoded image beind decoded
 *
 * Returns:     int:
 *                0 through 1728 representing the length
 *                of the run of pixels of the specified
 *                color, if found, or one of the special
 *                codes from the T4_RESULTS enumeration:
 *
 *                T4_EOF if GetNextPixel encounters an
 *                error or the end of the input file
 *
 *                T4_EOL if the end of line code word is
 *                found instead of a code word for a pixel
 *                run of the specified color
 *
 *                T4_INVALID if the sequence of bits
 *                retrieved from the input file does not
 *                represent a valid T.4 code word
 *
 * Description: this function is used to extract T.4 code
 *              representing pixel runs, or the EOL code
 *              word
 */
static int
GetPixelRun(PIXEL_WANTED color, FILE *fin)
{
  unsigned int code_word = 0;
  int bits = 0;
  int pixel_run, next_bit, min_bits, max_bits;
  const T4_DECODE *t4p;

  /* treat wanted as a Boolean, BLACK_WANTED indicates    */
  /* black, any other value indicates white               */
  if (BLACK_WANTED == color)
  {
    min_bits = 2;     /* minimum length black code word   */
    max_bits = 13;    /* maximum length black code word   */
  }
  else  /* WHITE_PIXEL wanted */
  {
    color = WHITE_WANTED;
    min_bits = 4;     /* minimum length white code word   */
    max_bits = 9;     /* maximum length white code word   */
  }

  for ( ; ; )         /* until code word found or error   */
  {
    do
    {
      /* because a do..while loop has the test at the end */
      /* it always executes at least once so on each pass */
      /* the GetNextBit() function will always be called  */
      /* even if the length of the code word under        */
      /* construction is greater than the minimum value   */
      if ((next_bit = GetNextBit(fin)) == T4_EOF)
      {
        return T4_EOF;
      }
      else
      {
        code_word = (code_word << 1) | next_bit;
      }
    } while (++bits < min_bits);

    /*  check for EOL once code word is long enough */
    if (bits >= EOL_LENGTH && code_word == 1)
    {
      return T4_EOL;
    }

    /* if already past maximum bit length and not   */
    /* EOL must be all zero bits on the way to EOL  */
    if (bits > max_bits)
    {
      if (code_word != 0)
      {
        return T4_INVALID;
      }
    }
    else if (NULL != (t4p =
      code_table[bits - 2][color].token))
    /* this condition has to be in an else if clause to the */
    /* one above it because if bits > max_bits the access   */
    /* to the code_table array will be beyond the end of    */
    /* array causing undefined behavior                     */
    {
      t4p = bsearch(&code_word, t4p,
            code_table[bits - 2][color].search_length,
            sizeof *t4p, T4Compare);

      if (NULL != t4p)
      {
        pixel_run = t4p->run_length;
        /* here the packing of make-up codes into unsigned  */
        /* chars is undone and the run length expanded back */
        /* to its full value                                */
        if (pixel_run & 0x80)
        {
          pixel_run &= ~0x80;
          pixel_run <<= 6;
        }
        return pixel_run;
      }
    }
  }
}

/* Function:    DecodePage
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
 *              unsigned char *buff, pointer to an array of
 *              at least OCTETS_PER_ROW bytes to store a
 *              full scan row of decoded pixels
 *
 * Returns:     int:
 *                0 through MAXIMUM_ROWS indicating the
 *                number of binary pixel scan lines written
 *                to the output file if no errors occurred,
 *                the standard macro EOF if any errors
 *                do occur
 *
 * Description: this function is used to extract T.4 code
 *              representing an encoded page image,
 *              consisting of 1,728 pixels per scan line
 *              and up to MAXIMUM_ROWS scan lines per
 *              page
 */
int
DecodePage(FILE *fin,
           FILE *fout,
           unsigned char * const buff)
{
  PIXEL_WANTED wanted;    /* color currently sought       */
  int pixel_run;          /* length of current color run  */
  int eop_count = 0;      /* use to recognize end of page */
  int fax_lines;          /* count of lines decoded       */
  int pixel_count;        /* total pixels in current line */
  int total_run;          /* total pixels in current run  */
  unsigned char *out_ptr; /* pointer into output buffer   */

  /* first code word in the file must be an EOL */
  pixel_run = GetPixelRun(WHITE_WANTED, fin);
  if (pixel_run != T4_EOL)
  {
    puts("missing initial EOL");
    return EOF;
  }

  /* read, decode, and output encoded scan lines one-by-one */
  for (fax_lines = 0; fax_lines < MAXIMUM_ROWS; )
  {
    wanted = WHITE_WANTED;  /* lines start with white runs  */
    out_ptr = buff;         /* output starts at beginning   */
    pixel_count = 0;        /* have no pixels in new line   */

    do
    {
      pixel_run = GetPixelRun(wanted, fin);
      if (pixel_run >= 0)
      {
        eop_count = 0;      /* pixel runs since last EOL    */
        if ((total_run = pixel_run) > 63)
        {
          /* if the pixel run just decodes is greater than  */
          /* 63, it is a make-up code and they are always   */
          /* followed by a terminating code for the same    */
          /* color, so call GetPixelRun again with the same */
          /* color to get the total run length              */
          pixel_run = GetPixelRun(wanted, fin);
          if (pixel_run >= 0)
          {
            total_run += pixel_run;
          }
          else
          {
            puts("decode: make-up code missing");
            return EOF;
          }
        }

        /* before inserting the new run of pixels into the  */
        /* output buffer, check to make sure that it will   */
        /* not exceed the proper number of pixels per row   */
        /* as this could cause writing past the end of the  */
        /* memory space belonging to the buffer, causing    */
        /* undefined behavior                               */
        if ((pixel_count += total_run) > PIXELS_PER_ROW)
        {
          puts("decode: line too long");
          return EOF;
        }
        else
        {
          /* the new run of pixels will fit in the buffer   */
          /* so insert it                                   */
          out_ptr = OutputPixels(total_run, wanted, out_ptr);
          /* since white and black pixel runs alternate we  */
          /* now want to look for the opposite color of the */
          /* last run                                       */
          if (wanted == WHITE_WANTED)
          {
            wanted = BLACK_WANTED;
          }
          else
          {
            wanted = WHITE_WANTED;
          }
        }
      }
    } while (pixel_run >= 0);

    /* a value which does not represent a pixel run has     */
    /* been returned by GetPixelRun(), decided what to do   */
    /* next based on its exact value                        */
    switch (pixel_run)
    {
      case T4_EOF:
        puts("decode: unexpected end of file");
        return EOF;
      case T4_EOL:
        /* there are two correct circumstances for finding  */
        /* an EOL code word, the first after decoding the   */
        /* code words for exactly the 1728 pixels...        */
        if (PIXELS_PER_ROW == pixel_count)
        {
          ++fax_lines;
          fwrite(buff, 1, OCTETS_PER_ROW, fout);
          ++eop_count;
        }
        /* ...and the second is after decoding 0 pixels     */
        /* after the preceeding EOL code, since six         */
        /* consecutive EOL codes in a row with no pixels at */
        /* all in between signal the end of the page        */
        else if (0 == pixel_count)
        {
          if (++eop_count >= 6)
          {
            return fax_lines;
          }
        }
        /* if an EOL code word is found after some number   */
        /* of pixels less than 1,728 it is an error         */
        else
        {
          puts("decode: invalid line length");
          return EOF;
        }
        break;
      case T4_INVALID:
        /* if GetPixelRun() detected a pattern of bits that */
        /* don't correspond to any T.4 code word...         */
        puts("decode: invalid t.4 code");
        return EOF;
      default:
        /* for safety sake there is a default case...       */
        puts("decode: program error");
        return EOF;
    }
  }

  return fax_lines;
}

/* Function:    main
 *
 * Inputs:      int argc, specifying number of command line
 *              arguments
 *
 *              char **argv, a pointer to an array of
 *              pointers to strings containing the contents
 *              of the command line arguments
 *
 *              argv[1] must represent the name of an
 *              existing file that can be opened by the
 *              program containing a valid T.4 encoding
 *              of a fax page image
 *
 *              argv[2] must represent a name that the
 *              program can create to contain the decoded
 *              binary image pixels of the page
 *
 * Returns:     int:
 *                EXIT_SUCCESS defined in <stdlib.h> if
 *                appropriate command line arguments are
 *                received and a T.4 encoded input file
 *                is successfully translated to a binary
 *                image output file
 *
 *                EXIT_FAILURE defined in <stdlib.h> if
 *                the command line arguments are incorrect,
 *                or if any error occurs
 *
 * Description: this is the main entry point for the decode
 *              program
 *
 *              it verifies the command line arguments,
 *              opens the source and destination files,
 *              and defines an array of unsigned char to
 *              hold the binary output as each line is
 *              row of pixels is being built
 */
int
main(int argc, char **argv)
{
  int line_count;
  FILE *ifile, *ofile;
  unsigned char obuff[OCTETS_PER_ROW];

  /* check command line arguments and open files  */
  if (argc < 3)
  {
    puts("usage: decode t4-input-file, binary-output-file");
    return EXIT_FAILURE;
  }
  else if ((ifile = fopen(argv[1], "rb")) == NULL)
  {
    printf("decode: can't open %s\n", argv[1]);
    return EXIT_FAILURE;
  }
  else if ((ofile = fopen(argv[2], "wb")) == NULL)
  {
    printf("decode: can't make %s\n", argv[2]);
    fclose(ifile);
    return EXIT_FAILURE;
  }

  line_count = DecodePage(ifile, ofile, obuff);

  if (line_count >= 0)
  {
    printf("decode: %d lines converted\n", line_count);
  }

  fclose(ifile);
  fclose(ofile);
  return EXIT_SUCCESS;
}

