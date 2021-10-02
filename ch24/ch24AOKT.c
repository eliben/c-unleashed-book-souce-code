/*
*
*  This code is part of Chapter 24 of "C Unleashed", and is to be     
*  read in conjunction with that chapter. This code deals only with 
*  very large/precise numbers. These may be integers or have fractional
*  parts (decimal places).
*
*  Code by Ian D. K. Kelly
*
*/

/* Ch24AOK2.c
* 
*  Arith - generalized arithmetic routines
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
            
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

/*
*
* These are sample routines for the basic four arithmetic operators
* for extremely large, extremely precise numbers. 
* All very large numbers are assumed to be stored in arrays of INT
* (which may be "int" or "long int", depending on environmental precisions).  
* The first element of each of these arrays is a counter of the total
* number of further elements that exist in that array. The second element
* of each array is a counter of the number of elements within that array
* that are to be considered as being to the right of the decimal point.
* This second value must be no greater than the first value. The value of  
* the number is stored from left to right, from the most significant
* to the least significant. That is, the lowest subscripts have the  
* largest part of the number.                                        
*
* Each routine, after computing its answer, tries to normalize it. In
* the context of this example code, "normalized" means "having the   
* fewest possible number of leading zeros". If there are any places 
* before the point, then these are the ones that are reduced, and NOT
* the places after the point. The trailing zeros after the point are
* also reduced to the minimum.
* In no case is the length of a number zero: it has at least one "value"
* word. Hence the value zero is:
*       2, 0, 0
* the value one would be
*       2, 0, 1
* the value "point three" (to the base of representation) would be
*       2, 1, 3
* and so on. If the base of representation is 10000 (ten thousand - see
* description further on) then "pi" to 36 places (truncated, not rounded)
* might be represented as
*       11, 9, 3, 1415, 9265, 3589, 7932, 3846, 2643, 3832, 7950, 2884
* and the speed of light in centimeters per second could be
*        5, 1, 2, 9979, 2459, 5000
* which is "299,792,459.5". Avogadro's constant (6.022 E+23) would be
*        7, 0, 6022, 0000, 0000, 0000, 0000, 0000 
*and the proton rest mass in kg. (1.672 E-27) would be
*       10, 9, 0000, 0000, 0000, 0000, 0000, 0000, 0001, 6726, 1411
* Hence the rather meaningless number of "Avogadro plus proton rest mass"
* would be
*       16, 9, 6022, 0000, 0000, 0000, 0000, 0000,
*       0000, 0000, 0000, 0000, 0000, 0000, 0001, 6726, 1411    
*/

/*
*
* The number representation can be thought of as using an extremely   
* large base. For our ordinary counting we use base 10 (ten), if we  
* are using octal, then the base is 8, for hexadecimal 16, and so on.
* We set here a base which is large enough to ensure that we can hold
* very large numbers, but small enough to ensure that the addition of
* two numbers does NOT overflow a word. That is, (2 * BASE) < MAX_INT
* The other restriction is that the number base must not be greater 
* than the square root of MAX_INT, as during the divide process we
* have to multiply the remainder by the base, and add on the next
* "digit" (word). This is the tighter of the two restrictions.
*
* We do not HAVE to choose a base which is a power of ten, but it    
* makes the printing of the answers so much simpler if we do.        
* The base chosen, for 32-bit int, is ten thousand, which is a power 
* of ten, and whose square root is 100 (one hundred). This gives exactly
* four decimal digits to each "digit" (word, element) of the stored
* very large numbers.                
*
*/

#if (INT_MAX>100000000) 

/* INT_MAX is large enough to use the definitions described above:    */
#define BASE 10000
#define SQRT_BASE 100
typedef int INT;
#define DIGITS_IN_BASE 4

#else

/*
*
* INT_MAX is not large enough, therefore we have to use "long int" as
* our base element. It might be possible to consider the 64-bit upper
* limit of 9,223,372,036,854,775,807 ( greater than 1E18), hence the
* BASE could be 100000000 (1E8, ten to the eight), and SQRT_BASE 10000 
* (1E4, ten to the four, ten thousand). This depends upon the particular
* machine, and the particular environment. For this sample code, however,
* we stick to the same limits as above: ten thousand, and one hundred.
*
*/
#define BASE 10000
#define SQRT_BASE 100
typedef long int INT;
#define DIGITS_IN_BASE 4

#endif

/*
* The "weak" naming convention addopted in these routines is:
*   each function name contains one Verb (capitalised) that indicates
*        the main action of that routine
*   there is a prefix (not capitalised) to that Verb which indicates:
*        a        operation on a set of arrays
*        casc     cascade: operation on one element, plus an array
*        one      operation on only single items
*        two      operation on one double and one single item
*        internal like an "a" prefix, but no allocations are made
*    variables may have "p", "pp", "i" prefixes indicating "pointer",
*        "pointer to pointer", "integer" and so on.
*
* Most routines whose names begin "a" or "casc" allocate space for their
* answers - except for "aCompare" and "aAbsCompare", which merely compare
* two arrays, and "aNormalize", which MAY allocate an answer array, but also 
* de-allocates the incoming array.
*/

int pairMultiply  ( INT iOne,   INT iTwo,   INT * pAnswer);
int cascMultiply  ( INT * iOne, INT * aTwo, INT ** aAnswer );
int aMultiply     ( INT * aOne, INT * aTwo, INT ** aAnswer);

int oneDivide  ( INT * iNumerator, INT * iDenominator, INT * iAnswerOD );
int pairDivide ( INT * iNumerator, INT * iDenominator, INT * iAnswerPD );
int cascDivide ( INT * aNumerator, INT * iDenominator, 
                 int places,       INT ** aAnswer );
int aDivide ( INT * aNumerator, INT * aDenominator, 
              int places,       INT ** aAnswer );
int aDivNormalize ( INT * pinNumerator, int places, INT ** ppoutNumerator );

int aAdd             ( INT * aOne, INT * aTwo, INT ** aAnswer );
int internalAdd      ( INT * aOne, INT * aTwo, INT ** aAnswer );
int aSubtract        ( INT * aOne, INT * aTwo, INT ** aAnswer );
int internalSubtract ( INT * aOne, INT * aTwo, INT ** aAnswer );

int aCompare    ( INT * aOne,       INT * aTwo );
int aAbsCompare ( INT * aOne,       INT * aTwo );
int aNormalize  ( INT ** aUnNormal, INT ** aNormal );
int aAllocate   ( int iCount,       INT ** aAnswer );

