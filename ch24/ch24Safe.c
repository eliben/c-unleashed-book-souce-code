/* Code by Ian D. K. Kelly, for "C Unleashed", chapter 24              */

/* Ch24Safe.c
* 
*  Safe - Safe double arithmetic routines
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

/***********************************************************************
 *                                                                     *
 * The Safe Floating Point routines provide a set of operations upon   *
 * variables of type double which are guranteed in precision and safety*
 * There are two routines for each of the aritmetic operations: the    *
 * "sensitive" (complex) routine, which performs detailed range and    *
 * precision checking, passing this information back to the caller, and*
 * also (potentially) raising error signals, and the "simple" routine, *
 * which implements the raw arithmetic operation. In each case the     *
 * "simple" routine calls the "sensitive" routine to perform the oper- *
 * ation, but indicates that no errors are to be raised.               *
 *                                                                     *
 ***********************************************************************/

#include <stdio.h>
#include <signal.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include <string.h>

#include "Ch24Safe.h"

#ifndef TRUE
#define TRUE (0==0)
#endif

#ifndef FALSE
#define FALSE (1==0)
#endif

/* Macro constant required for flpcmp() */
#define COMPARE_EPSILON DBL_EPSILON

/*
* ARITHMETIC_ACCEPT_PRECISION is the limit of acceptable precision after
* any arithmetic operation. This has been set to one percent (0.01),  
* but may be altered to any value considered appropriate. This must   
* be a long double (hence the trailing L):                            
* You may like to consider modifying these routines to take this limit
* as a parameter variable passed in, or as a global variable, whose   
* value may be altered at run time. For the moment, however, it is an  
* explicit value.                                                              
*/
#define ARITHMETIC_ACCEPT_PRECISION (0.01L)

/* If there are any errors detected by the Safe Arithmetic routines    */
/* they may wish to Raise an error signal. The mappings here are from  */
/* the internal symbolic references in the code to the system signals  */
/* which are to be raised. There is no necessity that these are all    */
/* different, but it does make subsequent error determination easier if*/
/* they are. It is also up to you, the implementor, to decide which of */
/* the possible signals these should be: this is just an example set:  */
#define ARITHMETIC_DENOMINATOR_ZERO SIGILL
#define ARITHMETIC_PRECISION_BAD    SIGINT
#define ARITHMETIC_BEYOND_RANGE     SIGFPE
#define ARITHMETIC_ILLEGAL_COMPARE  SIGILL

/***********************************************************************
 * This is the second ("complex") level Safe Divide function for double*
 * operands, which may raise error conditions, and indicates to the    *
 * caller the precision of the result.                                 *
 ***********************************************************************/

/***********************************************************************
 * Name:          flpSafeDivideSensitive
 * 
 * Description:   This function performs a safe floating-point divide
 *                operation, with detection and signalling of errors
 *                
 * Parameters: 
 *    flpNumerator            Numerator of the division
 *    flpNumeratorPrecision   precision of the numerator, expressed as
 *                            a double, which is the error margin
 *    flpDenominator          Denominator of the division
 *    flpdenominatorPrecision precision of the denominator
 *    pflpResultPrecision     pointer to a double which will contain
 *                            the resultant precision after the division.
 *                            If this pointer is NULL it is not used
 *    RaiseError              an int containing a logical value. 
 *                            If TRUE then errors will be Raised, if
 *                            FALSE they will not, even if detected
 *    pErrorRaised            an int * pointing to a word containing
 *                            error indication. If this pointer is NULL
 *                            then it is not used
 * 
 * External/Global variables:
 *    N/A
 *
 * Returns:
 *    the quotient, as a double
 *    the result precision, if requested
 *    the error condition, if requested
 *    Raises a signal error, if requested, and required
 * 
 **********************************************************************/

