/*  strarr.h - header for string array demo
 *
 *  STRARR - dynamic allocation of arrays of strings
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

#ifndef STRARR_H__
#define STRARR_H__

void FreeStrArray(char **Array, size_t NumRows);
char **AllocStrArray(size_t NumRows, size_t Width);
int ResizeOneString(char **Array,
                    size_t Row,
                    size_t NewSize);
int AddRowsToStrArray(char ***ArrayPtr,
                      size_t OldNumRows,
                      int NumRowsToAdd,
                      size_t InitWidth);
int ConsolidateStrArray(char **ArrayPtr,
                        size_t NumRows);

#endif