/* Multiply two INTs                                                  */
int pairMultiply ( INT iOne, INT iTwo, INT * pAnswer)
{
    /* This routine does not perform any internal allocations         */

    /*
    * Multiply two INTs giving an array of two INTs. The array is    
    * needed, because the answer (the product) may well have many more
    * digits than the incoming multiplicands.
    * This routine assumes that both input arguments are positive.
    */
    INT iStatus = EXIT_SUCCESS;
    INT iOneTop = 0;
    INT iOneBot = 0;
    INT iTwoTop = 0;
    INT iTwoBot = 0;
    INT iAnsTop = 0;
    INT iAnsBot = 0;
 
    
    /*
    * Split the incoming arguments up by the square root of the base,
    * so that we can never exceed the size of an INT during calculation:
    */
    if (iOne>SQRT_BASE)
    {
        iOneTop = iOne / SQRT_BASE;
        iOneBot = iOne % SQRT_BASE;
    }
    else
    {
        iOneTop = 0;
        iOneBot = iOne;
    }
    
    if (iTwo>SQRT_BASE)
    {
        iTwoTop = iTwo / SQRT_BASE;
        iTwoBot = iTwo % SQRT_BASE;
    }
    else
    {
        iTwoTop = 0;
        iTwoBot = iTwo;
    }
    
    iAnsBot = (iOneBot * iTwoBot) +
        (iOneBot * iTwoTop * SQRT_BASE) +
        (iOneTop * iTwoBot * SQRT_BASE);
    iAnsTop = (iOneTop * iTwoTop);

    /* Is there any carry from the bottom word to the top?            */
    if (iAnsBot>BASE)
    {
        iAnsTop = iAnsTop + (iAnsBot / BASE);
        iAnsBot = iAnsBot % BASE;
    }

    /* Set the caller's answer array:                                 */
    pAnswer[0] = iAnsTop;
    pAnswer[1] = iAnsBot;

    return iStatus;
}

/* Multiply an INT array by one INT                                   */
int cascMultiply  ( INT * iOne, INT * aTwo, INT ** aAnswer )
{
    /*
    * This routine allocates space for the answer, which the caller  
    * must, at some time, release.
    * The single INT is assumed to be positive, and the outgoing
    * sign is taken from the incoming sign on the very large number
    * (the second argument).
    */
    int iStatus = EXIT_SUCCESS;
    INT * pInt;
    INT ** ppInt = &pInt;
    INT w[2];
    int i = 0;
    int j = 0;
    int k = 0;
    int carry = 0;
    int iSign = 1;

    w[0] = 0;
    w[1] = 0;

    /*
    * The length of the product is at most one element longer than   
    * the incoming array. Allocate internal space for calculation:   
    */
    i = abs(*aTwo) + 1;
    k = aAllocate ( i, ppInt );
    /*
    * Remember the sign of the incoming array:
    */
    if (*aTwo<0)
        iSign = -1;
    else
        iSign = 1;
    /*
    * Set the decimal length of the new array to be the same as 
    * the incoming array:
    */
    *(pInt+1) = *(aTwo+1);

    /*
    * We multiply from the right to the left, bringing the carry over
    * and calculating the outgoing carry:                            
    */
    for (j=*aTwo; (j>1); j--)
    {
        /* Multiply just this pair of "digits": */
        pairMultiply ( *(aTwo + j), (*iOne), w );

        /* Add the carry into the least significant word: */
        *(pInt+j+1) = w[1] + carry;

        /*
        * Compute the outgoing carry, doing an integer division, and
        * truncating to the integer part of the quotient:
        */
        carry = w[0] + (*(pInt + j + 1) / BASE);

        /* And the correct the word from which the carry came: */
        *(pInt+j+1) %= BASE;
    }

    /*
    * The very last carry is used to set the topmost word in the
    * answer. This is the most significant word:
    */
    *(pInt+2) = carry;

    /* Now we normalize the answer, and hand it back to the caller:   */
    i = aNormalize ( ppInt, aAnswer );

    /* Ensure that the outgoing sign is correct:                      */
    if (iSign<0)
        **aAnswer = - **aAnswer;

    return iStatus;
}

/* Multiply two arrays of INT                                         */
int aMultiply ( INT * aOne, INT * aTwo, INT ** aAnswer)
{
    /*
    * This routine allocates space for the answer, which the caller  
    * must, at some time, release                                    
    */
    int iStatus = EXIT_SUCCESS;
    int i = 0;
    int j = 0;
    int k = 0;
    int m = 0;
    INT * pInt;
    INT ** ppInt = &pInt;
    INT w[2];
    
    /*
    * The function of this routine is to multiply two very large     
    * numbers. Each of them comes in an array of INT. This routine   
    * works out how the length required for the answer, allocates    
    * space for that answer, computes the product, and normalizes    
    * the answer, which is then passed back to the caller.           
    */
    
    /* Firstly check whether the incoming pointers are reasonable:    */
    if ((aOne==NULL) || (aTwo==NULL) || (aAnswer==NULL))
    {
        /*
        * There is a fault in the incoming pointers. Indicate this   
        * to the caller:                                             
        */
        iStatus = EXIT_FAILURE;
    }
    else
    {
        /*
        * The incoming pointers seem to be OK. Before we enter the main
        * (and long) body of this routine, we can check for two special
        * cases. These are if either multiplicand is zero or either is
        * one. In these cases we do not have to perform the full operation,
        * but we know the answer already.
        * So is the first operand zero or one?
        */
        if ((abs(aOne[0])==2) && (aOne[1]==0))
        {
            if (aOne[2]==0)
            {
                /*
                * The first multiplicand is zero. Hence the answer is
                * a normalized zero:
                */
                j = aAllocate ( 2, aAnswer );
                if (j!=0)
                    iStatus = EXIT_FAILURE;
                return iStatus;
            }
            else if (aOne[2]==1)
            {
                /*
                * The first multiplicand is a one. Hence the answer is
                * a copy of the second multiplicand:
                */
                i = abs(aTwo[0]);
                j = aAllocate ( i , aAnswer );
                if (j!=0)
                    iStatus = EXIT_FAILURE;
                else
                {
                    /*
                    * The copy can be a complete copy, including the two
                    * initial counter words.
                    */
                    for (k=0;(k<=i);k++)
                        *(*aAnswer+k) = aTwo[k];
                }   /* end of "if/else" allocation of answer was OK       */
                return iStatus;
            }       /* end of "if/then" first multiplicand have value one */
        }           /* end of "if/then" first multiplicand perhaps 0/1    */

        /*
        * There was nothing special about the first operand. How about
        * the second?
        */
        if ((abs(aTwo[0])==2) && (aTwo[1]==0))
        {
            if (aTwo[2]==0)
            {
                /*
                * The second multiplicand is zero. Hence the answer is
                * a normalized zero:
                */
                j = aAllocate ( 2, aAnswer );
                if (j!=0)
                    iStatus = EXIT_FAILURE;
                return iStatus;
            }
            else if (aTwo[2]==1)
            {
                /*
                * The second multiplicand is a one. Hence the answer is
                * a copy of the first multiplicand:
                */
                i = abs(aTwo[0]);
                j = aAllocate ( i , aAnswer );
                if (j!=0)
                    iStatus = EXIT_FAILURE;
                else
                {
                    /*
                    * The copy can be a complete copy, including the two
                    * initial counter words.
                    */
                    for (k=0;(k<=i);k++)
                        *(*aAnswer+k) = aOne[k];
                }   /* end of "if/else" allocation of answer was OK     */
                return iStatus;
            }       /* end of "if/then" second multiplcand is one       */
        }           /* end of "if/then" second multiplicand perhaps 0/1 */

        /*
        * So now determine the size of the output array required:                         
        */
        i = abs(aOne[0]) + abs(aTwo[0]) + 1;

        /* Allocate enough space for the answer:                      */
        j = aAllocate(i, ppInt);

        /* Did this allocation work? if not, tell caller:             */
        if (j!=0)
            iStatus = EXIT_FAILURE;
        else
        {
            /*
            * The allocation appeared to work. Set the number of decimal
            * places in the newly allocated array to be the sum of the
            * number of places in the two multiplicands:
            */
            *(pInt+1) = aOne[1] + aTwo[1];

            /*
            * Now compute the product of the two incoming, by computing
            * the pairwise product of each pair of incoming INT's, 
            * shifting up each time.
            *  
            * Note that multiplication proceeds from right to left:  
            * that is, we use the RIGHTmost digits first - just as we
            * do on pencil and paper.                                
            */
            for (k=abs(aTwo[0]); (k>1); k--)
            {
                for (j=abs(aOne[0]); (j>1); j--)
                {
                    /*
                    * Compute the pairwise product of One[j] by      
                    * Two[k], and place the result in answer[L-j]    
                    * where L = i + 1 - k                            
                    * Note that this may overflow, so we have to add 
                    * the high end of out answer into the next answer
                    * element "to the left" (with lower subscript):  
                    */
                    m = pairMultiply ( aOne[j], aTwo[k], w);
                    pInt[i + 2 - k - j] += w[1];
                    pInt[i + 1 - k - j] += w[0];
                }   /* End of inner "for/j" along first operand       */
            }       /* end of outer "for/k" along second operand      */

            /*
            * Now consider the sign of the output. Currently it is   
            * set to positive, but we need to apply the logic:       
            *              * | +   -                                 
            *             ___________                                
            *              + | +   -                                 
            *              - | -   +                                 
            */
            if (((aOne[0]>0)&&(aTwo[0]<0))
            ||  ((aOne[0]<0)&&(aTwo[0]>0)))
                pInt[0] = - pInt[0];

            /*
            * At this point we have in the local array the possibly  
            * un-normalized product of the input arrays. Normalize:  
            */
            m = aNormalize (ppInt, aAnswer);

            /* If there was a problem in normalization. Tell caller:  */
            if (m!=0)
                iStatus = EXIT_FAILURE;
        }   /* end of "if/else" allocation worked                     */
    }   /* end of "if/else" the incoming pointers were OK             */

    return iStatus;
}

