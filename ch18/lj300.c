#include <stdio.h>
#include <stdlib.h>
#include "fax.h"

/* define symbolic constants for literal printer   */
/* control strings to make them easy to change for */
/* a different printer                             */

/* this string puts an HP compatible laser printer */
/* into 300 dot per inch raster graphics mode      */
#define GRAPHICS_START    "\x1b""*t300R\x1b""*r0A"

/* this string directs the printer to treat the    */
/* next 216 bytes as bit-mapped raster graphics    */
#define GRAPHICS_LEADER   "\x1b""*b300W"

/* this string directs the printer to exit raster  */
/* graphics mode and includes a final form feed to */
/* print and output the page                       */
#define GRAPHICS_END      "\x1b""*rB\f"

int main(int argc, char **argv)
{
  FILE *input, *output;  /* source and destination files */
  unsigned char binary [OCTETS_PER_ROW];
  unsigned char hpcl [OCTETS_PER_ROW * 2];
  unsigned char *hptr;
  int count = 0, octets;
  unsigned long pattern, current;

  if (argc < 3)
  {
    fputs("usage: lj200 text-file binary-file\n", stderr);
    return EXIT_FAILURE;
  }
  else if (NULL == (input = fopen(argv[1], "rb")))
  {
    fprintf(stderr, "lj200: can't open %s", argv[1]);
    return EXIT_FAILURE;
  }
  else if (NULL == (output = fopen(argv[2], "wb")))
  {
    fprintf(stderr, "t2b: can't make %s", argv[2]);
    fclose(input);
    return EXIT_FAILURE;
  }

  fputs(GRAPHICS_START, output);

  while (OCTETS_PER_ROW ==
         fread(binary, 1, OCTETS_PER_ROW, input))
  {
    ++count;
    hptr = hpcl;

    for (octets = 0; octets < 200; octets += 2)
    {
      pattern = 0;
      current = binary[octets] ^ 0xff;
      if (current & 0x80)
        pattern |= 0xc00000;
      if (current & 0x40)
        pattern |= 0x600000;
      if (current & 0x20)
        pattern |= 0x180000;
      if (current & 0x10)
        pattern |= 0x0c0000;
      if (current & 0x08)
        pattern |= 0x030000;
      if (current & 0x04)
        pattern |= 0x018000;
      if (current & 0x02)
        pattern |= 0x006000;
      if (current & 0x01)
        pattern |= 0x003000;

      current = binary[octets + 1] ^ 0xff;
      if (current & 0x80)
        pattern |= 0x000c00;
      if (current & 0x40)
        pattern |= 0x000600;
      if (current & 0x20)
        pattern |= 0x000180;
      if (current & 0x10)
        pattern |= 0x0000c0;
      if (current & 0x08)
        pattern |= 0x000030;
      if (current & 0x04)
        pattern |= 0x000018;
      if (current & 0x02)
        pattern |= 0x000006;
      if (current & 0x01)
        pattern |= 0x000003;

     *hptr++ = (unsigned char)((pattern >> 16) & 0xff);
     *hptr++ = (unsigned char)((pattern >>  8) & 0xff);
     *hptr++ = (unsigned char)( pattern        & 0xff);
    }

    fputs(GRAPHICS_LEADER, output);
    fwrite(hpcl, 1, 300, output);
    fputs(GRAPHICS_LEADER, output);
    fwrite(hpcl, 1, 300, output);
    fputs(GRAPHICS_LEADER, output);
    fwrite(hpcl, 1, 300, output);

  }
  fputs(GRAPHICS_END, output);
  fclose(output);
  fprintf(stderr, "lj200: converted %d lines\n", count);
  return EXIT_SUCCESS;
}