double flpSafeDivideSensitive ( double flpNumerator,
                                double flpNumeratorPrecision,
                                double flpDenominator,
                                double flpDenominatorPrecision,
                                double * pflpResultPrecision,
                                int RaiseError, int * pErrorRaised )
{
   double flpReturnValue           = 0.0;
   double flpZero                  = 0.0;
   double flpLocalResultPrecision  = 0.0;
   double flpLocalNumPrecision     = 0.0;
   double flpLocalDenomPrecision   = 0.0;
   long double flpLocalReturnValue = 0.0;
   
   /* Set the outgoing precision initially to its minimum:            */
   /* The following calculation computes the maximum of (1) the passed*/
   /* in value of the precision, (2) the minimum representation error */
   /* for numbers of that magnitude and (3) the consequential error   */
   /* from the operation                                              */
   if ( pflpResultPrecision != NULL )
   {
      flpLocalNumPrecision = DBL_EPSILON * fabs( flpNumerator );
      if ( DBL_EPSILON > flpLocalNumPrecision )
         flpLocalNumPrecision = DBL_EPSILON;
      if ( flpNumeratorPrecision > flpLocalNumPrecision )
         flpLocalNumPrecision = flpNumeratorPrecision;
      flpLocalDenomPrecision = DBL_EPSILON * fabs( flpDenominator );
      if ( DBL_EPSILON > flpLocalDenomPrecision )
         flpLocalDenomPrecision = DBL_EPSILON;
      if ( flpDenominatorPrecision > flpLocalDenomPrecision )
         flpLocalDenomPrecision = flpDenominatorPrecision;
      flpLocalResultPrecision = ( flpLocalNumPrecision   * fabs ( flpDenominator ) )
                              + ( flpLocalDenomPrecision * fabs ( flpNumerator ) );
      if ( flpLocalResultPrecision < DBL_EPSILON )
           flpLocalResultPrecision = DBL_EPSILON;
   /* Note that the above calculation ignores the product of the two  */
   /* precisions, which is usually very small in comparison.          */
      *pflpResultPrecision = flpLocalResultPrecision;
   }
   
   /* Set the outgoing error condition to no error:                   */
   if ( pErrorRaised != NULL )
       *pErrorRaised = 0;

   /* If the incoming numerator is (close to) zero, then the result   */
   /* is also zero:                                                   */
   if ( flpcmp ( flpNumerator, "==", flpZero ) )
      return ( flpReturnValue );
   
   /* We know that the incoming numerator is not (close to) zero.     */
   /* Look at the incoming denominator, and if that is (close to) zero*/
   /* then raise the ARITHMETIC_DENOMINATOR_ZERO error:                 */
   if ( flpcmp ( flpDenominator, "==", flpZero ) )
   {
      /* Indicate that the precision is extremely low:                */
      if ( pflpResultPrecision != NULL )
          *pflpResultPrecision = DBL_MAX;
      
      /* Indicate the error we have detected:                         */
      if ( pErrorRaised != NULL )
          *pErrorRaised = ARITHMETIC_DENOMINATOR_ZERO;
      
      /* If we are raising errors, then raise the error, otherwise    */
      /* return to the caller with the maximum value:                 */
      if ( RaiseError )
      {
         raise ( ARITHMETIC_DENOMINATOR_ZERO );
      } 
      flpReturnValue = DBL_MAX;
      return ( flpReturnValue );
   }
   
   /* We now know that neither the numerator nor the denominator are  */
   /* (close to) zero.                                                */
   
   /* Test whether the result is likely to be beyond the representable*/
   /* range. That is, will the absolute value of the result exceed    */
   /* DBL_MAX. There are several ways of making this estimation, the  */
   /* simplest of which is to extend the operands into longer fields  */
   /* (in our case long double), perform the division and make a comp-*/
   /* arison of the result against DBL_MAX. This method will not work */
   /* if (a) there is no longer representation of floating point num- */
   /* bers than the arguments in use, or (b) the internal and invis-  */
   /* ible conversion is in any way in error. If this code is run     */
   /* under NT or AIX these are good assumptions, and the method is OK*/
   
   /* In the following expression the casts are made explicitly to    */
   /* ensure that the conversions to (long double) are performed early*/
   /* enough:                                                         */
   flpLocalReturnValue = (long double) flpNumerator / 
                         (long double) flpDenominator;
   /* Now we know the magnitude of the result, adjust the outgoing    */
   /* precision, to account for the minimum error for numbers of this */
   /* value-range:                                                    */
   if ( flpLocalResultPrecision < DBL_EPSILON * fabs ( flpLocalReturnValue ) )
        flpLocalResultPrecision = DBL_EPSILON * fabs ( flpLocalReturnValue ) ;

   if ( fabs ( flpLocalReturnValue ) > DBL_MAX )
   {
      /* Indicate that the precision is extremely low:                */
      if ( pflpResultPrecision != NULL )
          *pflpResultPrecision = DBL_MAX;
      
      /* Indicate the error we have detected:                         */
      if ( pErrorRaised != NULL )
          *pErrorRaised = ARITHMETIC_PRECISION_BAD;
      
      /* Set the return value to the maximum possible, with the       */
      /* the sign of the real result:                                 */
      if ( flpLocalReturnValue < 0.0L )
         flpReturnValue = - DBL_MAX;
      else flpReturnValue = DBL_MAX;
      
      /* If we are raising errors, then raise the error, otherwise    */
      /* return to the caller with a zero value:                      */
      if ( RaiseError )
      {
         raise ( ARITHMETIC_PRECISION_BAD );
      } 

      /* return to caller with computed value:                        */
      return ( flpReturnValue );

   } else {

      /* All is well: set the return value to the computed value:     */
      flpReturnValue = flpLocalReturnValue;

      /* Look at the resultant precision and compare it in magnitude  */
      /* to the result. If better than ARITHMETIC_ACCEPT_PRECISION    */
      /* then accept it, otherwise raise an error.                    */
      /* In any case, return its value to the caller:                 */
      if ( pflpResultPrecision != NULL )
          *pflpResultPrecision = flpLocalResultPrecision; 

      /* Note that the following calculation is done as a multiplica- */
      /* tion to avoid having to do all the division tests. The cross-*/
      /* multiplication of the inequality                             */
      /*      (error / value) > acceptable_precision                  */
      /* works for all values of "value"                              */
      if (  flpLocalResultPrecision  >
           ( fabs ( flpReturnValue ) > DBL_EPSILON ?
               ( fabs ( flpReturnValue ) * ARITHMETIC_ACCEPT_PRECISION ) : 
               DBL_EPSILON ) )
      {

         /* Indicate the error we have detected:                      */
         if ( pErrorRaised != NULL )
             *pErrorRaised = ARITHMETIC_BEYOND_RANGE;
      
         /* If we are raising errors, then raise the error:           */
         if ( RaiseError )
         {
            raise ( ARITHMETIC_BEYOND_RANGE );
         }  /* end of "if/then" raising ARITHMETIC_BEYOND_RANGE error */ 
      }     /* end of "if/then" precision is bad                      */
   }        /* end of "if/else" result lies with acceptable range     */
   
   /* Return to caller with the computed value:                       */
   return ( flpReturnValue );

}