/* Divide one INT by another INT, getting quotient and remainder      */
int oneDivide ( INT * iNumerator, INT * iDenominator, INT * iAnswerOD )
{
    /*
    * This routine allocates space for the answer, which the caller  
    * must, at some time, release                                    
    */
    int iStatus = EXIT_SUCCESS;

    /*
    * This routine divides the single INT pointed at by the first    
    * parameter by the second INT pointed at by the second parameter.
    * The quotient is given in the first of the pair of INTs pointed 
    * at by the third parameter, and the remainder in the second of  
    * that pair.                                                     
    */

    iAnswerOD[0] = *iNumerator / *iDenominator;
    iAnswerOD[1] = *iNumerator % *iDenominator;

    return iStatus;
}

/* Divide a pair of INTs by one INT, getting quotient and remainder   */
int pairDivide ( INT * iNumerator, INT * iDenominator, INT * iAnswerPD )
{
    int iStatus = EXIT_SUCCESS;
    int j = 0;
    INT r = 0;
    INT w = 0;

    /*
    * This routine divides the pair of INTs pointed at by the first  
    * parameter by the single INT pointed at by the second parameter.
    * The answer is given in the first two of the triplet of INTs    
    * pointed at by the third parameter, and the remainder in the    
    * third of that triplet.                                         
    */

    /* The algorithm used is also that of Knuth, Vol 3. 4.3.1, Ex 16  */

    r = 0;
    for (j=0;(j<=1);j++)
    {
         w = (r * BASE + *(iNumerator + j));
         *(iAnswerPD+j) = w / *(iDenominator);
         r = w % *(iDenominator);
    }
    *(iAnswerPD+2) = r;

    return iStatus;
}

/* Divide an INT array by one INT, getting just the quotient          */
int cascDivide ( INT * aNumerator, INT * iDenominator, 
                 int places, INT ** aAnswer )
{
    /*
    * This routine allocates space for the answer, which the caller  
    * must, at some time, release.
    */

    /*
    * This routine divides the long number, in the INT array pointed 
    * to by the first argument, by the single INT pointed to by the  
    * second argument. Space for the result is allocated. The final  
    * output (pointed to by the pointer the third argument points to)
    * is normalized. Since division may increase the number of places
    * after the decimal point, the argument "places" (the third argument)
    * is used to determine how many extra words are to be allocated to
    * the answer, over and above the number of places give in the
    * incoming numberator (first argument).
    * The sign of the single INT is assumed to be positive, and the
    * sign of the answer is that of the incoming long array (numerator).
    */

    int iStatus = EXIT_SUCCESS;
    int j = 0;
    int k = 0;
    int kk = 0;
    INT r = 0;
    INT w = 0;
    int iSign = 1;
    INT * pInt;
    INT ** ppInt = &pInt;

    if ((aNumerator==NULL) || (iDenominator==NULL) || ((aAnswer)==NULL))
    {
        /*
        * If any of the incoming pointers is NULL, then return with  
        * error indication:                                          
        */
        iStatus = EXIT_FAILURE;
        return iStatus;
    }

    /* The algorithm used is also that of Knuth, Vol 3. 4.3.1, Ex 16  */

    /* Determine the length of the numerator, and remember its sign   */
    r = 0;
    k = abs(*aNumerator);
    /*
    * The variable "k" indicates the number of elements in the incoming
    * array (numerator). This means that this contains:
    *      DIGITS_IN_BASE * (k - 1)
    * places. We need to ensure that what we allocate is at least as
    * long as the greater of (a) k and (b) (places+3)/DIGITS_IN_BASE
    */
    kk = (places+3) / DIGITS_IN_BASE;
    j = max(kk,k);
    aAllocate(j, ppInt);
    if (*aNumerator<0)
        iSign = -1;
    else
        iSign = 1;
    /*
    * The number of places after the point in the new intermediate
    * array is equal to the number of places in the incoming array plus
    * the extra words (if any) that have been allocated
    */
    *(pInt+1) = *(aNumerator+1) + (j>k)?j-k:0;
    k = j;

    for (j=2; (j<=k); j++)
    {
         w = (r * BASE + *(aNumerator + j));
         *(pInt + j) = w / *(iDenominator);
         r = w % *(iDenominator);
    }

    /*
    * Note that at the very end we have an outgoing, residual        
    * carry. This is simply lost, though we may have already saved
    * a lot of extra information in the extra words in the output.
    */

    /* Normalize the intermediate answer into the final answer:   */
    /* Note that this also frees the intermediate work area       */
    j = aNormalize ( ppInt, aAnswer );

    /* Apply the incoming sign to the answer:                         */
    if (iSign<0)
        **aAnswer = - (**aAnswer);

    return iStatus;
}

