/*
*
*  This code is part of Chapter 24 of "C Unleashed", and is to be     
*  read in conjunction with that chapter. It is NOT code of production
*  quality, and should not be so considered.
*
*  Code by Ian D. K. Kelly
*
*/

/* Ch24Abad.c
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
* These are sample routines for three of the basic four arithmetic   
* routines for extremely large numbers. This sample deals mostly with
* positive numbers (hence there is no mention here of subtraction).  
* The very large numbers are also assumed to be integers - whole.    
* All very large numbers are assumed to be stored in arrays of int.  
* the first element of each of these arrays is a counter of the total
* number of further elements that exist in that array. The value of  
* the number being stored in from left to right from most significant
* to the least significant. That is, the lowest subscripts have the  
* largest part of the number.                                        
*
* Each routine, after computing its answer, tries to normalize it. In
* the context of this example code, "normalized" means "having the   
* fewest possible number of leading zeros".                          
*
*/

/*
*
* The number representation can be though of as using an extremely   
* large base. For our ordinary counting we use base 10 (ten), if we  
* are using octal, then the base is 8, for hexadecimal 16, and so on.
* We set here a base which is large enough to ensure that we can hold
* very large numbers, but small enough to ensure that the addition of
* two numbers does NOT overflow a word. That is, (2 * BASE) < MAX_INT
* There is one other restriction on the size of BASE which you can   
* learn by careful inspection of the code. What is it?               
*
*
* We do not HAVE to choose a base which is a power of ten, but it    
* makes the printing of the answers so much simpler if we do.        
* The base chosen, for 32-bit int, is ten thousand, which is a power 
* of ten, and whose square root is 100 (one hundred).                
*
* As an exercise you might like to look at what is required to change
* this base to one hundred million (ten to the eight), whose square  
* root is ten thousand (ten to the four). You will need to be very   
* cautious about overflow during multiplication and division. There  
* are some hints about this in the implementation of "pairMultiply". 
*
*/

#if (INT_MAX>100000000)

/* INT_MAX is large enough to use the definitions described above:    */
#define BASE 10000
#define SQRT_BASE 100
typedef int INT;

#else

/*
*
* INT_MAX is not large enough. We shall assume, without further testing
* (though we should test!) that "int" is 16 bit, and UINT_MAX is 
* at least 20000. It will be, in fact, at least 65535. Note that this
* means we have to change the definition of all our work variables   
* from being "int" to being "unsigned int". That is why we declare   
* all of them to be of type INT                                      
* By the way, O careful reader, there is another problem here: can   
* you find it? and can you fix it? The solution to this problen may
* be found in the file Ch24AOK.c on the CD.
*
*/
#define BASE 10000
#define SQRT_BASE 100
typedef long int INT;

#endif

int pairMultiply ( INT iOne, INT iTwo, INT * pAnswer);
int cascMultiply  ( INT * iOne, INT * aTwo, INT ** aAnswer );
int aMultiply ( INT * aOne, INT * aTwo, INT ** aAnswer);

int oneDivide ( INT * iNumerator, INT * iDenominator, INT * iAnswerOD );
int pairDivide ( INT * iNumerator, INT * iDenominator, INT * iAnswerPD );
int cascDivide ( INT * aNumerator, INT * iDenominator, INT ** aAnswer );
int aDivide ( INT * aNumerator, INT * aDenominator, INT ** aAnswer );
int aDivNormalize ( INT * pinNumerator, INT ** ppoutNumerator );

int aAdd ( INT * aOne, INT * aTwo, INT ** aAnswer );

int aNormalize ( INT ** aUnNormal, INT ** aNormal );
int aAllocate ( int iCount, INT ** aAnswer );

/* Multiply two INTs                                                  */
int pairMultiply ( INT iOne, INT iTwo, INT * pAnswer)
{
    /* This routine does not perform any internal allocations         */

    /*
    * Multiply two INTs giving an array of two INTs. The array is    
    * needed, becasue the answer (the product) may well have many more
    * digits than the incoming multiplicands.                        
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
    * so that we can never exceed the size of an INT during calculation
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
    
    /*
    * Consider the two-by-two multiplcation:                         
    *                 a   b                                          
    *                 c   d                                          
    *           --------------                                       
    *                da  db                                          
    *            ca  cb                                              
    *           --------------                                       
    *            ca, da+cb db                                        
    *           ==============                                       
    * where we have put spaces between the "sqrt_base" units, and a  
    * comma at "base". This explains the following inscrutable lines 
    */
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

    /* Set the callers answer array:                                  */
    pAnswer[0] = iAnsTop;
    pAnswer[1] = iAnsBot;

    return iStatus;
}