/***********************************************************************
 * This is the first ("simple") safe divide procedure for floating     *
 * (double) values. It calls the "complex" safe divide procedure above *
 * (flpSafeDivideSensitive), indicating that no errors are to be raised*
 * to perform the body of the calculation.                             *
 ***********************************************************************/

/***********************************************************************
 * Name:          flpSafeDivide
 * 
 * Description:   This function performs a safe floating-point divide
 *                operation, without signalling any error
 *                
 * Parameters: 
 *    flpNumerator            Numerator of the division
 *    flpDenominator          Denominator of the division
 * 
 * External/Global variables:
 *    N/A
 *
 * Returns:
 *    the quotient, as a double
 * 
 **********************************************************************/

double flpSafeDivide (double flpNumerator,
                      double flpDenominator)
{
  double flpReturnValue = 0.0;
   
  flpReturnValue =  flpSafeDivideSensitive ( flpNumerator, 0.0, 
                                             flpDenominator, 0.0,
                                             NULL, FALSE, NULL);
  
  return (flpReturnValue);
 
} 

/***********************************************************************/

/***********************************************************************
 * This is the second ("complex") level Safe Multiply function for     *
 * double operands, which may raise error conditions, and indicates to *
 * the caller the precision of the result.                             *
 ***********************************************************************/

/***********************************************************************
 * Name:          flpSafeMultiplySensitive
 * 
 * Description:   This function performs a safe floating-point multiply
 *                operation, with detection and signalling of errors
 *                
 * Parameters: 
 *    flpFirstMultiplicand    the first Multiplicand
 *    flpFirstMultiplicandPrecision precision of the first multiplicand
 *    flpSecondMultiplicand   the second Multiplcand
 *    flpSecondMultiplicandPrecision precision of the second multiplicand
 *    pflpResultPrecision     pointer to a double which will contain
 *                            the resultant precision after the multiply.
 *                            If this pointer is NULL it is not used
 *    RaiseError              an int containing a logical value. 
 *                            If TRUE then errors will be Raised, if
 *                            FALSE they will not, even if detected
 *    pErrorRaised            an int * pointing to a word containing
 *                            error indication. If this pointer is NULL
 *                            then it is not used
 * 
 * External/Global variables:
 *    N/A
 *
 * Returns:
 *    the product, as a double
 *    the result precision, if requested
 *    the error condition, if requested
 *    Raises a signal error, if requested, and required
 * 
 **********************************************************************/