/* Divide an INT array by an INT array, getting just the quotient     */
int aDivide ( INT * aNumerator, INT * aDenominator, 
              int places,  INT ** aAnswer )
{
    /*
    * This routine allocates space for the answer, which the caller  
    * must, at some time, release                                    
    */

    /*
    * This is the most complex of the routines, and uses the algorithm  
    * described by Knuth in "The Art of Computer Programming".   
    * This is an extension of the simple "pencil and paper" technique
    * that we were taught at school - and many of us have since forgotten.
    */

    int iStatus = EXIT_SUCCESS;
    int i = 0;
    int j = 0;
    int k = 0;
    int l = 0;
    int m = 0;
    int n = 0;
    int borrow = 0;
    int iNmax;
    int iDmax;
    INT carry = 0;
    INT iBase = BASE;
    INT wN[2];
    INT aW[3];
    INT aX[3];
    INT aY[3];
    INT aZ[3];
    INT bigD = 0;
    INT littleD = 0;
    /* Space for a local copy of the numerator. This copy will be     */
    /* long enough to be longer than the denominator.                 */
    INT * plNumerator = NULL;
    INT ** pplNumerator = &plNumerator;
    /* Space for a local copy of the Normalized denominator. This will*/
    /* ensure that the leading INT does not have value zero:          */
    INT *plDenominator = NULL;
    INT ** pplDenominator = & plDenominator;
    /* Space for a local copy of the resultant quotient. This copy is */
    /* prior to the required normaization.                            */
    INT * plQuotient = NULL;
    INT ** pplQuotient = &plQuotient;
    /* Space for three work areas required during the calculation.    */
    INT * plSubtrahend = NULL;
    INT ** pplSubtrahend = &plSubtrahend;
    INT * plMinuend = NULL;
    INT ** pplMinuend = &plMinuend;
    INT * pInter = NULL;
    INT ** ppInter = &pInter;

    /*
    * There are three special cases to check for in Division. If the
    * denominator is one, then we do not have to perform the operation,
    * but just copy the numerator. If the numerator is zero, then the
    * answer is zero. If the denominator is zero, then we have an error
    */

    /* So look at the denominator: */
    if ((abs(*aDenominator)==2) && (*(aDenominator+1)==0))
    {
        if (*(aDenominator+2)==0)
        {
            /* The denominator is zero - error */
            iStatus = EXIT_FAILURE;
            return iStatus;
        }
        else if (*(aDenominator+2)==1)
        {
            /*
            * The denminator has value one - make a copy of the
            * numerator as the answer:
            */
            i = abs(*aNumerator);
            j = aAllocate ( i, aAnswer );
            if (j==0)
            {
                for (k=0;(k<=i);k++)
                    *(*aAnswer+k) = *(aNumerator+k);
            }   /* end of "if/then" alloction of answer was OK */
            else
                iStatus = EXIT_FAILURE;
            return iStatus;
        }   /* end of "if/then" denomintor has value one */
    }       /* end of "if/then" denominator perhaps 0/1  */

    /*
    * There was nothing special about the denominator. How about
    * the numerator? We only look for value zero:
    */
    if ((abs(*aNumerator)==2) && (*(aNumerator+1)==0) 
         && (*(aNumerator+2)==0))
    {
        /*
        * The numerator does have the value zero. Allocate a zero
        * value answer, and return to caller:
        */
        j = aAllocate ( 2, aAnswer );
        if (j!=0)
            iStatus = EXIT_FAILURE;
        return iStatus;
    }   /* end of "if/then" numerator has value zero */

    /*
    * If control falls through to here, we have the general, most usual,
    * case, and we will have to perform the bulk of the division
    * operation.
    */

    /*
    * Normalization for division means (a) ensuring that the numerator
    * is longer than the denominator, and (b) the leading "digit" 
    * of the denominator is greater than floor(BASE/2). We can get   
    * this by multiplying both numerator and denominator by "D",     
    * where "D = floor((b-1)/d" and "d" is the leading "digit" of the
    * denominator. "floor" means "round to the next integer below".
    */

    /* Find the length required for the denominator copy, and allocate*/
    iDmax = abs(aDenominator[0]);
    k = aAllocate ( iDmax, ppInter );

    /*
    * Copy from argument to local copy. Note that this is a simple copy
    * which also copies the count of the number of places after the point
    */
    for (i=1;(i<=iDmax);i++)
        *(pInter+i) = *(aDenominator+i);

    /* And normalize the local copy of the denominator:               */
    k = aNormalize ( ppInter, ppInter );

    /*
    * Get the length of the normalized copy, and compute the scaling 
    * factor by which to multiply both the numerator and denominator: 
    */
    iDmax = *pInter;
    littleD = *(pInter+1);
    bigD = (BASE - 1) / littleD;

    /*
    * Get a scaled (multiplied-up) local copy of the denominator, and
    * release the previous work area.
    */
    cascMultiply ( &bigD, pInter, pplDenominator );
    free ( pInter );

    /*
    * Find out how much space to allocate for a local copy of the 
    * numerator, and get the space, multiplying up at the same time:    
    */
    pplNumerator = &plNumerator;
    plNumerator = NULL;
    cascMultiply ( &bigD, aNumerator, ppInter );

    /*
    * Perform "Division Normalize", which inserts a leading zero, and
    * release the intermediate work area:                            
    */
    k = aDivNormalize ( pInter, places, pplNumerator );
    free ( pInter );

    /*
    * Get the lengths of the normalized numerator, and compute the   
    * length of the quotient:                                        
    * Knuth D1:                                                      
    */
    iNmax = *plNumerator;
    n = iDmax;
    m = iNmax - iDmax;

    /*
    * The length of the quotient was one greater than the difference 
    * in length of the numerator and denominator. Remember that this 
    * routine requires the numerator to be at least as long as the   
    * denominator. The difference in length (and not the "difference 
    * plus one") is required later in the calculation. We have already
    * "lengthened" the numerator, so we need use only the real 
    * difference in length now:                                      
    */

    /* Allocate space for the quotient pre-normalization:             */
    pplQuotient = &plQuotient;
    k = aAllocate ( m, pplQuotient );

    /* Allocate space for holding the intermediate subtraction        */
    pplSubtrahend = &plSubtrahend;
    aAllocate ( n, pplSubtrahend );

    /* Allocate space for holding numerator portion for suntraction:  */
    pplMinuend = &plMinuend;
    aAllocate ( n + 1, pplMinuend );

    /*
    * Division takes place from left to right: we start with the most
    * significant digits - those on the left:                        
    * Knuth D2:                                                      
    */
    for (j=2;(j<=m);j++)
    {
        /* Knuth D3:                                                  */
        wN[0] = *(plNumerator + j); 
        wN[1] = (j==(m+1)) ? 0 : *(plNumerator + j + 1); 
        k = pairDivide ( wN, plDenominator + 1, aW );

        /*
        * Now test for the cases where our putative quotient is too  
        * high. Note that it will sometimes be one too high, and very
        * rarely two too high.                                       
        * If quotient is equal to base, it is too high:              
        */
D3A:    if ((aW[0]==0) && (aW[1]==BASE))
            goto D3B;
        /* If quotient is equal to zero, it cannot be too high:       */
        if ((aW[0]==0) && (aW[1]==0))
            goto D3C;
        /* If there are no more places in the denominator, then there */
        /* is nothing else against which to test:                     */
        if (*plDenominator<2)
            goto D3C;

        /* Now we have to compute the inequality of Knuth step D3     */
        pairMultiply (*(plDenominator+2),aW[0],aX);
        pairMultiply (*(plDenominator+2),aW[1],aY);
        aY[0] += aY[1] / BASE; 
        aY[1] %= BASE;
        aX[1] += aY[0] / BASE; 
        aY[0] %= BASE;
        aZ[0] = aW[2];        
        aZ[1] = *(plNumerator + j - m + 3); 
        carry = aZ[1] / BASE; 
        aZ[1] %= BASE;

        if (aX[1]>carry)
            goto D3B;
        if (aY[0]>aZ[0])
            goto D3B;
        if (aY[0]<aZ[0]) 
            goto D3C;
        if (aY[1]>aZ[1]) 
            goto D3B;
        goto D3C;

D3B:    {
            /*
            * The putative quotient is too high. Reduce it, and go   
            * back round to see whether it is still to high:         
            */
            aW[1] -= 1;
            aW[2] += *(plDenominator + 1);
            if (aW[2]<BASE)
                goto D3A;
        }
D3C:    /*
        * This quotient digit is correct.                            
        * Knuth D4:                                                  
        * We now have to subtract from the local copy of the numerator
        * the denominator times the last quotient "digit":        
        */
        if (aW[1]!=0)
        {
            for (i=2; (i<=(n+1)); i++)
            {
                *(plMinuend + i) = *(plNumerator + m - j + i);
                if (i<(n + 1))
                    *(plSubtrahend + i) = *(plDenominator + i);
            }

            /* cascade multiply denominator by quotient digit */
            cascMultiply ( &aW[1], plDenominator, ppInter );

            /* subtract:                                      */
            if ( *pInter > *plMinuend )
                borrow = 1;
            else
            {
                carry = 0;
                k = *pInter;
                l = (*plMinuend) - k;
                for (i=k; (i>=0); i--)
                {
                    if ((i + l)>0)
                    {
                        *(plMinuend + i + l) -= carry; 
                        *(plMinuend + i + l) -= (i>0) ? *(pInter+i) : 0 ;
                        carry = 0;
                        if (*(plMinuend + i + l)<0)
                        {
                            *(plMinuend + i + l) += BASE;
                            carry = 1;
                        }
                        else
                            carry = 0;
                    }   /* end of "if/then" safe to make replacement  */
                }   /* end of "for/i" computing digits to subtract    */

                /*
                * Test the sign of the answer, and if it is negative then
                * set the "borrow" flag. If it is positive or zero then
                * replace a portion of the local copy of the numerator:
                */
                if (*(plMinuend+1)<0)
                    borrow = 1;
                else
                {
                    borrow = 0;
                    /* replace a portion of our local numerator copy  */
                    for (i=n+1; (i>0); i--)
                    {
                        if ((i + l) <= iNmax)
                           *(plNumerator + i + l) = *(plMinuend + i);
                    }   /* end of "for/i" replace portion of numerator    */
                }   /* end of "if/else" numerator portion was replaced    */
            }   /* end of "if/else" lengths allowed numerator subtraction */

            /* Knuth D5:                                                  */
            *(plQuotient + j) = aW[1];

            /* If the remainder was negative then add back:               */
            if (borrow != 0)
            {
                /* Knuth D6:                                              */
                /*
                * Note that this is a very low probability step. It happens 
                * only in the order of 2/BASE. We have to add back, but
                * ignoring the top end carry:                        
                */
                *(plQuotient + j) = *(plQuotient + j) - 1;
                carry = 0;
                for (i=n+1; (i>1); i--)
                {
                    k = (*plNumerator) - i;
                    *(plNumerator+j+k) += (i==1) ? 0 : 
                                          (*(plDenominator + k) + carry);
                    if (*(plNumerator + j + k) > BASE)
                    {
                        *(plNumerator + j + k) -= BASE;
                        carry = 1;
                    }
                    else
                        carry =0;
                }   /* end of "for/i" adding back in                  */
            }       /* end of "if/then" borrow was non zero           */
        }           /* end of "if/then" quotient digit was non zero   */

        /* Knuth D7:                                                  */

        /* Loop back to get the next quotient "digit"                 */

    }   /* end of "for/j" major loop along digits of numerator        */

    /* Knuth D8:                                                      */
    /* Normalize the result:                                          */
    k = aNormalize ( pplQuotient, aAnswer );

    /* Calculate and apply the sign of the answer:                    */
    /*
    *  This is according to the logic: 
    *        /  |  +   - 
    *        _____________
    *        +  |  +   -
    *        -  |  -   +
    *
    */
    if (((*aNumerator<0) && (*aDenominator>0))
    ||  ((*aNumerator>0) && (*aDenominator<0)))
        **aAnswer = - **aAnswer;

    /* Free the allocated intermediate work areas:                    */
    free ( pInter );
    free ( plSubtrahend );
    free ( plMinuend );
    free ( plQuotient );
    free ( plNumerator );
    free ( plDenominator );

    return iStatus;
}