/* Multiply an INT array by one INT                                   */
int cascMultiply  ( INT * iOne, INT * aTwo, INT ** aAnswer )
{
    /*
    * This routine allocates space for the answer, which the caller  
    * must, at some time, release                                    
    */
    int iStatus = EXIT_SUCCESS;
    INT * pInt;
    INT ** ppInt = &pInt;
    INT w[2];
    int i = 0;
    int j = 0;
    int k = 0;
    int carry = 0;

    w[0] = 0;
    w[1] = 0;

    /*
    * The length of the product is at most one element longer than   
    * the incoming array. Allocate internal space for calculation:   
    */
    i = (*aTwo) + 1;
    k = aAllocate ( i, ppInt );

    /*
    * We multiply from the right to the left, bringing the carry over
    * and calculating the outgoing carry:                            
    */
    for (j=*aTwo; (j>0); j--)
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
    * The vary last carry is used to set the topmost word in the
    * answer. This is the most significant word:
    */
    *(pInt+1) = carry;

    /* Now we normalize the answer, and hand it back to the caller:   */
    i = aNormalize ( ppInt, aAnswer );

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
    * works out the length required for the answer, allocates    
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
        * The incoming pointers seem to be OK. So now determine the  
        * size of the output array required:                         
        */
        i = abs(aOne[0]) + abs(aTwo[0]) + 1;

        /* Allocate enough space for the answer:                      */
        j = aAllocate(i, ppInt);

        /* Did this allocation work? if not, tell caller:             */
        if (j!=0)
        {
            iStatus = EXIT_FAILURE;
        }
        else
        {
            /*
            * The allocation appeared to work. Now compute the product 
            * of the two incoming, by computing the pairwise product
            * of each pair of incoming INT's, shifting up each time.
            *  
            * Note that multiplication proceeds from right to left:  
            * that is, we use the RIGHTmost digits first - just as we
            * do on pencil and paper.                                
            */
            for (k=abs(aTwo[0]); (k>0); k--)
            {
                for (j=abs(aOne[0]); (j>0); j--)
                {
                    /*
                    * Compute the pairwise product of One[j] by      
                    * Two[k], and place the result in answer[L-j]    
                    * where L = i + 1 - k                            
                    * Note that this may overflow, so we have to add 
                    * the high end of our answer into the next answer
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
            {
                pInt[0] = - pInt[0];
            }

            /*
            * At this point we have in the local array the possibly  
            * un-normalized product of the input arrays. Normalize:  
            */
            m = aNormalize (ppInt, aAnswer);

            if (m!=0)
            {
                /* There was a problem in normalization. Tell caller: */
                iStatus = EXIT_FAILURE;
            }   /* end of "if/then" there was a normalization problem */
        }   /* end of "if/else" allocation worked                     */
    }   /* end of "if/else" the incoming pointers were OK             */

    return iStatus;
}

/* Divide one INT by another INT, getting quotient and remainder      */
int oneDivide ( INT * iNumerator, INT * iDenominator, INT * iAnswerOD )
{
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

    /* The algorithm used is that of Knuth, Vol 3. 4.3.1, Ex 16       */

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
int cascDivide ( INT * aNumerator, INT * iDenominator, INT ** aAnswer )
{
    /*
    * This routine allocates space for the answer, which the caller  
    * must, at some time, release                                    
    */

    /*
    * This routine divides the long number, in the INT array pointed 
    * to by the first argument, by the single INT pointed to by the  
    * second argument. Space for the result is allocated. The final  
    * output (pointed to by the pointer the third argument points to)
    * is normalized.                                                 
    */

    int iStatus = EXIT_SUCCESS;
    int j = 0;
    int k = 0;
    INT r = 0;
    INT w = 0;
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

    /* The algorithm used is again that of Knuth, Vol 3. 4.3.1, Ex 16 */

    r = 0;
    k = *aNumerator;
    aAllocate(k, ppInt);
    for (j=1; (j<=k); j++)
    {
        /*
        * It is the following statement (and its parallel in pair-   
        * Divide) that creates the further restriction upon the size  
        * of BASE, which must be no greater than the suqre root of   
        * INT_MAX. This is a much tighter restriction than all the   
        * others. To avoid it, you must find a different algorithm   
        * for division which does not involve multiplying BASE by the
        * remainder - which might be anything up to BASE-2.          
        */

         w = (r * BASE + *(aNumerator + j));
         *(pInt + j) = w / *(iDenominator);
         r = w % *(iDenominator);
    }

    /*
    * Note that at the very end we have an outgoing, residual        
    * carry. This is simply lost.                                    
    */

    /* Normalize the intermediate answer into the final answer:   */
    /* Note that this also frees the intermediate work area       */
    j = aNormalize ( ppInt, aAnswer );

    return iStatus;
}

/* Divide an INT array by an INT array, getting just the quotient     */
int aDivide ( INT * aNumerator, INT * aDenominator, INT ** aAnswer )
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
    INT wN[2];
    int borrow = 0;
    int iNmax;
    int iDmax;
    INT carry = 0;
    INT iBase = BASE;
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
    /* Space for three work areas required during the calculation. If */
    /* you consider the code carefully you should find that this set  */
    /* of work areas can be removed entirely. A sample solution for   */
    /* this may be found in the file CH24AOK.c on the CD              */
    INT * plSubtrahend = NULL;
    INT ** pplSubtrahend = &plSubtrahend;
    INT * plMinuend = NULL;
    INT ** pplMinuend = &plMinuend;
    INT * pInter = NULL;
    INT ** ppInter = &pInter;

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

    /* Copy from argument to local copy:                              */
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
    * release the previous work area:                                
    */
    cascMultiply ( &bigD, pInter, pplDenominator );
    free ( pInter );

    /*
    * Find out how much space to alocate for a local copy of the 
    * numerator, and get the space, multiplying up at the same time:    
    */
    pplNumerator = &plNumerator;
    plNumerator = NULL;
    cascMultiply ( &bigD, aNumerator, ppInter );

    /*
    * Perform "Division Normalize", which inserts a leading zero, and
    * release the intermediate work area:                            
    */
    k = aDivNormalize ( pInter, pplNumerator );
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
    for (j=1;(j<=m);j++)
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
            for (i=1; (i<=(n+1)); i++)
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
                * set the "borrow" flg. If it is positive or zero then
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
                    }   /* end of "for/i" replace protion of numerator    */
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
                for (i=n+1; (i>0); i--)
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

        /* Get rid of the intermediate work areas, and loop back      */
        /* It is left as an exercise to the reader to place the "free"*/
        /* statements correctly in this routine. It is not quite as   */
        /* obvious as you might at first think. Also consider: are    */
        /* any more "free" statements required? Is this routine memory*/
        /* safe? How could this routine be rephrased so as to require */
        /* less allocation/de-allocation?                             */
        /* This file will NOT compile correctly, as these statments do*/
        /* need to be placed correctly!                               */
#error "Fix the placement of the following free code"
#if (0)
        free ( pInter );
        pInter = NULL;
        free ( pInter2 );
        pInter2 = NULL;
#endif

        /* Clearly the allocation and de-allocation of these work     */
        /* areas, pInter and pInter2, within the central body of this */
        /* loop is very inefficient. Wilst you are considering how to */
        /* remove all the work areas, you could begin by considering  */
        /* how to move their allocation and de-allocation to outside  */
        /* of this loop.                                              */

    }   /* end of "for/j" major loop along digits of numerator        */

    /* Knuth D8:                                                      */
    /* Normalize the result:                                          */
    k = aNormalize ( pplQuotient, aAnswer );

    /* Free the allocated intermediate work areas:                    */
    /* See the above excercise to consider placement.                 */
