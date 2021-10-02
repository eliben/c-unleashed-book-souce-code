/*  hamming.c - contains code for generating and checking
 *              (12,8) Hamming codes
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

const int hamming[256] =
{
  0X00,  0X03,  0X05,  0X06,  0X06,  0X05,  0X03,  0X00,
  0X07,  0X04,  0X02,  0X01,  0X01,  0X02,  0X04,  0X07,
  0X09,  0X0A,  0X0C,  0X0F,  0X0F,  0X0C,  0X0A,  0X09,
  0X0E,  0X0D,  0X0B,  0X08,  0X08,  0X0B,  0X0D,  0X0E,
  0X0A,  0X09,  0X0F,  0X0C,  0X0C,  0X0F,  0X09,  0X0A,
  0X0D,  0X0E,  0X08,  0X0B,  0X0B,  0X08,  0X0E,  0X0D,
  0X03,  0X00,  0X06,  0X05,  0X05,  0X06,  0X00,  0X03,
  0X04,  0X07,  0X01,  0X02,  0X02,  0X01,  0X07,  0X04,
  0X0B,  0X08,  0X0E,  0X0D,  0X0D,  0X0E,  0X08,  0X0B,
  0X0C,  0X0F,  0X09,  0X0A,  0X0A,  0X09,  0X0F,  0X0C,
  0X02,  0X01,  0X07,  0X04,  0X04,  0X07,  0X01,  0X02,
  0X05,  0X06,  0X00,  0X03,  0X03,  0X00,  0X06,  0X05,
  0X01,  0X02,  0X04,  0X07,  0X07,  0X04,  0X02,  0X01,
  0X06,  0X05,  0X03,  0X00,  0X00,  0X03,  0X05,  0X06,
  0X08,  0X0B,  0X0D,  0X0E,  0X0E,  0X0D,  0X0B,  0X08,
  0X0F,  0X0C,  0X0A,  0X09,  0X09,  0X0A,  0X0C,  0X0F,
  0X0C,  0X0F,  0X09,  0X0A,  0X0A,  0X09,  0X0F,  0X0C,
  0X0B,  0X08,  0X0E,  0X0D,  0X0D,  0X0E,  0X08,  0X0B,
  0X05,  0X06,  0X00,  0X03,  0X03,  0X00,  0X06,  0X05,
  0X02,  0X01,  0X07,  0X04,  0X04,  0X07,  0X01,  0X02,
  0X06,  0X05,  0X03,  0X00,  0X00,  0X03,  0X05,  0X06,
  0X01,  0X02,  0X04,  0X07,  0X07,  0X04,  0X02,  0X01,
  0X0F,  0X0C,  0X0A,  0X09,  0X09,  0X0A,  0X0C,  0X0F,
  0X08,  0X0B,  0X0D,  0X0E,  0X0E,  0X0D,  0X0B,  0X08,
  0X07,  0X04,  0X02,  0X01,  0X01,  0X02,  0X04,  0X07,
  0X00,  0X03,  0X05,  0X06,  0X06,  0X05,  0X03,  0X00,
  0X0E,  0X0D,  0X0B,  0X08,  0X08,  0X0B,  0X0D,  0X0E,
  0X09,  0X0A,  0X0C,  0X0F,  0X0F,  0X0C,  0X0A,  0X09,
  0X0D,  0X0E,  0X08,  0X0B,  0X0B,  0X08,  0X0E,  0X0D,
  0X0A,  0X09,  0X0F,  0X0C,  0X0C,  0X0F,  0X09,  0X0A,
  0X04,  0X07,  0X01,  0X02,  0X02,  0X01,  0X07,  0X04,
  0X03,  0X00,  0X06,  0X05,  0X05,  0X06,  0X00,  0X03,
};

const int ham_pos_values [8] =
{ 3, 5, 6, 7, 9, 10, 11, 12 };

unsigned int corrections [16] =
{
  0x00,   /* 0 result means no error  */
  0x00,   /*  C1 is a Hamming bit, not a data bit */
  0x00,   /*  C2 is a Hamming bit, not a data bit */
  0x01,   /*  C3 is data bit D0                   */
  0x00,   /*  C4 is a Hamming bit, not a data bit */
  0x02,   /*  C5 is data bit D1                   */
  0x04,   /*  C6 is data bit D2                   */
  0x08,   /*  C7 is data bit D3                   */
  0x00,   /*  C8 is a Hamming bit, not a data bit */
  0x10,   /*  C9 is data bit D4                   */
  0x20,   /* C10 is data bit D5                   */
  0x40,   /* C11 is data bit D6                   */
  0x80,   /* C12 is data bit D7                   */
};