/* Allocate an INT array for the aDivide internal operation which is  */
/* one longer than the incoming array:                                */
int aDivNormalize ( INT * pinNumerator, int places, INT ** ppoutNumerator )
{
    int iStatus = EXIT_SUCCESS;
    int iNmax = 0;
    int i = 0;
    int j = 0;
    int k = 0;
    int iAllocLength = 0;
    INT * pAnswer = NULL;
    INT ** ppAnswer = &pAnswer;

    /* What is the length required for the new allocation to make?    */
    iNmax = abs(*pinNumerator);
    k = iNmax + 1;
    j = ((places+3)/DIGITS_IN_BASE)+1;
    iAllocLength = max(j,k);

    /* make the allocation:                                           */
    i = aAllocate ( iAllocLength, ppAnswer );

    /*
    * Indicate the new number of places after the point for the new
    * array. This is the same as the number for the incoming array, plus
    * any new words that have been added:
    */
    *(pAnswer+1) = *(pinNumerator+1) + (j>k)?(j-k):0;

    /* Now copy the original into the new area.                       */
    /* Set up the initial zero:                                       */
    *(pAnswer + 2) = 0;

    /* Now copy the rest of the input array to the output:            */
    if (iAllocLength>=3)
    {
        for (k=3; (k <= iAllocLength); k++)
            *(pAnswer + k) = *(pinNumerator + k - 1);
    }
    /*
    * Note that the remaining words, if any, will already have been
    * set to zero during the allocation process
    */

    /* Point the answer to the newly allocated:                      */
    *ppoutNumerator = pAnswer;

    return iStatus;
}

