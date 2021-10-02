/*  fax.h - defines constants used in Group 3 Facsimilie
 *          T.4 encoding and decoding
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

#define PIXELS_PER_ROW    1728
#define PIXELS_PER_OCTET  8
#define OCTETS_PER_ROW    (PIXELS_PER_ROW / PIXELS_PER_OCTET)
#define MAXIMUM_ROWS      1024
#define T4_BUFFER_SIZE    1024
#define EOL_LENGTH        12
#define OCTET_MASK        0xff