double flpSafeMultiplySensitive ( double flpFirstMultiplicand,
                                  double flpFirstMultiplicandPrecision,
                                  double flpSecondMultiplicand,
                                  double flpSecondMultiplicandPrecision,
                                  double * pflpResultPrecision,
                                  int RaiseError, int * pErrorRaised )
{
   double flpReturnValue           = 0.0;
   double flpZero                  = 0.0;
   double flpLocalResultPrecision  = 0.0;
   double flpLocalFirstPrecision   = 0.0;
   double flpLocalSecondPrecision  = 0.0;
   long double flpLocalReturnValue = 0.0;
   
   /* Set the outgoing precision initially to its minimum:            */
   if ( pflpResultPrecision != NULL )
   {
      flpLocalFirstPrecision = DBL_EPSILON * fabs( flpFirstMultiplicand );
      if ( DBL_EPSILON > flpLocalFirstPrecision )
         flpLocalFirstPrecision = DBL_EPSILON;
      if ( flpFirstMultiplicandPrecision > flpLocalFirstPrecision )
         flpLocalFirstPrecision = flpFirstMultiplicandPrecision;
      flpLocalSecondPrecision = DBL_EPSILON * fabs( flpSecondMultiplicand );
      if ( DBL_EPSILON > flpLocalSecondPrecision )
         flpLocalSecondPrecision = DBL_EPSILON;
      if ( flpSecondMultiplicandPrecision > flpLocalSecondPrecision )
         flpLocalSecondPrecision = flpSecondMultiplicandPrecision;
      flpLocalResultPrecision = ( flpLocalFirstPrecision   * 
                                  fabs ( flpSecondMultiplicand ) )
                              + ( flpLocalSecondPrecision * 
                                  fabs ( flpFirstMultiplicand ) );
      if ( flpLocalResultPrecision < DBL_EPSILON )
           flpLocalResultPrecision = DBL_EPSILON;
      *pflpResultPrecision = flpLocalResultPrecision;
   }
   
   /* Set the outgoing error condition to no error:                   */
   if ( pErrorRaised != NULL )
       *pErrorRaised = 0;

   /* If either of the incoming operands is close to zero, set the    */
   /* result to zero, and compute the resultant precision as being the*/
   /* largest of the incoming precisions:                             */
   if (    flpcmp ( flpFirstMultiplicand,  "==", 0.0 ) 
        || flpcmp ( flpSecondMultiplicand, "==", 0.0 ) )
   {
      if ( pflpResultPrecision != NULL )
      {
        if ( flpFirstMultiplicandPrecision > flpSecondMultiplicandPrecision ) 
              *pflpResultPrecision = flpFirstMultiplicandPrecision;
         else *pflpResultPrecision = flpSecondMultiplicandPrecision;
      }
      /* Return to caller with the computed value:                    */
      return ( flpReturnValue );
   }
   
   /* Neither of the incoming multiplicands is (close to) zero. So    */
   /* compute the resultant precision of the product:                 */
   if ( pflpResultPrecision != NULL )
   {
      *pflpResultPrecision = (  ( flpFirstMultiplicandPrecision  * 
                                  fabs ( flpSecondMultiplicand ) )
                              + ( flpSecondMultiplicandPrecision * 
                                  fabs ( flpFirstMultiplicand  ) ) );
   }
   
   /* Now compute the product, but in long double format. Look at the */
   /* result to determine whether we have overshot the allowable      */
   /* size for "double". The same comments apply here as are stated in*/
   /* the routine flpSafeDivideSensitive:                             */
   /* In the following expression the casts are made explicitly to    */
   /* ensure that the conversions to (long double) are performed early*/
   /* enough:                                                         */
   flpLocalReturnValue = (long double) flpFirstMultiplicand * 
                         (long double) flpSecondMultiplicand;
   /* Now we know the magnitude of the result, adjust the outgoing    */
   /* precision, to account for the minimum error for numbers of this */
   /* value-range:                                                    */
   if ( flpLocalResultPrecision < DBL_EPSILON * fabs ( flpLocalReturnValue ) )
        flpLocalResultPrecision = DBL_EPSILON * fabs ( flpLocalReturnValue ) ;

   if ( fabs ( flpLocalReturnValue ) > DBL_MAX )
   {
      /* Indicate that the precision is extremely low:                */
      if ( pflpResultPrecision != NULL )
          *pflpResultPrecision = DBL_MAX;
      
      /* Indicate the error we have detected:                         */
      if ( pErrorRaised != NULL )
          *pErrorRaised = ARITHMETIC_PRECISION_BAD;
      
      /* Set the return value to the maximum possible, with the       */
      /* the sign of the real result:                                 */
      if ( flpLocalReturnValue < 0.0L )
         flpReturnValue = - DBL_MAX;
      else flpReturnValue = DBL_MAX;
      
      /* If we are raising errors, then raise the error, otherwise    */
      /* return to the caller with a zero value:                      */
      if ( RaiseError )
      {
         raise ( ARITHMETIC_PRECISION_BAD );
      } 

      /* return to caller with computed value:                        */
      return ( flpReturnValue );

   } else {

      /* All is well: set the return value to the computed value:     */
      flpReturnValue = flpLocalReturnValue;

      /* Look at the resultant precision and compare it in magnitude  */
      /* to the result. If better than ARITHMETIC_ACCEPT_PRECISION    */
      /* then accept it, otherwise raise an error:                    */
      /* In any case, return it to the caller:                        */
      if ( pflpResultPrecision != NULL )
          *pflpResultPrecision = flpLocalResultPrecision;

      /* Note that the following calculation is done as a multiplica- */
      /* tion to avoid having to do all the division tests. The cross-*/
      /* multiplication of the inequality                             */
      /*      (error / value) > acceptable_precision                  */
      /* works for all values of "value"                              */
      if ( flpLocalResultPrecision  >
           ( fabs ( flpReturnValue ) > DBL_EPSILON ?
               ( fabs ( flpReturnValue ) * ARITHMETIC_ACCEPT_PRECISION ) : 
               DBL_EPSILON ) )
      {

         /* Indicate the error we have detected:                      */
         if ( pErrorRaised != NULL )
             *pErrorRaised = ARITHMETIC_BEYOND_RANGE;
      
         /* If we are raising errors, then raise the error:           */
         if ( RaiseError )
         {
            raise ( ARITHMETIC_BEYOND_RANGE );
         }  /* end of "if/then" raising ARITHMETIC_BEYOND_RANGE error */ 
      }     /* end of "if/then" precision is bad                      */
   }        /* end of "if/else" result lies with acceptable range     */
   
   /* Return to caller with the computed value:                       */
   return ( flpReturnValue );

}