/* Normalize an INT array                                             */
int aNormalize ( INT ** aUnNormal, INT ** aNormal )
{
    /*
    * Although this routine may allocate space for its answer, if it 
    * does so it frees the space used by its first argument. Hence the   
    * caller does NOT have to consider whether to free the returned  
    * space from this routine.                                       
    */

    int iStatus = EXIT_SUCCESS;
    int i = 0;
    int j = 0;
    int k = 0;
    int n = 0;
    int m = 0;
    int mm = 0;
    int b = 0;
    int iSign = 1;
    INT * plNormal = NULL;
    INT ** pplNormal = &plNormal;
    
    /*
    * This function takes two arguments. The first is a pointer to   
    * a pointer to an array of INT. This array may be un-normalized. 
    * the second argument is a pointer to a pointer. This latter is  
    * not yet pointing to anything, but will (at the termination of  
    * this routine) be an array of INT. This will be a normalized    
    * copy of the first argument. If the first array is normal then   
    * this pointer is identical to the first. If the first argument  
    * is not normal, then a new array is allocated, and the first is 
    * de-allocated, and the incoming first pointer set to NULL       
    */
    
    if ((aUnNormal==NULL) || (aNormal==NULL) || ((*aUnNormal)==NULL))
    {
        /*
        * If any of the incoming pointers is NULL, or the first     
        * array does not exist, then indicate an error:            
        */
        iStatus = EXIT_FAILURE;
    }
    else
    {
        /*
        * We know that the incoming pointers are (apparently) OK, So 
        * set the initial outgoing value to be simply a copy of the  
        * incoming:                                                  
        */
        plNormal = *aUnNormal;

        /*
        * Determine how many words (INTs) in the first array we have 
        * to scan:                                                   
        */
        k = abs(**aUnNormal);
        if (**aUnNormal<0)
            iSign = -1;
        /* Number of these which are after the point:                 */
        n = (*aUnNormal)[1];
        /* Therefore the number which are before the point:           */
        b = k - n - 1;

        /*
        * If there is anything at all in the first array, then count 
        * the number of zero words with which it begins:              
        */
        if (b>0)
        {
            j = (*aUnNormal)[2];
            m = 0;
            for (i = 2; ((j==0)&&(i<=b)); i++)
            {
                j = (*aUnNormal)[i];
                if (j==0)
                    m++;
            }
        }
        
        /*
        * At this point we know that 'm' contains a count of the     
        * number of zero words at the start of the UnNormal input.
        * Now count backwards (provided that there is something
        * following the point) up the array, counting the number
        * of zero words with which the array ends, after the point:
        */
        if (n>0)
        {
            j = (*aUnNormal)[k];
            mm = 0;
            for (i = 0; ((j==0)&&(i<n)); i--)
            {
                j = (*aUnNormal)[k-i];
                if (j==0)
                    mm++;
            }
        }
        /*
        * So now in 'm' we have the count of the number of leading
        * words to omit, and in 'mm' we have the count of the trailing
        * words to omit. 
        * So we need to (a) allocate a new array, and (b) copy the   
        * old into the new, and (c) deallocate the old array:        
        */
        if ((m+mm)>0)
        {
            /* Allocate the new normal array:                         */
            k = abs((*aUnNormal)[0]) - m - mm;

            /* Note that a normal zero has at least one leading word: */
            /* do not normalize away all the digits!                  */
            if (k<=1)
            {
                k = 2;
                m--;
            }
            j = aAllocate(k, pplNormal);

            /* Copy the un-normal to the normal:                      */
            for (i=1; (i<(k+1)); i++)
            {
                j = i + m;
                *(plNormal + i) = (*aUnNormal)[j];
            }

            /* Set the sign in the new, remembered from the old:      */
            if (iSign<0)
                *plNormal = - (*plNormal);

            /* de-allocate the old un-normal:                         */
            free(*aUnNormal);

            /* And tell the caller this has happened:                 */
            *aUnNormal = NULL;

        }   /* end of "if/then" there was a necessity to allocate new */

        /* And point the caller to the new normalized value:          */
        *aNormal = plNormal;

    }       /* end of "if/else" incoming pointers were OK             */
    
    return iStatus;
}

/* Add an INT array to an INT array                                   */
int aAdd ( INT * aOne, INT * aTwo, INT ** aAnswer )
{
    /* This routine allocates space for the answer, which the caller  */
    /* must, at some time, release                                    */

    int iStatus = EXIT_SUCCESS;
    int i = 0;
    int j = 0;
    int k = 0;
    int iBefore = 0;
    int iAfter = 0;
    int iAllocateLength = 0;
    int iSign = 1;
    int bUseAdd = 1;
    INT carry = 0;
    INT * pInt;
    INT ** ppInt = &pInt;
    
    /*
    * This routine adds together two very large numbers. Each of the 
    * arguments is assumed to be in an array of INT. This routine    
    * allocates enough space for the answer, and computes the sum.   
    * The answer is normalized before returning to the caller.       
    */

    /*
    * Before performing the addition the numbers have to be aligned, 
    * so that the number of places after the point are the same for
    * both numbers. This means possible extending the shorted number.
    */
    
    /* Test the incoming pointers for validity:                       */
    if ((aOne==NULL) || (aTwo==NULL) || (aAnswer==NULL))
    {
        /* At least one of the arguments was bad: indicate to caller: */
        iStatus = EXIT_FAILURE;
        return iStatus;
    }
    else
    {
        /*
        * How many places are before and after the point in each 
        * of the addends?
        */
        iAfter = max(aOne[1],aTwo[1]);
        iBefore = max (abs(aOne[0])-aOne[1],abs(aTwo[0])-aTwo[1]);

        /*
        * Compute the maximum length of the answer array. This caters
        * for the worst case of carry upwards from the top word:     
        */
        iAllocateLength = iBefore + iAfter + 1;

        /* Allocate enough space for the answer, but only point to it */
        /* locally:                                                   */
        j = aAllocate(iAllocateLength, ppInt);
        /*
        * Now set the number of places after the point in the answer to
        * be the maximum number of places af the two incoming addends:
        */
        *(pInt+1) = iAfter;

        /*
        * We need to look at the signs of the incoming arguments. If 
        * these are the same, then we simply add the arguments, as if
        * they were both positive, and give the answer that sign. If,
        * however, they differ in sign, then we force them both positive
        * and do a subtraction. After the subtraction we determine the
        * outgoing sign. Hence we call either "internalAdd" or "internal-
        * Subtract" to do the real calculation. Note that each of these
        * internal routines normalizes its answer before returning, so
        * we can just hand that answer back to the caller (after any
        * adjustment of sign).
        */

        if ((*aOne>0) && (*aTwo>0))
        {
            bUseAdd = 1;
            iSign = 1;
        }
        else if ((*aOne<0) && (*aTwo<0))
        {
            bUseAdd = 1;
            iSign = -1;
        }
        else
        {
            bUseAdd = 0;
            iSign = 1;
        }

        if (bUseAdd==1)
            j = internalAdd ( aOne, aTwo, ppInt );
        else
        {
            /*
            * We know that we have to subtract - these elements
            * differ in sign. Place the positive element first.
            * We do NOT compare the absolute or signed values of
            * the two arguments here (using aCompare) - we only
            * need to know their signs:
            */
            if (*aOne>0)
                j = internalSubtract ( aOne, aTwo, ppInt );
            else
                j = internalSubtract ( aTwo, aOne, ppInt );
            /*
            * Note that the sign will have been computed by the
            * internalSubtract routine, so there is no need to
            * alter it in this routine.
            */
        }

        /* Correct the sign of the answer:                           */
        if (iSign<0)
            **ppInt = - (**ppInt);

        /* Tell caller the address of the answer:                     */
        aAnswer = ppInt;

    }       /* end of "if/else" incoming pointers were OK             */
    
    return iStatus;
}