int HammingBitwise(int value)
{
  int result = 0;

  /* ensure that value is in the allowable  */
  /* range of 0 through 255                 */
  value &= 0xff;

  /* exclusive OR data bits 0, 1, 3, 4, & 5 */
  /* into check bit 0                       */
	result ^= ((value     ) & 1);	/* D0 to D0	*/
	result ^= ((value >> 1) & 1);	/* D1 to D0	*/
	result ^= ((value >> 3) & 1);	/* D3 to D0	*/
	result ^= ((value >> 4) & 1);	/* D4 to D0	*/
	result ^= ((value >> 6) & 1);	/* D6 to D0	*/

  /* exclusive OR data bits 0, 2, 3, 5, & 6 */
  /* into check bit 1                       */
	result ^= ((value << 1) & 2);	/* D0 to D1	*/
	result ^= ((value >> 1) & 2);	/* D2 to D1	*/
	result ^= ((value >> 2) & 2);	/* D3 to D1 */
	result ^= ((value >> 4) & 2);	/* D5 to D1	*/
	result ^= ((value >> 5) & 2);	/* D6 to D1	*/

  /* exclusive OR data bits 1, 2, 3, & 7    */
  /* into check bit 2                       */
	result ^= ((value << 1) & 4);	/* D1 to D2	*/
	result ^= ((value     ) & 4);	/* D2 to D2	*/
	result ^= ((value >> 1) & 4);	/* D3 to D2	*/
	result ^= ((value >> 5) & 4);	/* D7 to D2	*/

  /* exclusive OR data bits 4, 5, 6, & 7    */
  /* into check bit 3                       */
	result ^= ((value >> 1) & 8);	/* D4 to D3	*/
	result ^= ((value >> 2) & 8);	/* D5 to D3	*/
	result ^= ((value >> 3) & 8);	/* D6 to D3	*/
	result ^= ((value >> 4) & 8);	/* D7 to D3	*/

	return result;
}

int HammingPosition(int value)
{
  int result = 0;
  int counter;

  /* ensure that value is in the allowable  */
  /* range of 0 through 255                 */
  value &= 0xff;

  /* loop through the bits, exclusive OR in */
  /* the value for the one bits             */
  for (counter = 0; counter < 8; ++counter)
  {
    if ((value & (1 << counter)) != 0)
    {
      result ^= ham_pos_values[counter];
    }
  }
  return result;
}

int main(void)
{
  unsigned long value;
  unsigned long modified;
  long the_bit;
  int ham;
  int mod_ham;
  char buff[50];

  for ( ; ; )
  {
    printf("Enter a value between 0 and 255 in C notation: ");
    fflush(stdout);
    if (NULL == (fgets(buff, sizeof buff, stdin)))
    {
      break;
    }
    else if (*buff == '\n')
    {
      break;
    }
    value = strtoul(buff, NULL, 0);
    if (value > 255)
    {
      puts("Value too large");
      continue;
    }
    printf("Enter the bit (0 - 7) to change: ");
    fflush(stdout);
    if (NULL == (fgets(buff, sizeof buff, stdin)))
    {
      break;
    }
    else if (*buff == '\n')
    {
      break;
    }
    the_bit = strtol(buff, NULL, 0);
    if (the_bit > 7 || the_bit < 0)
    {
      puts("Bit number out of range");
      continue;
    }
    ham = hamming[value];
    modified = value ^ (1 << the_bit);
    mod_ham = hamming[modified];
    printf("Original value 0x%02X Hamming Code 0x%X\n",
            value, ham);
    printf("Modified value 0x%02X Hamming Code 0x%X\n",
            modified, mod_ham);
    printf("Exclusive OR of the Hamming Codes is 0x%X\n",
            ham ^ mod_ham);
    printf("Correction mask is 0x%02X\n",
            corrections[ham ^ mod_ham]);
    printf("Corrected value is 0x%02X\n\n",
            modified ^ corrections[ham ^ mod_ham]);
  }
  printf("\nGoodbye\n");
  return 0;
}