/***********************************************************************
 * This is the first ("simple") safe multiply procedure for floating   *
 * (double) values. It calls the "complex" safe multiply procedure     *
 * (flpSafeMultiplySensitive), indicating that no errors are to be     *
 * raised, to perform the body of the calculation.                     *
 ***********************************************************************/

/***********************************************************************
 * Name:          flpSafeMultiply
 * 
 * Description:   This function performs a safe floating-point multiply
 *                operation, without signalling any error
 *                
 * Parameters: 
 *    flpFirstMultiplicand    the first Multiplicand
 *    flpSecondMultiplicand   the second Multiplcand
 * 
 * External/Global variables:
 *    N/A
 *
 * Returns:
 *    the product, as a double
 * 
 **********************************************************************/

double flpSafeMultiply (double flpFirstMultiplicand,
                        double flpSecondMultiplicand)
{
  double flpReturnValue = 0.0;
   
  flpReturnValue =  flpSafeMultiplySensitive ( flpFirstMultiplicand, 0.0,
                                               flpSecondMultiplicand, 0.0,
                                               NULL, FALSE, NULL);
  
  return (flpReturnValue);
 
} 

/***********************************************************************/

/***********************************************************************
 * This is the second ("complex") level Safe Addition function for     *
 * double operands, which may raise error conditions, and indicates to *
 * the caller the precision of the result.                             *
 ***********************************************************************/

/***********************************************************************
 * Name:          flpSafeAddSensitive
 * 
 * Description:   This function performs a safe floating-point add
 *                operation, with detection and signalling of errors
 *                
 * Parameters: 
 *    flpFirstAddend          the first addend
 *    flpFirstAddendPrecision precision of the first addend
 *    flpSecondAddend         the second addend
 *    flpSecondAddendPrecision precision of the second addend
 *    pflpResultPrecision     pointer to a double which will contain
 *                            the resultant precision after the add.
 *                            If this pointer is NULL it is not used
 *    RaiseError              An int containing a logical value. 
 *                            If TRUE then errors will be Raised, if
 *                            FALSE they will not, even if detected
 *    pErrorRaised            An int * pointing to a word containing
 *                            error indication. If this pointer is NULL
 *                            then it is not used
 * 
 * External/Global variables:
 *    N/A
 *
 * Returns:
 *    the sum, as a double
 *    the result precision, if requested
 *    the error condition, if requested
 *    Raises a signal error, if requested, and required
 * 
 **********************************************************************/