/* Subtract an INT array from an INT array                            */
int aSubtract ( INT * aOne, INT * aTwo, INT ** aAnswer )
{
    /* This routine allocates space for the answer, which the caller  */
    /* must, at some time, release                                    */

    int iStatus = EXIT_SUCCESS;
    int i = 0;
    int j = 0;
    int k = 0;
    int iBefore = 0;
    int iAfter = 0;
    int iAllocateLength = 0;
    int iSign = 1;
    int bUseAdd = 1;
    INT carry = 0;
    INT * pInt;
    INT ** ppInt = &pInt;
    
    /*
    * This routine subtracts two very large numbers. Each of the 
    * arguments is assumed to be in an array of INT. This routine    
    * allocates enough space for the answer, and computes the difference.
    * The answer is normalized before returning to the caller.       
    */
    
    /* Test the incoming pointers for validity:                       */
    if ((aOne==NULL) || (aTwo==NULL) || (aAnswer==NULL))
    {
        /* At least one of the arguments was bad: indicate to caller: */
        iStatus = EXIT_FAILURE;
        return iStatus;
    }
    else
    {
        /*
        * How many places are before and after the point in each 
        * of the addends?
        */
        iAfter = max(aOne[1],aTwo[1]);
        iBefore = max (abs(aOne[0])-aOne[1],abs(aTwo[0])-aTwo[1]);

        /*
        * Compute the maximum length of the answer array. This caters
        * for the worst case of borrow upwards from/to the top word:     
        */
        iAllocateLength = iBefore + iAfter + 1;

        /* Allocate enough space for the answer, but only point to it */
        /* locally:                                                   */
        j = aAllocate(iAllocateLength, ppInt);
        /*
        * Now set the number of places after the point in the answer to
        * be the maximum number of places af the two incoming addends:
        */
        *(pInt+1) = iAfter;

        /*
        * We need to look at the signs of the incoming arguments. If 
        * these are the same, then we subtract the absolutely smaller
        * from the absolutely larger, and give the result the sign of
        * the absolutely larger. If the signs are different, then we
        * add together the absolute values, and give the result the
        * appropriate sign.
        */

        if ((*aOne>0) && (*aTwo<0))
        {
            bUseAdd = 1;
        }
        else if ((*aOne<0) && (*aTwo>0))
        {
            bUseAdd = 1;
        }
        else
        {
            /*
            * The signs are the same. Perform an internalSubtract
            */
            bUseAdd = 0;
            iSign = aAbsCompare ( aOne, aTwo );
            if (iSign<0)
            {
                /*
                * The first argument is smaller than the second. Take
                * the first from the second, and use the sign of the
                * second:
                */
                j = internalSubtract ( aTwo, aOne, ppInt );
                if (*aTwo<0)
                    *pInt = - *pInt;
            }
            else if (iSign>0)
            {
                /* 
                * The first argument is larger than the second. Take
                * the second from the first, and use the sign of the
                * first:
                */
                j = internalSubtract ( aOne, aTwo, ppInt );
                if (*aOne<0)
                    *pInt = - *pInt;
            }
            else
            {
                /*
                * This is the special case where we know that the two
                * arguments have the same sign AND are equal in magnitude
                * so the answer is zero. We simply allocate a zero array
                * (which we have already done), and normalize it:
                */
                j = aNormalize(ppInt, ppInt);
            }

            /*
            * We have the answer - return to caller, indicating failure
            * if detected:
            */
            if (j!=0)
                iStatus = EXIT_FAILURE;
            return iStatus;
        }

        /*
        * If control falls through to here, then we have to
        * add the absolute values together, and take the 
        * sign of the first as the outgoing sign:
        */
        j = internalAdd ( aOne, aTwo, ppInt );
        if (j!=0)
            iStatus = EXIT_FAILURE;

        if (*aOne<0)
            **ppInt = - **ppInt;


        /* Tell caller the address of the answer:                     */
        aAnswer = ppInt;

    }       /* end of "if/else" incoming pointers were OK             */
    
    return iStatus;
}

/* Add an INT array to an INT array, all pre-allocated, all positive  */
int internalAdd ( INT * aOne, INT * aTwo, INT ** aAnswer )
{

    int iStatus = EXIT_SUCCESS;
    int i = 0;
    int j = 0;
    int k = 0;
    int m = 0;
    int a1 = 0;
    int a2 = 0;
    int a3 = 0;
    int b3 = 0;
    INT v1 = 0;
    INT v2 = 0;
    INT w;
    INT carry = 0;
    INT * pInt;
    INT ** ppInt = &pInt;
    
    /*
    * This routine adds together two very large numbers. Each of the 
    * arguments is assumed to be in an array of INT. The sum is
    * computed and passed back in the third argument.
    * The pre-allocated answer array has enough places after the
    * opint to cater for the larger of the incoming addends.
    * The answer is normalized before returning to the caller.       
    */
    
    /*
    * We start at the right hand end of each input array,    
    * putting the answer into the right hand end of the new  
    * array. Note that we (a) keep "carry" up to date, and   
    * (b) we use value zero if the subscripts overshoot the  
    * incoming arrays (which they will):                     
    */
    /* Point to the ends of the incoming addends: */
    k = abs(aOne[0]);
    m = abs(aTwo[0]);

    /* Get the number of places after the point for each addedn: */
    a1 = aOne[1];
    a2 = aTwo[1];

    /* Compute the number of places after the point for the answer: */
    i = abs(**aAnswer);
    b3 = i - a3;

    /* Loop backwards along the two addedns, adding corresponding words: */
    for (; (i>1); i--)
    {
        /*
        * Compute the decimal position we are in the answer. This
        * may well be negative, when we are before the point:
        */
        a3 = i - b3;

        /*
        * If this decimal position is within the range of the first
        * addend, then use it, otherwise use zero:
        */
        if (a3<=a1)
        {
            v1 = aOne[k];
            k--;
        }
        else 
            v1 = 0;

        /*
        * and the same for the second addend as for the first:
        */
        if (a3<=a2)
        {
            v2 = aTwo[m];
            m--;
        }
        else 
            v2 = 0;

        /* Compute the sum: */
        w = v1 + v2 + carry;

        /* Have we overshot the maximum value we can place in */
        /* a single word?                                     */
        if (w>BASE)
        {
            carry = 1;
            w -= BASE;
        }
        else
            carry = 0;

        /* Set the answer from our result:                    */
        (*ppInt)[i] = w;

    }   /* end of "for/i" loop backwards along the answer     */

    /* Now normalize the answer:                              */
    k = aNormalize(aAnswer, aAnswer);

    /* Did the Normalize function indicated something wrong?  */
    if (k!=0)
        iStatus = EXIT_FAILURE;
    
    return iStatus;
}