#error "Fix the placement of the following free code"
#if (0)
    free ( plSubtrahend );
    plSubtrahend = NULL;
    free ( plMinuend );
    plMinuend = NULL;
    free ( plQuotient );
    plQuotient = NULL;
    free ( plNumerator );
    plNumerator = NULL;
    free ( plDenominator );
    plDenominator = NULL;
#endif

    return iStatus;
}


/* Allocate an INT array for the aDivide internal operation which is  */
/* one longer than the incoming array:                                */
int aDivNormalize ( INT * pinNumerator, INT ** ppoutNumerator )
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
    iAllocLength = iNmax + 1;

    /* make the allocation:                                           */
    i = aAllocate ( iAllocLength, ppAnswer );

    /* Now copy the original into the new area.                       */
    /* Set up the initial zero:                                       */
    *(pAnswer + 1) = 0;

    /* Now copy the rest of the input array to the output:            */
    for (k=2; (k <= iAllocLength); k++)
    {
        *(pAnswer + k) = *(pinNumerator + k - 1);
    }

    /* Point the answer to the newly allocated:                      */
    *ppoutNumerator = pAnswer;

    return iStatus;
}

/* Normalize an INT array                                             */
int aNormalize ( INT ** aUnNormal, INT ** aNormal )
{
    /*
    * Although this routine may allocate space for its answer, if it 
    * does so it frees the space for its first argument. Hence the   
    * caller does not have to consider whether to free the returned  
    * space from this routine.                                       
    */

    int iStatus = EXIT_SUCCESS;
    int i = 0;
    int j = 0;
    int k = 0;
    int m = 0;
    int s = 1;
    INT * plNormal = NULL;
    INT ** pplNormal = &plNormal;
    
    /*
    * This function takes two arguments. The first is a pointer to   
    * a pointer to an array of INT. This array may be un-normalized. 
    * the second argument is a pointer to a pointer. This latter is  
    * not yet pointing to anything, but will (at the termination of  
    * this routine) be an array of INT. This will be a normalized    
    * copy of the first argument. If the first array is noral then   
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
        k = abs((*aUnNormal)[0]);
        if ((*aUnNormal)[0]<0)
        {
            s = -1;
        }

        /*
        * If there is anything at all in the first array, then count 
        * the number of zero words with which it begins:              
        */
        if (k>0)
        {
            j = (*aUnNormal)[1];
            m = 0;
            for (i = 1; ((j==0)&&(i<=k)); i++)
            {
                j = (*aUnNormal)[i];
                if (j==0)
                    m++;
            }
        }
        
        /*
        * At this point we know that 'm' contains a count of the     
        * number of zero words at the start of the UnNormal input.   
        * So we need to (a) allocate a new array, and (b) copy the   
        * old into the new, and (c) deallocate the old array:        
        */
        if (m>0)
        {
            /* Allocate the new normal array:                         */
            k = abs((*aUnNormal)[0]) - m;

            /* Note that a normal zero has at least one leading word: */
            /* do not normalize away all the digits!                  */
            if (k<=0)
            {
                k = 1;
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
            if (s<0)
            {
                *plNormal = - (*plNormal);
            }

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
    int m = 0;
    INT w;
    INT carry = 0;
    INT * pInt;
    INT ** ppInt = &pInt;
    
    /*
    * This routine adds together two very large numbers. Each of the 
    * arguments is assumed to be in an array of INT. This routine    
    * allocates enough space for the answer, and computes the sum.   
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
        * Compute the maximum length of the answer array. This caters
        * for the worst case of carry upwards from the top word:     
        */
        i = max(abs(aOne[0]), abs(aTwo[0])) + 1;

        /* Allocate enough space for the answer, but only point to it */
        /* locally:                                                   */
        j = aAllocate(i, ppInt);

        if (j==0)
        {
            /*
            * We start at the right hand end of each input array,    
            * putting the answer into the right hand end of the new  
            * array. Note that we (a) keep "carry" up to date, and   
            * (b) we use value zero if the subscripts overshoot the  
            * incoming arrays (which they will):                     
            */
            k = abs(aOne[0]);
            m = abs(aTwo[0]);
            for (; (i>0); i--)
            {
                w = (k>0) ? aOne[k] : 0 + 
                    (m>0) ? aTwo[m] : 0 + 
                    carry;

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

                /* Get ready to process the next words to the left, by*/
                /* decrementing their subscripts:                     */
                m--;
                k--;

            }   /* end of "for/i" loop bacwards along the answer      */

            /* Now normalize the answer:                              */
            k = aNormalize(ppInt, aAnswer);

            if (k!=0)
            {
                /* The Normalize function indicated something wrong:  */
                iStatus = EXIT_FAILURE;
            }
        }   /* end of "if/then" the initial allocation was OK         */
        else
        {
            /* The initial allocation indicated an error. Pass this   */
            /* back to the caller:                                    */
            iStatus = EXIT_FAILURE;
        }
    }       /* end of "if/else" incoming pointers were OK             */
    
    return iStatus;
}

/* Allocate and initialize to zero an INT array                       */
int aAllocate ( int iCount, INT ** aAnswer )
{
    int iStatus = EXIT_SUCCESS;
    INT * pInt = NULL;
    int i = 0;
    
    /*
    * This has to allocate an array of "iCount+1" INT's, and set the 
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
        {
            pInt[i] = 0;
        }

        /* Set the counter word to indicate the length of the array:  */
        *pInt = iCount;
    }
    
    /* Provided that the incoming pointer can be used, tell caller:   */
    if (aAnswer!=NULL)
    {
        *aAnswer = pInt;
    }

    return iStatus;
}