double flpSafeAddSensitive ( double flpFirstAddend,
                             double flpFirstAddendPrecision,
                             double flpSecondAddend,
                             double flpSecondAddendPrecision,
                             double * pflpResultPrecision,
                             int RaiseError, int * pErrorRaised )
{
   double flpReturnValue           = 0.0;
   double flpZero                  = 0.0;
   double flpLocalResultPrecision  = 0.0;
   double flpLocalFirstPrecision   = 0.0;
   double flpLocalSecondPrecision  = 0.0;
   long double flpLocalReturnValue = 0.0;
   
   /* Set the outgoing precision initially to its minimum:            */
   if ( pflpResultPrecision != NULL )
   {
      flpLocalFirstPrecision = DBL_EPSILON * fabs( flpFirstAddend );
      if ( DBL_EPSILON > flpLocalFirstPrecision )
         flpLocalFirstPrecision = DBL_EPSILON;
      if ( flpFirstAddendPrecision > flpLocalFirstPrecision )
         flpLocalFirstPrecision = flpFirstAddendPrecision;
      flpLocalSecondPrecision = DBL_EPSILON * fabs( flpSecondAddend );
      if ( DBL_EPSILON > flpLocalSecondPrecision )
         flpLocalSecondPrecision = DBL_EPSILON;
      if ( flpSecondAddendPrecision > flpLocalSecondPrecision )
         flpLocalSecondPrecision = flpSecondAddendPrecision;
      flpLocalResultPrecision = flpLocalFirstPrecision    
                              + flpLocalSecondPrecision  ;
      if ( flpLocalResultPrecision < DBL_EPSILON )
         flpLocalResultPrecision = DBL_EPSILON;
      *pflpResultPrecision = flpLocalResultPrecision;
   }
   
   /* Set the outgoing error condition to no error:                   */
   if ( pErrorRaised != NULL )
       *pErrorRaised = 0;
   
   /* Now compute the sum, but in long double format. Look at the     */
   /* result to determine whether we have overshot the allowable      */
   /* size for "double". The same comments apply here as are stated in*/
   /* the routine flpSafeDivideSensitive:                             */
   /* In the following expression the casts are made explicitly to    */
   /* ensure that the conversions to (long double) are performed early*/
   /* enough:                                                         */
   flpLocalReturnValue = (long double) flpFirstAddend +
                         (long double) flpSecondAddend;
   /* Now we know the magnitude of the result, adjust the outgoing    */
   /* precision, to account for the minimum error for numbers of this */
   /* value-range:                                                    */
   if ( flpLocalResultPrecision < DBL_EPSILON * fabs ( flpLocalReturnValue ) )
        flpLocalResultPrecision = DBL_EPSILON * fabs ( flpLocalReturnValue ) ;

   if ( fabs ( flpLocalReturnValue ) > DBL_MAX )
   {
      /* Indicate that the precision is extremely low:                */
      if ( pflpResultPrecision != NULL )
          *pflpResultPrecision = DBL_MAX;
      
      /* Indicate the error we have detected:                         */
      if ( pErrorRaised != NULL )
          *pErrorRaised = ARITHMETIC_PRECISION_BAD;
      
      /* Set the return value to the maximum possible, with the       */
      /* the sign of the real result:                                 */
      if ( flpLocalReturnValue < 0.0L )
         flpReturnValue = - DBL_MAX;
      else flpReturnValue = DBL_MAX;
      
      /* If we are raising errors, then raise the error, otherwise    */
      /* return to the caller with a zero value:                      */
      if ( RaiseError )
      {
         raise ( ARITHMETIC_PRECISION_BAD );
      } 

      /* return to caller with computed value:                        */
      return ( flpReturnValue );

   } else {

      /* All is well: set the return value to the computed value:     */
      flpReturnValue = flpLocalReturnValue;

      /* Look at the resultant precision and compare it in magnitude  */
      /* to the result. If better than ARITHMETIC_ACCEPT_PRECISION    */
      /* then accept it, otherwise raise an error:                    */
      /* In any case, return it to the caller:                        */
      if ( pflpResultPrecision != NULL )
          *pflpResultPrecision = flpLocalResultPrecision;

      /* Note that the following calculation is done as a multiplica- */
      /* tion to avoid having to do all the division tests. The cross-*/
      /* multiplication of the inequality                             */
      /*      (error / value) > acceptable_precision                  */
      /* works for all values of "value"                              */
      if ( flpLocalResultPrecision  >
           ( fabs ( flpReturnValue ) > DBL_EPSILON ?
               ( fabs ( flpReturnValue ) * ARITHMETIC_ACCEPT_PRECISION ) : 
               DBL_EPSILON ) )
      {

         /* Indicate the error we have detected:                      */
         if ( pErrorRaised != NULL )
             *pErrorRaised = ARITHMETIC_BEYOND_RANGE;
      
         /* If we are raising errors, then raise the error:           */
         if ( RaiseError )
         {
            raise ( ARITHMETIC_BEYOND_RANGE );
         }  /* end of "if/then" raising ARITHMETIC_BEYOND_RANGE error */ 
      }     /* end of "if/then" precision is bad                      */
   }        /* end of "if/else" result lies with acceptable range     */
   
   /* Return to caller with the computed value:                       */
   return ( flpReturnValue );

}