/* Subtract an INT array from an INT array, all pre-allocated, & +ve  */
int internalSubtract ( INT * aOne, INT * aTwo, INT ** aAnswer )
{

    int iStatus = EXIT_SUCCESS;
    int i = 0;
    int j = 0;
    int k = 0;
    int m = 0;
    int a1 = 0;
    int a2 = 0;
    int a3 = 0;
    int b3 = 0;
    INT v1 = 0;
    INT v2 = 0;
    INT w;
    INT carry = 0;
    INT * pInt;
    INT ** ppInt = &pInt;
    
    /*
    * This routine subtracts two very large numbers. Each of the 
    * arguments is assumed to be in an array of INT. The difference
    *  "One minus Two"
    * is computed and passed back in the third argument.
    * The answer is normalized before returning to the caller.
    * Both of the incoming arguments are assumed to be positive, and
    * their actual signs are simply not inspected here. This is an
    * internal routine, and the callers (aAdd and aSubtract) will be 
    * making the appropriate sign corrections
    */
    
    /*
    * We start at the right hand end of each input array,    
    * putting the answer into the right hand end of the new  
    * array. Note that we (a) keep "carry" up to date, and   
    * (b) we use value zero if the subscripts overshoot the  
    * incoming arrays (which they will). "carry" is also "borrow".
    */
    k = abs(aOne[0]);
    m = abs(aTwo[0]);

    /* Get the number of places after the point for each addedn: */
    a1 = aOne[1];
    a2 = aTwo[1];

    /* Compute the number of places after the point for the answer: */
    i = abs(**aAnswer);
    b3 = i - a3;

    for (; (i>1); i--)
    {
        /*
        * Compute the decimal position we are in the answer. This
        * may well be negative, when we are before the point:
        */
        a3 = i - b3;

        /*
        * If this decimal position is within the range of the first
        * addend, then use it, otherwise use zero:
        */
        if (a3<=a1)
        {
            v1 = aOne[k];
            k--;
        }
        else 
            v1 = 0;

        /*
        * and the same for the subtrahend as for the minuend:
        */
        if (a3<=a2)
        {
            v2 = aTwo[m];
            m--;
        }
        else 
            v2 = 0;

        /* Perform the subtraction, including the carry (borrow): */
        w = v1 - v2 - carry;

        /*
        * Have we overshot the maximum value we can place in 
        * a single word (improbable), or is the answer word  
        * negative? Force the answer word positive and in range: 
        */
        if (w>BASE)
        {
            carry = -1;
            w -= BASE;
        }
        else if (w<0)
        {
            carry = 1;
            w += BASE;
        }
        else
            carry = 0;

        /* Set the answer from our result:                    */
        (*ppInt)[i] = w;

    }   /* end of "for/i" loop backwards along the answer     */

    /*
    * At this stage the "carry" flag indicates the sign of   
    * the final answer:                                      
    *    carry    output sign
    *      -1     error in algorithm
    *       0     positive
    *      +1     negative
    *
    * If the output is negative we only need to flip the sign
    * of the normalized result.
    */

    /* Now normalize the answer:                                      */
    k = aNormalize(aAnswer, aAnswer);

    /*
    * Did the Normalize function indicate something wrong, or
    * was there an error in the subtraction algorithm?
    */
    if ((k!=0) || (carry<0))
        iStatus = EXIT_FAILURE;

    /* Set the outgoing sign:                                         */
    if (carry>0)
        **aAnswer = - (**aAnswer);
    
    return iStatus;
}

/* Compare the values of two very large numbers                      */
int aCompare ( INT * aOne, INT * aTwo )
{
    int iAnswer = 0;
    int i = 0;
    int j = 0;
    int k = 0;

    /*
    * This routine compares the absolute valus of two very large
    * numbers passed in. 
    * The returned value is one of:
    *     -1     first argument is smaller than second
    *      0     first and second arguments are equal
    *     +1     first argument is greater than second
    * The routine tries to take shortcuts to getting the answer, to
    * avoid having to scan down the whole of both arguments.
    * Note that both arguments are assumed to have been normalized
    * by the caller, so that there are no excess leading zeros.
    */

    /* Firstly, can we decide merely from the signs of the numbers?   */
    i = *aOne;
    j = *aTwo;

    if ((i>0) && (j<0))
    {
        iAnswer = +1;
        return iAnswer;
    }
    else if ((i<0) && (j>0))
    {
        iAnswer = -1;
        return iAnswer;
    }
    /*
    * If control falls through, then the two arguments are the same
    * sign, so we have to compare their absolute values:
    */

    iAnswer = aAbsCompare ( aOne, aTwo );
    /*
    * If the arguments are different, and both are negative, then
    * we need to invert the sense of the answer we have found, as
    * currently we have the absolute value comparison here:
    */
    if ((iAnswer!=0) && (*aOne<0))
        iAnswer = - iAnswer;

    return iAnswer;
}

/* Compare the absolute values of two very large numbers              */
int aAbsCompare ( INT * aOne, INT * aTwo )
{
    int iAnswer = 0;
    int i = 0;
    int j = 0;
    int k = 0;
    int b1 = 0;
    int a1 = 0;
    int b2 = 0;
    int a2 = 0;

    /*
    * This routine compares the absolute valus of two very large
    * numbers passed in. 
    * The returned value is one of:
    *     -1     absolute value of first argument is smaller than second
    *      0     first and second arguments are equal
    *     +1     absolute value of first argument is greater than second
    * The routine tries to take shortcuts to getting the answer, to
    * avoid having to scan down the whole of both arguments.
    * Note that both arguments are assumed to have been normalized
    * by the caller, so that there are no excess leading zeros.
    */

    /* Get the lengths of the two arguments: */
    i = abs(*aOne);
    j = abs(*aTwo);

    /*
    * Remember the length of the smaller, as that is the maximum
    * length of scan:
    */
    k = min(i,j);

    /* Get the number of places before and after the point for both: */
    a1 = *(aOne + 1);
    b1 = i - a1;
    a2 = *(aTwo + 1);
    b2 = j - b2;

    /* If the pre-point lengths are not equal, then we know the answer: */
    if (b1<b2)
        iAnswer = -1;
    else if (b1>b2)
        iAnswer = 1;
    else
    {
        /*
        * The two arguments are equal in length pre-point. Scan from left
        * to right till we find the first word that differs, if any:
        */
        for (i=2; ((i<=k) && (iAnswer==0)); i++)
        {
            if ((*aOne+i)!=(*aTwo+i))
            {
                /* There is a difference. Which way is it? */
                if ((*aOne+i)<(*aTwo+i))
                    iAnswer = -1;
                else
                    iAnswer = 1;
            }   /* end of "if/then" there is a difference         */
        }       /* end of "for/i" scanning down the arguments     */
    }           /* end of "if/else" arguments are the same length */

    /*
    * We have scanned both numbers fully, and may not have found a difference.
    * If the two numbers were equal in length, then they were really
    * equal. If one is longer than the other then (because they were
    * both normalized prior to this test) the longer number is greater.
    */
    if (iAnswer==0)
    {
        if (i>j)
            iAnswer = 1;
        else if (i<j)
            iAnswer = -1;
    }

    return iAnswer;
}

/* Allocate and initialize to zero an INT array                       */
int aAllocate ( int iCount, INT ** aAnswer )
{
    int iStatus = EXIT_SUCCESS;
    INT * pInt = NULL;
    int i = 0;
    
    /*
    * This allocates an array of "iCount+1" INT's, and sets the 
    * first of these INT's to iCount, and all the rest to zero:      
    */
    pInt = calloc ( sizeof (INT), iCount + 1);
    
    if (pInt==NULL)
    {
        /* We could not allocate the memory: tell the caller          */
        iStatus = EXIT_FAILURE;
        return iStatus;
    }
    else
    {
        /* Set the body of the new array to zero:                     */
        for (i=1; (i <= iCount); i++)
            pInt[i] = 0;

        /* Set the counter word to indicate the length of the array:  */
        *pInt = iCount;
    }
    
    /* Provided that the incoming pointer can be used, tell caller:   */
    if (aAnswer!=NULL)
        *aAnswer = pInt;

    return iStatus;
}


