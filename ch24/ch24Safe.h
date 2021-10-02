/* Code by Ian D. K. Kelly for Chapter 24, "C Unleashed"    */

/* Ch24Safe.h
* 
*  Prototypes for Safe Arithmetic Functions.
* 
*  Copyright (C) 1999  Ian D. K. Kelly, 
*                      idkk Consultancy Ltd.
*                      Macmillan Computer Publishing 
* 
*  This program is free software; you can redistribute it and/or modify 
*  it under the terms of the GNU General Public License as published by 
*  the Free Software Foundation; either version 2 of the License, or 
*  (at your option) any later version. 
* 
*  This program is distributed in the hope that it will be useful, 
*  but WITHOUT ANY WARRANTY; without even the implied warranty of 
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
*  GNU General Public License for more details. 
* 
*  You should have received a copy of the GNU General Public License 
*  along with this program; if not, write to the Free Software 
*  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. 
* 
*  Ian Kelly may be contacted at idkk@idkk.com
* 
*/ 

#ifndef Ch24_safearithmetic_h
#define Ch24_safearithmetic_h

/* Prototypes for the "first" (simple) safe arithmetic functions:     */
 double flpSafeAdd ( double flpFirstAddend,
                     double flpSecondAddend );
 double flpSafeSubtract ( double flpMinuend,
                          double flpSubtrahend );
 double flpSafeMultiply ( double flpFirstMultiplicand,
                          double flpSecondMultiplicand );
 double flpSafeDivide ( double flpNumerator,
                        double flpDenominator );

/* Prototypes for the "second" (complex) safe arithmetic functions:  */
 double flpSafeAddSensitive ( double flpFirstAddend,
                              double flpFirstAddendPrecision,
                              double flpSecondAddend,
                              double flpSecondAddendPrecision,
                              double * flpReturnPrecision,
                              int RaiseError,
                              int * ErrorRaised );
 double flpSafeSubtractSensitive ( double flpMinuend,
                                   double flpMinuendPrecision,
                                   double flpSubtrahend,
                                   double flpSubtrahendPrecision,
                                   double * flpReturnPrecision,
                                   int RaiseError,
                                   int * ErrorRaised );
 double flpSafeMultiplySensitive ( double flpFirstMultiplicand,
                                   double flpFirstMultiplicandPrecision,
                                   double flpSecondMultiplicand,
                                   double flpSecondMultiplicandPrecision,
                                   double * flpReturnPrecision,
                                   int RaiseError,
                                   int * ErrorRaised );
 double flpSafeDivideSensitive ( double flpNumerator,
                                 double flpNumeratorPrecision,
                                 double flpDenominator,
                                 double flpDenominatorPrecision,
                                 double * flpReturnPrecision,
                                 int RaiseError,
                                 int * ErrorRaised );

/* general floating compare and equivalence routines: */
 int flpcmp( double dA, char *sCond, double dB );
 int flpSetEquivalent( double *dA, double *dB );
   
   
#endif    /* Ch24_safearithmetic_h double copy protection */