/***********************************************************************
 * This is the first ("simple") safe addition procedure for floating   *
 * (double) values. It calls the "complex" safe addition procedure     *
 * (flpSafeAddSensitive), indicating that no errors are to be          *
 * raised, to perform the body of the calculation.                     *
 ***********************************************************************/


/***********************************************************************
 * Name:          flpSafeAdd
 * 
 * Description:   This function performs a safe floating-point add
 *                operation, without signalling errors
 *                
 * Parameters: 
 *    flpFirstAddend        the first Addend
 *    flpSecondAddend       the second Addend
 * 
 * External/Global variables:
 *    N/A
 *
 * Returns:
 *    the sum, as a double
 * 
 **********************************************************************/

double flpSafeAdd (double flpFirstAddend,
                   double flpSecondAddend)
{
  double flpReturnValue = 0.0;
   
  flpReturnValue =  flpSafeAddSensitive ( flpFirstAddend, 0.0,
                                          flpSecondAddend, 0.0,
                                          NULL, FALSE, NULL);
  
  return (flpReturnValue);
 
} 

/***********************************************************************/

/***********************************************************************
 * This is the second ("complex") level Safe Subtraction function for  *
 * double operands, which may raise error conditions, and indicates to *
 * the caller the precision of the result.                             *
 ***********************************************************************/

/***********************************************************************
 * Name:          flpSafeSubtractSensitive
 * 
 * Description:   This function performs a safe floating-point subtract
 *                operation, with detection and signalling of errors
 *                
 * Parameters: 
 *    flpMinuend              the Minuend
 *    flpMinuendPrecision     precision of the minuend
 *    flpSubtrahend           the Subtrahend
 *    flpSubtrahendPrecision  precision of the subtrahend
 *    pflpResultPrecision     pointer to a double which will contain
 *                            the resultant precision after the subtract.
 *                            If this pointer is NULL it is not used
 *    RaiseError              An int containing a logical value. 
 *                            If TRUE then errors will be Raised, if
 *                            FALSE they will not, even if detected
 *    pErrorRaised            An int * pointing to a word containing
 *                            error indication. If this pointer is NULL
 *                            then it is not used
 * 
 * External/Global variables:
 *    N/A
 *
 * Returns:
 *    the difference, as a double
 *    the result precision, if requested
 *    the error condition, if requested
 *    Raises a signal error, if requested, and required
 * 
 **********************************************************************/

double flpSafeSubtractSensitive ( double flpMinuend,
                                  double flpMinuendPrecision,
                                  double flpSubtrahend,
                                  double flpSubtrahendPrecision,
                                  double * pflpResultPrecision,
                                  int RaiseError, int * pErrorRaised )
{
  double flpReturnValue           = 0.0;
  
  /* Since subtraction is simply the reverse of addition, we call the */
  /* Safe Addition function to perform the body:                      */
  flpReturnValue = flpSafeAddSensitive ( 
                          flpMinuend,  flpMinuendPrecision,
                        - flpSubtrahend, flpSubtrahendPrecision,
                          pflpResultPrecision, RaiseError, pErrorRaised );
  
  return (flpReturnValue);
 
} 

/***********************************************************************
 * This is the first ("simple") safe Subtraction procedure for floating*
 * (double) values. It calls the "complex" safe Subtraction procedure  *
 * (flpSafeSubtractSensitive), indicating that no errors are to be     *
 * raised, to perform the body of the calculation.                     *
 ***********************************************************************/

/***********************************************************************
 * Name:          flpSafeSubtract
 * 
 * Description:   This function performs a safe floating-point subtract
 *                operation, without signalling any errors
 *                
 * Parameters: 
 *    flpMinuend        the Minuend
 *    flpSubtrahend     the Subtrahend
 * 
 * External/Global variables:
 *    N/A
 *
 * Returns:
 *    the difference, as a double
 * 
 **********************************************************************/

double flpSafeSubtract (double flpMinuend,
                        double flpSubtrahend)
{
  double flpReturnValue = 0.0;
   
  flpReturnValue =  flpSafeSubtractSensitive ( flpMinuend, 0.0,
                                               flpSubtrahend, 0.0,
                                               NULL, FALSE, NULL);
  
  return (flpReturnValue);
 
} 

/***********************************************************************/

/***********************************************************************
 * Name: flpcmp
 *
 * Description:
 *  Tests two real numbers (doubles) according to the conditional
 *  operator provided. The inequality is checked using a defined epsilon
 *  value to determine if the values are equivalent if not exactly equal
 *  to reduce the risk of errors in floating point calculations.
 *  If an invalid conditional operator is specified SIGILL will be raised.
 *
 * Parameters:
 *  dA (in)       - Value to test.
 *  sCond (in)    - String containing the conditional operator, may be
 *                  "==", "!=", "<=", ">=", "<", or ">".
 *  dB (in)       - Value to test.
 *
 * External/Global variables:
 *    N/A
 *
 * Returns:
 *  TRUE  - Condition is true.
 *  FALSE - condition is false.
 *
 ***********************************************************************/

int flpcmp( double dA, char *sCond, double dB )
{
   /* Variable declarations. */
   int iStatus = -1;
   int iEqual = -1;
   int iALargerThanB = -1;
   double dBig, dAbsDiff, dDelta;

   /* Get the input value with the largest absolute value. We want the largest
      of the two values  as this will minimize the introduction of any further
      errors to the inequality when we multiply by epsilon. */
   dBig = fabs( ( fabs( dA ) > fabs( dB ) ) ? dA : dB );

   /* Apply epsilon to the largest absolute value to get the value delta which
      is maximum difference between the two values which can be considered to
      be the same value. */
   dDelta = dBig * COMPARE_EPSILON;

   /* Get the absolute difference between the two values. */
   dAbsDiff = fabs( dA - dB );

   /* Test the absolute difference against the delta value to determine if
      'dA' and 'dB' are equivalent. */
   iEqual = ( dAbsDiff <= dDelta ) ? TRUE : FALSE;

   /* Determine if 'a' is larger than 'b' regardless of epsilon. */
   iALargerThanB = dA > dB;

   /* Set the return status of the function according to the equality operator
      provided. */
   if ( 0 == strcmp( sCond, "==" ) )
   {
      iStatus = iEqual;
   }
   else if ( 0 == strcmp( sCond, "!=" ) )
   {
      iStatus = !iEqual;
   }
   else if ( 0 == strcmp( sCond, "<=" ) )
   {
      iStatus = ( !iALargerThanB ) || iEqual;
   }
   else if ( 0 == strcmp( sCond, ">=" ) )
   {
      iStatus = iALargerThanB || iEqual;
   }
   else if ( 0 == strcmp( sCond, "<" ) )
   {
      iStatus = ( !iALargerThanB ) && ( !iEqual );
   }
   else if ( 0 == strcmp( sCond, ">" ) )
   {
      iStatus = iALargerThanB && ( !iEqual );
   }
   else
   {
      /* Error condition.
         This case should never arise with correct use of this function, if it
         does we will treat this as an illegal instruction. */
      raise( ARITHMETIC_ILLEGAL_COMPARE );
   }

   /* Return the status of the conditional operation. */
   return iStatus;
}

/***********************************************************************
 * Name: flpSetEquivalent;
 *
 * Description:
 *  If the variables provided are equivalent according to epsilon both
 *  are set to the variable with the highest absolute value. If the
 *  variables are not equivalent then the function returns false.
 *
 * Parameters:
 *  dA (in/out) - 
 *  dB (in/out) - 
 *
 * External/Global variables:
 *  N/A
 *
 * Returns:
 *  TRUE  - Variables are equivalent.
 *  FALSE - Variables provided are not equivalent.
 *
 ***********************************************************************/

int flpSetEquivalent( double *dA, double *dB )
{
   /* Variable declarations. */
   int iStatus = FALSE;

   /* Determine if the two values are equivalent. */
   iStatus = flpcmp( *dA, "==", *dB );

   /* If the values are equivalent set both values to the value with the
      highest absolute value of the two. */
   if ( TRUE == iStatus )
   {
      if ( fabs( *dA ) > fabs( *dB ) )
      {
         *dB = *dA;
      }
      else
      {
         *dA = *dB;
      }
   }

   return iStatus;
}

/***********************************************************************/

