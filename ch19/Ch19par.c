/* Code by Ian D. K. Kelly for Chapter 19, "C Unleashed"    */
/* Parse an arithmetic expression, and plant simple code    */

/* Ch19Par.c
*
*  Parse an arithmetic expression against a given syntax definition.
*
*  Copyright (C) 2000  Ian D. K. Kelly,
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
#include <string.h>
#include <ctype.h>

#include <assert.h>

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#include "Ch19Par.h"
#if 1
#define WAIT {char dummy[6]; printf("pak\n"); fgets(dummy,6,stdin);}
#else
#define WAIT
#endif

/*
*  We need to be able to:
*    1) read in the abstract syntax, and construct from it the
*       syntax tables (tree) which will be used during the parsing
*    2) from the abstract syntax we need to construct the (sparse)
*       "must contain" and "can start with" tables
*    3) read in and parse a possible expression
*    4) generate code for the parsed expression
*/

/*
*  This is the more elaborate version of the parse program n this
*  CD. It contains some of the techniques for speeding up the parse
*  process, and attempts to produce more comprehensible error messages.
*/

/* ====== Read abstract syntax, Free abstract syntax ================ */

/*
*  The abstract syntax will be stated in a file whose filename is
*  in the string variable "sSyntaxFileName". The file pointer for
*  this file is "fSyntaxFile".
*  The format of the syntax is a series of definitions, with each
*  definition commencing on a new line.
*  A definition starts with a syntax name followed by a space followed
*  by its alternates. Each alternate is separated from the next by a
*  dollar sign.
*  Each alternate consists of a series of items. An item is a name
*  (which represents one of the syntax definitions), or a character
*  string enclosed in double-quotes, or a procedure name enclosed in
*  square brackets., or a procedure name enclosed in round brackets.
*  Each procedure name must be a procedure that is available for either
*  syntax verification within this parser (the names enclosed in round
*  brackets), or for generating a part of the output code (the names
*  enclosed in square brackets).
*/

int ReadSyntax ( struct sSyntaxTableElement* SyntaxTable,
                 struct sAlternateTableElement* AlternateTable,
                 struct sRoutineNameTableElement* RoutineNameTable,
                 char* SyntaxLine,
                 struct sSyntaxHead** pRoot, char* sSyntaxFileName )
{
    int iStatus = TRUE;
    struct sSyntaxHead* pRootSyntax = NULL;

assert(SyntaxTable!=NULL);
assert(AlternateTable!=NULL);
assert(RoutineNameTable!=NULL);
assert(SyntaxLine!=NULL);
assert(pRoot!=NULL);
assert(sSyntaxFileName!=NULL);

    pRootSyntax = *pRoot;
    fSyntaxFile = fopen ( sSyntaxFileName, "r" );
    if (fSyntaxFile==NULL)
    {
        /* ERROR - the syntax file could not be opened */
        iStatus = FALSE;
        return iStatus;
    }

    /*
    *  Read in each syntax line, to the end of file, and then
    *  close the file. The first definition is assumed to be
    *  the root that we are aiming for.
    */
    while (!feof(fSyntaxFile))
    {
        iStatus = ProcessSyntaxLine ( SyntaxTable, AlternateTable,
                                      RoutineNameTable, SyntaxLine, pRootSyntax );
        if ( *pRoot == NULL )
        {
            *pRoot = SyntaxTable[0].pSyntaxPointer;

        }
    }

    fclose(fSyntaxFile);

    return iStatus;
}

/* ------------------------------------------------------------------ */

/*
*  Examine the syntax table for an entry containing this name. If it
*  is found, return its subscript. If it is not found, create one,
*  increase the maximum counter, and return its subscript.
*/
int FindSyntaxName ( struct sSyntaxTableElement* SyntaxTable,
                     char* SyntaxName )
{
    int iReturn = 0;
    int i;
    int j;

    j = iNextSyntax;
    /*
    *  If there is an existing entry in the table, then look in the
    *  table for a match:
    */
    if (j>0)
    {
        for (i=0;((iReturn==0)&&(i<iNextSyntax));i++)
        {
            if (strcmp(SyntaxName,SyntaxTable[i].SyntaxName)==0)
            {
                iReturn = i;
            }
        }
    }

    /* If no match has yet been found, create a new table entry: */
    if (iReturn==0)
    {
        iReturn = CreateSyntaxTableEntry ( SyntaxTable, SyntaxName );
        /* Create the new table entry: the name */
#if 0
        strcpy(SyntaxTable[iNextSyntax].SyntaxName,SyntaxName);
        /* Set the pointers to empty: */
        SyntaxTable[iNextSyntax].pSyntaxPointer  = NULL;
        SyntaxTable[iNextSyntax].iMustContain = 0;
        SyntaxTable[iNextSyntax].iStartsWith  = 0;
        iReturn = iNextSyntax;
        /* Set the maximum count up by one: */
        iNextSyntax++;
#endif
    }

    return iReturn;
}

/* ------------------------------------------------------------------ */

int CreateSyntaxTableEntry (struct sSyntaxTableElement* SyntaxTable,
                            char* pSyntaxName)
{
    int iStatus;

    strcpy(SyntaxTable[iNextSyntax].SyntaxName,pSyntaxName);
    /* Set the pointers to empty: */
    SyntaxTable[iNextSyntax].pSyntaxPointer  = NULL;
    SyntaxTable[iNextSyntax].iMustContain = 0;
    SyntaxTable[iNextSyntax].iStartsWith  = 0;
    iStatus = iNextSyntax;
    /* Set the maximum count up by one: */
    iNextSyntax++;

    return iStatus;
}

/* ------------------------------------------------------------------ */

void FreeWholeSyntax ( struct sSyntaxTableElement* SyntaxTable )
{
    int i;

    for (i=0;(i<iNextSyntax);i++)
    {
        if ( SyntaxTable[i].pSyntaxPointer!=NULL )
        {
            FreeSyntaxHead ( SyntaxTable[i].pSyntaxPointer );
            SyntaxTable[i].pSyntaxPointer = NULL;
        }
    }

    return;
}

/* ------------------------------------------------------------------ */

int GetSyntaxName ( struct sSyntaxTableElement* SyntaxTable,
                    char* SyntaxLine, int* j )
{
    struct sSyntaxHead* pNewSyntaxHead;
    char SyntaxName[SYNTAX_NAME_LENGTH+1];
    int i;
    int m;
    int iMySyntaxNumber;

    strcpy(SyntaxName,"");
    m = 0;
    for (i=0; (m==0); i++)
    {
        if (isidchar(SyntaxLine[i + *j]))
        {
            SyntaxName[i] = SyntaxLine[i + *j];
            SyntaxName[i + 1] = '\0';
        }
        else
        {
            m = i;
            *j += i;
        }
    }

    iMySyntaxNumber = FindSyntaxName ( SyntaxTable, SyntaxName );
    if (SyntaxTable[iMySyntaxNumber].pSyntaxPointer==NULL)
    {
        /*
        *  This is the defining entry for this name.
        *  Get and initialise a new SyntaxHead head item
        *  The table entry can then point to this tree item.
        */
        pNewSyntaxHead = malloc ( sizeof (struct sSyntaxHead) );
        if(pNewSyntaxHead == NULL)
        {
          printf("Out of memory on line %d - exiting.\n", __LINE__);
          exit(EXIT_FAILURE);
        }
#ifdef PARDEBUG
        strcpy((pNewSyntaxHead)->SyntaxHeadIdent,"SHD");
#endif
        SyntaxTable[iMySyntaxNumber].pSyntaxPointer  = pNewSyntaxHead;
        /*
        *  Set the name, and all the other fields to empty or null:
        */
        strcpy(pNewSyntaxHead->SyntaxName,SyntaxName);
        pNewSyntaxHead->iSyntaxNumber  = iMySyntaxNumber;
        pNewSyntaxHead->iStartsWith    = 0;
        pNewSyntaxHead->iMustContain   = 0;
        pNewSyntaxHead->FirstAlternate = NULL;
        pNewSyntaxHead->LexRoutine     = NULL;
        pNewSyntaxHead->iIsLexical     = FALSE;
        pNewSyntaxHead->pNextHead      = NULL;
        if (iMySyntaxNumber>0)
        {
            /*
            *  We know that there is a real previous name - so we need to
            *  chain this new element to the one before. This involves
            *  setting this back pointer to the element before, and
            *  the forward pointer of the element before to this one:
            */
            pNewSyntaxHead->pPreviousHead =
                           SyntaxTable[iMySyntaxNumber-1].pSyntaxPointer;
            (SyntaxTable[iMySyntaxNumber-1].pSyntaxPointer)->pNextHead =
                           pNewSyntaxHead;
        }
        else
        {
            pNewSyntaxHead->pPreviousHead  = NULL;
        }
    }
    return iMySyntaxNumber;
}

/* ------------------------------------------------------------------ */

int isidchar ( char testchar )
{
    int iStatus;

    /*
    *  Test the character being presented, and determine whether it
    *  can be part of an identifier or not. The characters allowed
    *  are letters, digits and the underscore. All other characters
    *  are forbidden:
    */

    if ((isalpha(testchar))||(isdigit(testchar)||(testchar=='-')))
        iStatus = TRUE;
    else
        iStatus = FALSE;

    return iStatus;
}

/* ------------------------------------------------------------------ */

int GetIdentifier ( char* InputBuffer, char* Identifier, int* j )
{
    int iStatus = FALSE;
    int i = 0;

    /*
    *  Get the identifier from the input buffer, stopping at the
    *  first character that cannot be in an identifier (under our
    *  definitions):
    */

    while ((isidchar(*(InputBuffer+*j))) && (i<ROUTINE_NAME_LENGTH))
    {
        /*  The character is good: copy it to the caller: */
        Identifier[i++] = *(InputBuffer+*j);
        /*  And indicate the advance of the input pointer: */
        (*j)++;
        /*
        *  Since we have had one good character, indicate success
        *  to the caller. If there are no good characters in the
        *  input, then we will be indicating failure:
        */
        iStatus = TRUE;
    }

    /*  Put in the string termination character: */
    Identifier[i] = '\0';

    return iStatus;
}

/* ------------------------------------------------------------------ */

int GetLexName ( struct sSyntaxTableElement* SyntaxTable,
                 char* SyntaxLine, char* Identifier, int* j, int iMySyntaxNumber )
{
    int    iMyLexNumber = -1;

    while (SyntaxLine[*j]==' ')
        (*j)++;

    /*
    *  We are now either pointing to the first colon of ::= or
    *  :L:= OR we are pointing at the opening left curly of
    *  {name}, where "name" is the name of a routine to be called
    *  at lexical analysis time:
    */
    if ((SyntaxLine[*j]=='{') || (SyntaxLine[*j]=='(')
    ||  (SyntaxLine[*j]=='['))
    {
        /*
        *  Point to the first character of the identifier in
        *  the input line, advancing past the left bracket that
        *  we have just seen:
        */
        (*j)++;
        /*
        *  Fetch the identifier:
        */
        iMyLexNumber = GetIdentifier ( SyntaxLine, Identifier, j );
        /*
        *  Skip the terminating right bracket. There could be an
        *  error check here to verify that it really IS a right
        *  bracket of the same type as the opening bracket (square,
        *  brace or parenthesis).
        *    iStatus = (SyntaxLine[(*j)++]=='}');
        */
        (*j)++;
        /*
        *  and skip past any terminating white space:
        */
        while ((SyntaxLine[*j]==' ') || (SyntaxLine[*j]=='\t'))
        {
            (*j)++;
        }
    }

    if (strncmp(&SyntaxLine[*j],":L:=",4)==0)
    {
        SyntaxTable[iMySyntaxNumber].pSyntaxPointer->iIsLexical = TRUE;
        (*j)+=4;
    }
    else
    {
        SyntaxTable[iMySyntaxNumber].pSyntaxPointer->iIsLexical = FALSE;
        (*j)+=3;
    }

    /*
    *  Note that if we did not get a good identifier, then this
    *  routine passes that error back to its caller:
    */
    return iMyLexNumber;
}

/* ------------------------------------------------------------------ */

int GetRoutinePointer ( struct sRoutineNameTableElement* RoutineNameTable,
                        char* pszRoutineName, int (**FoundRoutine) (void*) )
{
    int iStatus = FALSE;
    int i;

    /*
    *  Scan the routine name table for the matching name, if any, and
    *  return a pointer to the real routine. If there is no match, then
    *  there is an error, and this routine indicates FALSE to its caller
    */

    /*
    *  Initial answer pointer is NULL:
    */
    *FoundRoutine = NULL;

    for (i=0;(i<=GLOBMAXNAME);i++)
    {
        if (strcmp(pszRoutineName, RoutineNameTable[i].sNameBody ) == 0)
        {
            /*
            *  The match has been found, so return the pointer to
            *  the real routine, indicate success, and return to caller
            */
            *FoundRoutine = RoutineNameTable[i].BodyRoutine;
            iStatus = TRUE;
            return iStatus;
        }
        /*
        *  This was not the match - go try the next table element
        */
    }

    /*
    *  No entry in the table matched - return to caller indicating
    *  that we have failure:
    */

    return iStatus;
}

/* ------------------------------------------------------------------ */

int GetNewSyntaxHead ( struct sSyntaxTableElement* SyntaxTable,
                       struct sRoutineNameTableElement* RoutineNameTable,
                       struct sSyntaxHead** ppNewSyntaxHead ,
                       char*  Identifier )
{
    int iStatus = TRUE;

    *ppNewSyntaxHead = malloc(sizeof(struct sSyntaxHead));
    if(*ppNewSyntaxHead == NULL)
    {
      printf("Out of memory on line %d - exiting.\n", __LINE__);
      exit(EXIT_FAILURE);
    }
#ifdef PARDEBUG
    strcpy((*ppNewSyntaxHead)->SyntaxHeadIdent,"SHD");
#endif

    (*ppNewSyntaxHead)->SyntaxName[0]  ='\0';
    (*ppNewSyntaxHead)->FirstAlternate = NULL;
    (*ppNewSyntaxHead)->iStartsWith    = 0;
    (*ppNewSyntaxHead)->iMustContain   = 0;
    (*ppNewSyntaxHead)->LexRoutine     = NULL;
    (*ppNewSyntaxHead)->iIsLexical     = FALSE;
    (*ppNewSyntaxHead)->pNextHead      = NULL;
    (*ppNewSyntaxHead)->iSyntaxNumber  = iNextSyntax/*++*/;
    if (iNextSyntax>0)
    {
        /*
        *  This new syntax head is not the very first, so there is
        *  a predecessor. We need to chain the back pointer of this
        *  new Syntax Head item to the previous one, and the forward
        *  pointer of the previous one to this:
        */
        (*ppNewSyntaxHead)->pPreviousHead =
                  SyntaxTable[iNextSyntax-1].pSyntaxPointer;
        if ((SyntaxTable[iNextSyntax-1].pSyntaxPointer)!=NULL)
        {
            (SyntaxTable[iNextSyntax-1].pSyntaxPointer)->pNextHead =
                  *ppNewSyntaxHead;
        }
    }
    else
    {
        /*
        *  There is no predecessor:
        */
        (*ppNewSyntaxHead)->pPreviousHead = NULL;
    }
    /*
    *  The array "Identifier" now contains a null-terminated
    *  string with the lexical routine name. Get a pointer
    *  to that real routine, by checking against our internal
    *  table of names:
    */
    if (strcmp(Identifier,"")==0)
    {
        (*ppNewSyntaxHead)->LexRoutine = NULL;
        (*ppNewSyntaxHead)->iIsLexical = FALSE;
    }
    else
    {
        GetRoutinePointer ( RoutineNameTable,
                            Identifier,
                            &((*ppNewSyntaxHead)->LexRoutine) );
        (*ppNewSyntaxHead)->iIsLexical = TRUE;
    }

    return iStatus;
}

/*
*  Process just one syntax line. That is, just one definition
*/
int ProcessSyntaxLine ( struct sSyntaxTableElement* SyntaxTable,
                        struct sAlternateTableElement* AlternateTable,
                        struct sRoutineNameTableElement* RoutineNameTable,
                        char* SyntaxLine,
                        struct sSyntaxHead* pRootSyntax )
{
    int    iStatus = TRUE;
    char*  pC;
    char   LexIdentifier[ROUTINE_NAME_LENGTH+1];
    int    j=0;
    int    iMySyntaxNumber;
    struct sSyntaxHead *pNewSyntaxHead = pRootSyntax;

    /*
    *  Get the next input line, and if there is anything there
    *  then process it. If there is nothing there, then we
    *  have finished.
    */
    pC = fgets(SyntaxLine, SYNTAX_LINE_LIMIT, fSyntaxFile );
    if (pC!=NULL)
    {
        /*
        *  Get the name, and look in the Syntax Names Table:
        */
#ifdef SYNDEBUGPRINT
        printf("ProcessSyntaxLine: We have a syntax line: j=%d\n",j);
        printf("ProcessSyntaxLine: =-=-=-=-=-=-=-=-:%s:=-=-=-=-=-=-=-=-\n",
                 SyntaxLine);
#endif
        iMySyntaxNumber = GetSyntaxName ( SyntaxTable, SyntaxLine, &j );
        pNewSyntaxHead = SyntaxTable[iMySyntaxNumber].pSyntaxPointer;

        /*
        *  Variable "j" now contains the subscript of the space
        *  that terminates the initial name element. Read in the
        *  terminator (which is either ::= or :L:=, and then read all
        *  the alternates for this item until we reach the end
        *  of line and/or the terminating hash sign:
        */
        LexIdentifier[0] = '\0';
        GetLexName ( SyntaxTable, SyntaxLine, LexIdentifier, &j,
                    iMySyntaxNumber );
        iStatus = GetAlternates ( SyntaxTable, AlternateTable,
                                  RoutineNameTable, SyntaxLine, &j, pNewSyntaxHead );

    }   /* end of "if/then" there was something on the input line */
    return iStatus;
}

/* ------------------------------------------------------------------ */

void FreeSyntaxHead ( struct sSyntaxHead* pFreeHead )
{
    struct sSyntaxAlt* pFreeAlt;

    while ( pFreeHead!=NULL )
    {
        /* Point to the first alternate, and free the list of them: */
        pFreeAlt = pFreeHead->FirstAlternate;
        FreeAlternates ( &pFreeAlt );
        /* Free the head item itself: */
        free ( pFreeHead );
        /* And return NULL pointer to caller: */
        pFreeHead = NULL;
    }
    /*
    *  Note that the above "while" loop will execute only once. This
    *  code is generalized for future expansion should the head items
    *  be collected into a list.
    */

    return;
}

/* ------------------------------------------------------------------ */

/*
*  Get the alternates, starting at the character whose subscript
*  is passed in the argument. This parameter is passed on down to
*  the code that gets each individual alternate, and back up to
*  the caller, who then knows how much of the input has been taken
*  by the function. When this variable is set to zero, then all the
*  alternates have been read.
*/
int GetAlternates ( struct sSyntaxTableElement* SyntaxTable,
                    struct sAlternateTableElement* AlternateTable,
                    struct sRoutineNameTableElement* RoutineNameTable,
                    char* SyntaxLine,
                    int* j, struct sSyntaxHead* pNewSyntaxHead )
{
    int iStatus = TRUE;
    struct sSyntaxAlt* pNewAlternate = NULL;
    struct sSyntaxAlt* pPreviousAlternate;

    /*
    *  Until we have either failure or we reach the end of the
    *  definition, get the next alternate:
    */
    while (( iStatus == TRUE ) && ( *j != 0 ))
    {
        /*
        *  Within this loop, remember the address of the previous
        *  alternate, and get a new alternate header:
        */
        pPreviousAlternate = pNewAlternate;
        pNewAlternate = malloc ( sizeof ( struct sSyntaxAlt ) );
        if(pNewAlternate == NULL)
        {
          printf("Out of memory on line %d - exiting.\n", __LINE__);
          exit(EXIT_FAILURE);
        }
#ifdef PARDEBUG
        strcpy((pNewAlternate)->SyntaxAltIdent,"SAL");
#endif
        pNewAlternate->iAlternateNumber = 0;
        pNewAlternate->iMustContain = 0;
        pNewAlternate->iStartsWith = 0;
        pNewAlternate->iSyntaxNumber = 0;
        pNewAlternate->NextAlt = NULL;
        pNewAlternate->ParentHead = NULL;
        pNewAlternate->PreviousAlt = NULL;
        pNewAlternate->ThisBody = NULL;

        /*
        *  If this is not the first alternate, then chain this to
        *  the previous one:
        */
        if (pPreviousAlternate!=NULL)
        {
assert(pPreviousAlternate!=pNewAlternate);
            pPreviousAlternate->NextAlt = pNewAlternate;
            pNewAlternate->PreviousAlt = pPreviousAlternate;
        }
        else
        {
            /*
            *  This is the first ever alternate: set the back pointer
            *  to NULL - there is no previous:
            */
            pNewAlternate->PreviousAlt = NULL;
        }
        /*
        *  Point this new alternate forward to nothing:
        */
        pNewAlternate->NextAlt = NULL;
        /*  the alternate counter for this new alternate: */
        iNextAlternate++;
        /* Set the Alternate Table: */
        AlternateTable[iNextAlternate].pAlternatePointer = pNewAlternate;
        AlternateTable[iNextAlternate].iSyntaxNumber = iNextSyntax;
        AlternateTable[iNextAlternate].iStartsWith = 0;
        AlternateTable[iNextAlternate].iMustContain = 0;
        /* Save this alternate number in the tree item: */
        pNewAlternate->iAlternateNumber = iNextAlternate;
        /* Point back to the parent syntax item: */
        pNewAlternate->iSyntaxNumber = iNextSyntax;
        /*
        *  Now chain this new alternate item onto the end of the
        *  chain. If it is the first item, point the parent to it,
        *  otherwise point the previous item to it:
        */
        if (pPreviousAlternate==NULL)
        {
            pNewSyntaxHead->FirstAlternate = pNewAlternate;
        }
        else
        {
assert(pPreviousAlternate!=pNewAlternate);        
            pPreviousAlternate->NextAlt = pNewAlternate;
        }
        /* Get the body of the alternate, i.e. its individual items: */
        iStatus = GetOneAlternate ( SyntaxTable, RoutineNameTable, SyntaxLine,
                                    j , pNewAlternate );
    }   /* end "while" getting successive Alternates */

    return iStatus;
}

/* ------------------------------------------------------------------ */

/*
*  Get just one alternate. This will terminate at:
*    1)  end of line, or
*    2)  a vertical bar, or
*    3)  the hash sign which terminates a definition
*/
int GetOneAlternate ( struct sSyntaxTableElement* SyntaxTable,
                      struct sRoutineNameTableElement* RoutineNameTable,
                      char* SyntaxLine,
                      int* j, struct sSyntaxAlt* pNewAlternate )
{
    int iStatus = TRUE;
    int i;
    struct sSyntaxBody* pPreviousBody = NULL;
    struct sSyntaxBody* pNewBody = NULL;

    i = 0;
    while (( i == 0 ) && ( iStatus == TRUE ))
    {
        SkipSpaces ( SyntaxLine, j );
        if ((SyntaxLine[*j]=='|')  || (SyntaxLine[*j]=='#')
        ||  (SyntaxLine[*j]=='\n') || (SyntaxLine[*j]=='\0'))
        {
            /* End of alternate: */
            /* Set the marker to terminate the enclosing "while": */
            i = 1;
            if ((SyntaxLine[*j]=='#') || (SyntaxLine[*j]=='\0'))
            {
                iStatus = FALSE;
            }
            else
            {
                /*
                *  Skip past the Alternate terminator character:
                */
                (*j)++;
                iStatus = TRUE;
            }
        }   /* end of "if/then" termination of alternate */
        else
        {
            /*
            *  Get a new body item, and chain it on to the tree
            *  structure:
            */
            pPreviousBody = pNewBody;
            pNewBody = malloc ( sizeof ( struct sSyntaxBody ) );
            if(pNewBody == NULL)
            {
              printf("Out of memory on line %d - exiting.\n", __LINE__);
              exit(EXIT_FAILURE);
            }

#ifdef PARDEBUG
            strcpy((pNewBody)->SyntaxBodyIdent,"SBD");
#endif

            /*
            *  If this is the first body item within an alternate, then
            *  the parent alternate structure must point to it, the back
            *  pointer is NULL (because there is no previous):
            */
            if (pPreviousBody==NULL)
            {
                pNewAlternate->ThisBody = pNewBody;
                pNewBody->PreviousBody  = NULL;
            }
            else
            {
                /*
                *  This is not the first body item within an alternate,
                *  so chain this item back to the previous one - and the
                *  previous one forward to this:
                */
                pNewBody->PreviousBody = pPreviousBody;
                pPreviousBody->NextBody = pNewBody;
            }
            pNewBody->NextBody      = NULL;
            pNewBody->BodyContents  = NULL;
            pNewBody->BodyCheck     = NULL;
            pNewBody->BodyHead      = NULL;
            pNewBody->LexCheck      = NULL;
            pNewBody->CodeGenerate  = NULL;
            pNewBody->iSyntaxNumber = iNextSyntax;
            pNewBody->iAlternateNumber = iNextAlternate;
            pNewBody->ParentAlt     = pNewAlternate;
            pNewBody->iStartsWith   = 0;
            pNewBody->iMustContain  = 0;
            iStatus = GetSyntaxItem ( SyntaxTable, RoutineNameTable, SyntaxLine,
                                      j, pNewBody );
            if (!iStatus)
            {
                i = 1;
                iStatus = TRUE;
            }
        }
    }   /* end of "while" getting items within this alternate */
    return iStatus;
}

/* ------------------------------------------------------------------ */

void FreeAlternates ( struct sSyntaxAlt** pFreeAlt )
{
    struct sSyntaxAlt* pNextAlt = NULL;
    struct sSyntaxBody* pFreeBody;
    /*
    *  Provided that we still have a pointer to an Alternate to
    *  free, get the address of the next one, free its contents,
    *  and free the alternate header itself:
    */
    while (*pFreeAlt!=NULL)
    {
        /* Get pointer to next: */
        pNextAlt  = (*pFreeAlt)->NextAlt;
        /* Get pointer to first body item, and free the body list: */
        pFreeBody = (*pFreeAlt)->ThisBody;
        FreeSyntaxItem ( &pFreeBody );
        /* Release the Alternate header: */
        free ( *pFreeAlt );
        /* And point to the next one (if any): */
        *pFreeAlt = pNextAlt;
    }

    /* Return the NULL signal to caller: */
    *pFreeAlt = pNextAlt;

    return;
}

/* ------------------------------------------------------------------ */

int GetSyntaxItem ( struct sSyntaxTableElement* SyntaxTable,
                    struct sRoutineNameTableElement* RoutineNameTable,
                    char* SyntaxLine,
                    int* j, struct sSyntaxBody* pNewBody )
{
    int iStatus = TRUE;
    int i;
    int k;
    int l;
    int m;
    int n;
    char Identifier[ROUTINE_NAME_LENGTH+1];
    int (*RoutineAddress) (void *);
    /*
    *  The Syntax Item that starts at subscript "j" starts with one
    *  of the following characters:
    *     a letter                  this is a reference to another
    *                               syntax name
    *     a left round parenthesis  this is a reference to a function
    *                               which will be called at recognition
    *                               time
    *     a left square bracket     this is a reference to a function
    *                               which will be called at code genera-
    *                               tion time
    *     a left curly bracket      this is a reference to a function
    *                               called during lexical analysis
    *     a double-quote            this is an explicit string which must
    *                               be present at this point in the text
    *                               which is being parsed
    *     anything else             an error in the syntax definition
    */
    switch (SyntaxLine[*j]) {
    case '(':
        /*
        *  This is the name of a routine to be called at parse time.
        *  We must change the value of BodyCheck. Get in the name
        *  of the routine to be called, and look its name up in the
        *  table of names and pointers, so that we can get a pointer
        *  to that routine:
        */
        (*j)++;
        iStatus = GetIdentifier ( SyntaxLine , Identifier , j );
        /* Skip the terminating right parenthesis: */
        (*j)++;
        iStatus = GetRoutinePointer ( RoutineNameTable, Identifier, &RoutineAddress );
        pNewBody->BodyCheck = RoutineAddress;
        break;
    case '[':
        /*
        *  This is the name of a routine to be called at code genera-
        *  tion time. This code is the same as that for BodyCheck above.
        */
        (*j)++;
        iStatus = GetIdentifier ( SyntaxLine , Identifier , j );
        /* Skip the terminating right square bracket: */
        (*j)++;
        iStatus = GetRoutinePointer ( RoutineNameTable, Identifier, &RoutineAddress );
        pNewBody->CodeGenerate = RoutineAddress;
        break;
    case '{':
        /*
        *  This is the name of a routine to be called during the
        *  lexical analysis phase.
        */
        (*j)++;
        iStatus = GetIdentifier ( SyntaxLine , Identifier , j );
        /* Skip the terminating right curly: */
        (*j)++;
        iStatus = GetRoutinePointer ( RoutineNameTable, Identifier, &RoutineAddress );
        pNewBody->LexCheck = RoutineAddress;
        break;
    case '\"':
        /*
        *  This is a character string for matching. We must change the
        *  value of BodyContents. Scan forward until we find the end of
        *  the string, which is an un-escaped double-quote. Also note
        *  the address of the start of the string.
        */
        k = *j + 1;  /* Substript of first character in string */
        m = 0;
        n = 0;
        for (i=k;(m==0);i++)
        {
            if (((SyntaxLine[i]=='\"') && (SyntaxLine[i+1]!='\\'))
            ||  (SyntaxLine[i]=='\n') || (SyntaxLine[i]=='\0'))
            {
                m = i;  /* Subscript of terminating quote */
                (*j) += n + 2;
            }
            n++;
        }
        /* Get enough memory for the length of this string: */
        l = m - k;
        pNewBody->BodyContents = malloc(l + 1);
        if(pNewBody->BodyContents == NULL)
        {
          printf("Out of memory on line %d - exiting.\n", __LINE__);
          exit(EXIT_FAILURE);
        }

        strcpy(pNewBody->BodyContents,"");
        strncat(pNewBody->BodyContents,SyntaxLine+k,(l));
        pNewBody->iSyntaxNumber = 0;
        pNewBody->BodyHead = NULL;
        break;
    default:
        if (isalpha(SyntaxLine[*j]))
        {
            /* This is a reference to a syntax item. Get the name and
            look it up: */
            k = GetSyntaxName ( SyntaxTable, SyntaxLine, j );
            pNewBody->iSyntaxNumber = k;
            pNewBody->BodyHead = SyntaxTable[k].pSyntaxPointer;
        }
        else
        {
            /*
            *  This is an error - the syntax definition is incorrect
            *  An error message could be issued here. At the very least,
            *  we signal this failure to the caller:
            */
            iStatus = FALSE;
        }
        break;
    };

    return iStatus;
}

/* ------------------------------------------------------------------ */

void FreeSyntaxItem ( struct sSyntaxBody** pFreeBody )
{
    struct sSyntaxBody* NextSyntaxBody = NULL;

    /*
    *  If the incoming pointer is not null, then release anything
    *  pointed at by this element, get the address of the next
    *  element, and free this element. Loop ruond till thw whole
    *  list is freed:
    */
    while (*pFreeBody!=NULL)
    {
        /* Get address of next element: */
        NextSyntaxBody = (*pFreeBody)->NextBody;
        /* If there is a character string to free, free it: */
        if (((*pFreeBody)->BodyContents)!=NULL)
        {
            free((*pFreeBody)->BodyContents);
        }
        /* free the list item itself: */
        free ( *pFreeBody );
        /* And point to the next one (if any): */
        *pFreeBody = NextSyntaxBody;
    }

    /* Return NULL pointer to caller: */
    *pFreeBody = NextSyntaxBody;

    return;
}

/* ------------------------------------------------------------------ */

void SkipSpaces ( char* SyntaxLine,
                  int* j )
{
    /*
    *  This moves parameter "j" forward past any whitespace, NOT
    *  including newline. So spaces and tabs are bypassed. On return
    *  "j" points to the first non-space character, which might be
    *  the end-of-string marker, '\0', or the first character of
    *  the next body item:
    */
    while (isspace(SyntaxLine[*j]) && (SyntaxLine[*j]!='\n'))
    {
        (*j) ++;
    }

    return;
}

/* ========================== Parse input buffer ======================= */

void RemoveSpaces ( char* InputBuffer )
{
   char *t;
   for (t=InputBuffer; *InputBuffer; !isspace(*InputBuffer) ?
                 *t++ = *InputBuffer++ : *InputBuffer++)
   {
      continue;
   }
   *t = '\0';
}


/* ------------------------------------------------------------------ */

/*
*  These are the parse routines for speeding up the parse process. They
*  are code implementations of some of the parse nodes, which will be
*  very much faster than the general technique. They are not necessary:
*  they merely make things faster.
*/

int parletter ( void* one )
{
    int iStatus;

    /*
    *  Check whether the character being pointed at by the first
    *  argument is a letter. If it is, then return TRUE,
    *  otherwise indicate failure.
    */
    if (isalpha(*(char*)one))
    {
        iStatus=TRUE;
    }
    else
        iStatus = FALSE;

    return iStatus;
}

int pardigit ( void* one )
{
    int iStatus;

    if (isdigit(*(char*)one))
    {
        iStatus=TRUE;
    }
    else
        iStatus = FALSE;

    return iStatus;
}

int paroctal ( void* one )
{
    int iStatus;

    if ((isdigit(*(char*)one)) && (*(char*)one!='8')
    &&  (*(char*)one!='9'))
    {
        iStatus=TRUE;
    }
    else
        iStatus = FALSE;

    return iStatus;
}

int parchar ( void* one )
{
    int iStatus;

    /*
    *  Allow everything except single quote, double quote, question-
    *  mark and back-slash:
    */

    if ((*(char*)one=='\'') || (*(char*)one=='\\')
    ||  (*(char*)one=='\?') || (*(char*)one=='\"'))
    {
        iStatus = FALSE;
    }
    else
        iStatus=TRUE;

    return iStatus;
}

/* ------------------------------------------------------------------ */

int BuildNewParseBody ( struct sParseNode*  pParentHead,
                        struct sParseNode*  pPreviousBody,
                        struct sParseNode** ppNewBody,
                        int    iFirstChar,
                        int    iLastChar )
{
    int iStatus;

    /*
    *  Get a new ParseBody item, chaining it to the previous item
    *  (if any). Then place in the ParseBody item the first and
    *  last addresses of the character string processed by it:
    */
printf("BuildNewParseBody TOP\n");
    iStatus = GetNewParseBody ( pParentHead, pPreviousBody, ppNewBody );
printf("BuildNewParseBody iStatus=%d\n",iStatus);

    if (iStatus==TRUE)
    {
        (*ppNewBody)->iFirstChar = iFirstChar;
        (*ppNewBody)->iLastChar  = iLastChar;
    }

    return iStatus;
}

/* ------------------------------------------------------------------ */

int GetNewParseBody ( struct sParseNode*  pParentHead,
                      struct sParseNode*  pPreviousBody,
                      struct sParseNode** ppNewBody )
{
    int iStatus = TRUE;
assert(pParentHead!=NULL);
    /*
    *  Get a new Parse Body item, and point the caller's
    *  given pointer to it:
    */
printf("GetNewParseBody TOP\n");
    *ppNewBody = malloc(sizeof (struct sParseNode));

    /*
    *  Chain this forward and back, and set the parent pointer:
    */
    if ((*ppNewBody)!=NULL)
    {
#ifdef PARDEBUG
        strcpy((*ppNewBody)->ParseNodeIdent,"PBD");
#endif
        pParentHead->ThisParse =     *ppNewBody;
        (*ppNewBody)->ParentParse   = pParentHead;
        (*ppNewBody)->NextParse     = NULL;
        (*ppNewBody)->PreviousParse = pPreviousBody;
        if (pPreviousBody!=NULL)
        {
            pPreviousBody->NextParse = *ppNewBody;
        }
        (*ppNewBody)->ThisParse  = NULL;
        (*ppNewBody)->ThisHead   = NULL;
        (*ppNewBody)->ThisBody   = NULL;
        (*ppNewBody)->CodeGenerate = NULL;
        (*ppNewBody)->iFirstChar = 0;
        (*ppNewBody)->iLastChar  = -1;
    }
    else
    {
        /*
        *  We were unable to allocate enough memory for another
        *  Parse Body item - this is bad! At his point we could
        *  issue an error message, collapse in a heap, or perform
        *  some undefined action that causes demons to fly from the
        *  user's nose - an error message is probably best.
        */
        printf("Out of memory on line %d - exiting.\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    return iStatus;
}

/* ------------------------------------------------------------------ */

int TryMatchParse ( char** ppInputBuffer,
                    int* k,
                    struct sSyntaxBody* pSyntaxP,
                    struct sParseNode** ppParseBody )
{
    int iStatus = TRUE;
    int iBodyStringLength;
    int kLocal;
    struct sParseNode* pNewParse = NULL;
    char *pInputBuffer = NULL;

assert(ppInputBuffer!=NULL);
assert(*ppInputBuffer!=NULL);
    pInputBuffer = *ppInputBuffer;
assert(ppParseBody!=NULL);
assert(*ppParseBody!=NULL);
assert(k!=NULL);
    (*ppParseBody)->iFirstChar = *k;

    /*
     *  The question this routine tries to answer is "does the input
     *  buffer, starting at position 'k', match the abstract syntax
     *  to which pSyntaxP refers? If so, return TRUE and complete the
     *  parse node referred to from ppParseBody. If not, then return
     *  FALSE, without having completed the ppParseBody node.
     *  In order to answer this question, this routine has to:
     *  EITHER check the buffer character-string against an abstract
     *  syntax string
     *  OR set up the environment for a deeper Parse
     *  OR (in the case of the existence and support of the short-
     *  circuit routines) call some specific interior routines which
     *  performs a rapid check.
     *  In this implementation we have just the first two possibilities,
     *  as this version of the code does not act upon short-circuit
     *  routines.
     */

    /*
     *  At what kind of syntax node are we pointing?
     *  We answer this question by looking at the syntax node and seeing
     *  whether it points to a character string (BodyContents is non-NULL)
     *  or to another Syntax node (sSyntaxHead is non-NULL)
     */
printf("TryMatchParse pSyntaxp=:%p:\n", (void *)pSyntaxP);
if(pSyntaxP!=NULL)printf("pSyntaxP->BodyContents=:%p:\n",
(void *)pSyntaxP->BodyContents);
    if (pSyntaxP->BodyCheck!=NULL)
    {
printf("About to call BodyCheck routine %p\n",(void*)pSyntaxP->BodyCheck);
       iStatus = (*pSyntaxP->BodyCheck)( pInputBuffer+*k );
    }
    else if (pSyntaxP->BodyContents!=NULL)
    {
printf("TryMatchParse string comparison\n");
        /*
         *  This is a character-string comparison node:
         *  Make the string compare, from the abstract string (pointed
         *  at by the syntax tree) and the actual string (starting at
         *  position 'k' in the input buffer). The length of the
         *  comparison has to be the length of the abstract string, as
         *  all of this must match exactly. We have to check that this
         *  length does not overshoot the end of the input buffer:
         */
        iBodyStringLength = strlen(pSyntaxP->BodyContents);
        if (((iBodyStringLength + *k)>PARSE_BUFFER_LENGTH)
           ||((iBodyStringLength + *k)>strlen(pInputBuffer+*k))
           ||(iBodyStringLength<1))
        {
            /*
            *  This overshoots or undershoots - so it cannot compare exactly -
            *  indicate failure:
            */
            iStatus = FALSE;
            return iStatus;
        }
        /*
        *  At this point we know that the input buffer is at least
        *  as long, from the character being pointed at, as the abstract
        *  string. Hence we can safely compare the full length of the
        *  strings:
        */
assert(pInputBuffer+*k!=NULL);
assert(pSyntaxP->BodyContents!=NULL);
printf("contents k=%d\n",*k);
printf("comparing [%s] against [%s] for %d\n",
pInputBuffer+*k,pSyntaxP->BodyContents,iBodyStringLength);
if (iBodyStringLength==1) printf("%02X|%02X\n",*(pInputBuffer+*k),
*pSyntaxP->BodyContents);
        if (strncmp(pInputBuffer+*k,pSyntaxP->BodyContents,
                    iBodyStringLength)!=0)
        {
printf("Compare not equal\n");
WAIT
            /*
            *  The strings are not equal - indicate failure:
            */
            iStatus = FALSE;
            return iStatus;
        }
printf("Compare was equal\n");
WAIT
        *ppInputBuffer += iBodyStringLength;
        (*ppParseBody)->iLastChar = *k + iBodyStringLength;
        /*
        *  If control reaches this part of the "if/then" we have
        *  a successful string comparison
        */
    } else {
        /*
        *  This is a node which has to be parsed more deeply:
        *  We have to go down a level, so we need to call Parse recursively.
        *  To do this we have to allocate a new parse node, and chain it
        *  to this current node as the first daughter. If the recursive
        *  call to Parse returns success, then we can carry on. If it
        *  returns failure, then we too have to return failure - and
        *  the Parse level above will find the next potential syntax
        *  element for us to test against.
        */

        /*
        *  So, firstly, get and chain in a new daughter node:
        */
printf("TrymatchParse deeper comparison\n");
        pNewParse = NULL;
        kLocal = *k;
        if (!BuildNewParseBody ( *ppParseBody, NULL, &pNewParse, kLocal, -1))
        {
            /*
            *  Oh dear. We were unable to build a new parse node to place
            *  on the tree. Now is a good time to burst into tears. Failing
            *  that, we can just issue an error message and terminate the
            *  whole program. For the time being, however, we shall just
            *  weakly indicate failure:
            */
            iStatus = FALSE;
            return iStatus;
        }
        pNewParse->CodeGenerate = pSyntaxP->CodeGenerate;
printf("Setting ThisHead [%p] line %d\n",pSyntaxP->BodyHead,__LINE__);
        pNewParse->ThisHead = pSyntaxP->BodyHead;
        pNewParse->ThisBody = pSyntaxP->ParentAlt->ThisBody;
printf("TryMatchParse about to Parse\n");
        if((pSyntaxP->BodyHead)!=NULL)
/*           iStatus = Parse ( &pInputBuffer, &kLocal, pSyntaxP->BodyHead, */
           iStatus = Parse ( ppInputBuffer, &kLocal, pSyntaxP->BodyHead,
                          &pNewParse );
        else
           iStatus = TRUE;
        *k = kLocal;
printf("TryMatchParse Parse=%d\n",iStatus);
    }

    return iStatus;
}


/* ------------------------------------------------------------------ */

/*
 *  The generalised Parse routines take as initial input three
 *  parameters:
 *    the address of the input buffer,
 *    the address of the abstract sytax tree ("what can be recognised")
 *    the address of the output parse tree ("what was recognised").
 *  Initially the syntax tree is givem but the parse tree is NULL.
 *  The input buffer is assumed to have already been stripped of extra
 *  spaces, so that every <whitespace> has been changed to be exactly
 *  one space character.
*/

int Parse ( char** ppInputBuffer,
            int*   k,
            struct sSyntaxHead* pRootS,
            struct sParseNode** ppRootP )
{
    int    iStatus                   = FALSE;
    int    iWorkStatus               = TRUE;
    int    bNextExists               = FALSE;
    int    bSkipCalled               = FALSE;
    struct sSyntaxBody*  pSyntaxP    = NULL;
    struct sSyntaxBody** ppSyntaxP   = &pSyntaxP;
    struct sParseNode*   pParseBody  = NULL;
    struct sParseNode**  ppParseBody = &pParseBody;
    struct sParseNode*   pRootP      = NULL;

    /*
    *  If the parse pointer is NULL, then we have not performed any
    *  parse yet at all. In this case we have to allocate the first,
    *  root element. In all other cases the parse pointer is pointing
    *  to the lowest element we are trying to complete.
    */
assert(pRootS!=NULL);
assert(ppRootP!=NULL);
printf("TOP of Parse, ppRootP=:%p:\n", (void *)ppRootP);
if(ppRootP!=NULL)printf("->:%p:\n", (void *)*ppRootP);
    if ((*ppRootP)==NULL)
    {
printf("Root Pointer is NULL in Parse\n");
        *ppRootP = malloc(sizeof (struct sParseNode));

        if ((*ppRootP)==NULL)
        {
            /*
            *  We have not been able to allocate the root element.
            *  At this point we could issue an error message. In any
            *  case we need to signal an error and return:
            */
            printf("Out of memory on line %d of file %s - exiting.\n",
                   __LINE__,__FILE__);
            iStatus = FALSE;
            return iStatus;
        }

#ifdef PARDEBUG
        strcpy((*ppRootP)->ParseNodeIdent,"PBD");
#endif

        /*
        *  We have the new root element. Set its initial values:
        */
        (*ppRootP)->ParentParse   = NULL;
        (*ppRootP)->NextParse     = NULL;
        (*ppRootP)->PreviousParse = NULL;
        (*ppRootP)->ThisHead      = pRootS;
        (*ppRootP)->ThisBody      = NULL;
        (*ppRootP)->ThisParse     = NULL;
        (*ppRootP)->CodeGenerate  = NULL;
        (*ppRootP)->iFirstChar    = 0;
        (*ppRootP)->iLastChar     = -1;
    }
    else
    {
printf("ppRootP was not NULL in Parse\n");
        (*ppRootP)->ThisHead      = pRootS;
    }
    pParseBody = *ppRootP;
printf("pRootS=:%p:\n",(void *)pRootS);
assert(pRootS->FirstAlternate!=NULL);
printf("FirstAlternate=:%p:\n",(void *)pRootS->FirstAlternate);
printf("ThisBody=:%p:\n",(void *)pRootS->FirstAlternate->ThisBody);

    pSyntaxP = pRootS->FirstAlternate->ThisBody;
    ppSyntaxP = &pSyntaxP;
    bNextExists = (pSyntaxP!=NULL);

    /*
    *  If this a non-NULL pointer, then we have to inspect the
    *  buffer, and match it against the syntax. If it is a NULL
    *  pointer, however, then we have finished this parse node:
    */
printf("bNextExists=%d iWorkStatus=%d\n",bNextExists,iWorkStatus);
        while (bNextExists && iWorkStatus)
        {
printf("pSyntaxP=:%p: ppSyntaxP=:%p:\n",(void *)pSyntaxP,(void *)ppSyntaxP);
    if (pSyntaxP==NULL)
    {
        iStatus = TRUE;
        return iStatus;
    }   /* this was a finished parse */
        bNextExists = TRUE;
            /*
            *  Now try to match the input buffer against this possible
            *  syntax item:
            */
            iWorkStatus = TryMatchParse ( ppInputBuffer, k, pSyntaxP,
                                          ppParseBody );
printf("After TryMatchParse iWorkStatus=%d\n",iWorkStatus);
WAIT
            /*
            *  If this matched, then we go on to the next item in the chain
            *  of items in this syntax branch. If it does not match, then we
            *  have to go back up a level to the next choice:
            */
            bSkipCalled = FALSE;
            if (iWorkStatus)
            {
                *ppInputBuffer += *k;
                bNextExists = GetNextSyntax ( ppSyntaxP );
    if (pSyntaxP==NULL)
    {
        iStatus = TRUE;
        return iStatus;
    }   /* this was a finished parse */
            }
            else
            {
printf("Before SkipNexSyntax ppSyntaxP=%p\n",(void *)*ppSyntaxP);
                iWorkStatus = SkipNextSyntax ( ppSyntaxP );
printf("After  SkipNexSyntax ppSyntaxP=%p\n",(void *)*ppSyntaxP);
                bSkipCalled = TRUE;
/*WAIT*/
                *k = pParseBody->iFirstChar;
    if (pSyntaxP==NULL)
    {
        iStatus = FALSE;
        return iStatus;
    }   /* this was a finished parse */
            }
printf("After Get/Skip bNextExists=%d iWorkStatus=%d\n",bNextExists,iWorkStatus);
printf("pSyntaxP=:%p: ppSyntaxP=:%p:\n",(void *)pSyntaxP,(void *)ppSyntaxP);
    if (((*ppSyntaxP)!=NULL) && bNextExists && iWorkStatus)
    {
        pSyntaxP = *ppSyntaxP;
printf("After resetting pSyntaxP=%p\n",(void *)pSyntaxP);
    if (pSyntaxP==NULL)
    {
        iStatus = TRUE;
        return iStatus;
    }   /* this was a finished parse */
#if 0
        (*ppRootP)->ThisBody = NextParseItem ( &pRootP ); /* ??? */
        pSyntaxP = NextParseItem ( &pRootP );
        *ppRootP = pRootP;
#endif
    }
    else
    {
       if(*ppRootP==NULL)
       {
          iStatus = FALSE;
          *ppRootP = NULL;
          return iStatus;
       }
       else if ((bSkipCalled) && (!iWorkStatus))
       {
printf("Skip was called, parse node=%.*s\n",
SYNTAX_NAME_LENGTH,(*ppRootP)->ThisHead->SyntaxName);
WAIT
          pRootP = (*ppRootP)->ParentParse;
          free(*ppRootP);
if (pRootP==NULL) {printf("About to set ppRootP to NULL\n");WAIT}
          *ppRootP = pRootP;
       }
    }

        }   /* end of "while" there is another node to check */
        /*
        *  if "iWorkStatus" is TRUE then we have finished a parse
        *  node successfully. Otherwise we have to tell the caller
        *  to backtrack.
        */
        iStatus = iWorkStatus;
printf("Parse is returning %d\n",iStatus);
    return iStatus;
}

/* ---------------------------------------------------------------- */
int GetNextSyntax ( struct sSyntaxBody** ppSyntaxP )
{
    int iReturn = FALSE;

    /*
    *  We have just parsed a node successfully, and ppSyntaxP is
    *  pointing at that syntax node. We need to find the next item in
    *  the abstract syntax tree that needs to be inspected next.
    *  That item has to be on this syntax branch, following the
    *  one we have just finished. If it exists, then we construct
    *  a new parse node, chain it on to the previous one, point
    *  to it, and return indicating success.
    *  If it does not exist, howver, then we leave all the input
    *  pointers in their current status, and return indicating
    *  failure.
    */
assert(ppSyntaxP!=NULL);
assert(*ppSyntaxP!=NULL);
    *ppSyntaxP = (*ppSyntaxP)->NextBody;
    if ((*ppSyntaxP)!=NULL)
        iReturn = TRUE;
    return iReturn;
}

/* ---------------------------------------------------------------- */

int SkipNextSyntax (struct sSyntaxBody** ppSyntaxP )
{
    int iReturn = FALSE;

    /*
    *  We have just parsed a node unsuccessfully, and ppSyntaxP
    *  is pointing at that node. We need to find the next item in
    *  the abstract syntax tree which has to be considered. This
    *  item must be the first item on a subsequent branch of the
    *  same main syntax head as we have just tried. If this
    *  exists, then we point this parse node to it, and return
    *  indicating success. If, however, it does not exist then
    *  we leave all the pointers as they are, and return indicating
    *  failure. It will then be up to the caller to get rid of
    *  this parse node, and try again higher up.
    */
assert(ppSyntaxP!=NULL);
assert(*ppSyntaxP!=NULL);
assert((*ppSyntaxP)->ParentAlt!=NULL);
    if (((*ppSyntaxP)->ParentAlt->NextAlt)!=NULL)
    {
        *ppSyntaxP = (*ppSyntaxP)->ParentAlt->NextAlt->ThisBody;
    }
    else
    {
        *ppSyntaxP = NULL;
    }

    if ((*ppSyntaxP)!=NULL)
        iReturn = TRUE;
    return iReturn;
}

/* ---------------------------------------------------------------- */

int GenerateOutputCode ( struct sParseNode* pRootP )
{
    int  iStatus = TRUE;
    struct sParseNode* pParseHead;
    struct sParseNode* pParseBody;

    /*
    *  The parse has been successful. We must now scan the whole
    *  parse tree, processing the nodes in Reverse Polish order,
    *  generatng the output code, by calling the routines (if any)
    *  that are associated with the particular syntax items:
    */

    pParseHead = pRootP;
    if (pParseHead!=NULL)
    {
    pParseBody = pParseHead->ThisParse;
    while ((pParseHead!=NULL) && (iStatus==TRUE))
    {
        /*
        *  process this node:
        */
        iStatus = ProcessOutputNode( &pParseHead, &pParseBody );
    }
    }

    return iStatus;
}

int ProcessOutputNode ( struct sParseNode** ppParseHead,
                        struct sParseNode** ppParseBody )
{
    int iStatus = TRUE;

    /*
    *  Get the address of the next node:
    */
assert(ppParseBody!=NULL);
assert(ppParseHead!=NULL);
fprintf(fOutputFile,"ABOUT TO CONSIDER GENERATION, ppParseBody=%p->%p\n",
(void *)ppParseBody,(void *)*ppParseBody);
    if ((*ppParseBody)==NULL)
    {
        /*
        *  This is the last element in a branch. Anything to do?
        */
assert(*ppParseHead!=NULL);
        if (/*((*ppParseHead)->ThisBody!=NULL) && */
            (((*ppParseHead)->CodeGenerate)!=NULL))
        {
fprintf(fOutputFile,"ABOUT TO GENERATE\n");
             (*ppParseHead)->CodeGenerate ( NULL );
        }
    }
    else
    {
fprintf(fOutputFile,"NO OUTPUT TO GENERATE\n");
        *ppParseBody = (*ppParseBody)->NextParse;
    }
    if((*ppParseBody)!=NULL)
    {
        *ppParseHead = (*ppParseBody)->ParentParse;
    }
    else
    {
        *ppParseHead = NULL;
    }
    return iStatus;
}

/* ---------------------------------------------------------------- */

int gencomparison ( char* pszComparator )
{
    int iStatus = TRUE;
    static int globLabel = 0;

    fprintf(fOutputFile,"A1 = stack[index--];\n");
    fprintf(fOutputFile,"A2 = stack[index--];\n");
    fprintf(fOutputFile,"if (A1 %s A2) goto M%4.4d;\n",
                        pszComparator,++globLabel);
    fprintf(fOutputFile,"A1 = 0;\n");
    fprintf(fOutputFile,"goto L%4.4d;\n",globLabel);
    fprintf(fOutputFile,"M%4.4d:\n",globLabel);
    fprintf(fOutputFile,"A1 = 1;\n");
    fprintf(fOutputFile,"L%4.4d:\n",globLabel);
    return iStatus;
}

/* ---------------------------------------------------------------- */

int genlt ( void* one )
{
    int iStatus;
    iStatus = gencomparison("<");
    return iStatus;
}

int gengt ( void* one )
{
    int iStatus;
    iStatus = gencomparison(">");
    return iStatus;
}

int genle ( void* one )
{
    int iStatus;
    iStatus = gencomparison("<=");
    return iStatus;
}

int genge ( void* one )
{
    int iStatus;
    iStatus = gencomparison(">=");
    return iStatus;
}

int geneq ( void* one )
{
    int iStatus;
    iStatus = gencomparison("==");
    return iStatus;
}

int genne ( void* one )
{
    int iStatus;
    iStatus = gencomparison("!=");
    return iStatus;
}

/* ---------------------------------------------------------------- */

int genoperate ( char* pszOperator )
{
    int iStatus = TRUE;

    fprintf(fOutputFile,"A1 = stack[index--];\n");
    fprintf(fOutputFile,"A2 = stack[index--];\n");
    fprintf(fOutputFile,"A1 = A1 %s A2;\n",pszOperator);
    fprintf(fOutputFile,"stack[index++] = A1;\n");

    return iStatus;
}

/* ---------------------------------------------------------------- */

int genadd ( void* one )
{
    int iStatus;

    /*
    *  Plant the code to add together the top two items on
    *  the stack:
    *       A1 = stack[index--];
    *       A2 = stack[index--];
    *       A1 = A1 + A2;
    *       stack[index++] = A1;
    *
    *  This uses a general routine, which takes just the "plus"
    *  sign as argument, and is also used by gensubtract, gendivide,
    *  genmultiply and genmodulus.
    *  What is not so obvious at first glance is that the same
    *  pattern of code is planted for genAND, genOR, genXOR,
    *  genLAND and genLOR.
    */

    iStatus = genoperate("+");

    return iStatus;
}

int gensubtract ( void* one )
{
    int iStatus;
    iStatus = genoperate("-");
    return iStatus;
}

int genmultiply ( void* one )
{
    int iStatus;
    iStatus = genoperate("*");
    return iStatus;
}

int gendivide ( void* one )
{
    int iStatus;
    iStatus = genoperate("/");
    return iStatus;
}

int genmodulus ( void* one )
{
    int iStatus;
    iStatus = genoperate("%");
    return iStatus;
}

int genAND ( void* one )
{
    int iStatus;
    iStatus = genoperate("&&");
    return iStatus;
}

int genOR ( void* one )
{
    int iStatus;
    iStatus = genoperate("||");
    return iStatus;
}

int genXOR ( void* one )
{
    int iStatus;
    iStatus = genoperate("^");
    return iStatus;
}

int genLAND ( void* one )
{
    int iStatus;
    iStatus = genoperate("&");
    return iStatus;
}

int genLOR ( void* one )
{
    int iStatus;
    iStatus = genoperate("|");
    return iStatus;
}

/* ---------------------------------------------------------------- */
void sp(int depth)
{
    int i;
    for (i=0;i<depth;i++)
        printf(" ");
    return;
}

void PrintParseTree ( struct sParseNode* pInput, int depth )
{
    struct sParseNode* pParseNode;
    int    newDepth;

    newDepth = depth+1;
    pParseNode = pInput;
    while (pParseNode!=NULL)
    {
       sp(depth); printf("At :%p: points :%p:%p:",(void *)pParseNode,
       (void *)pParseNode->ThisHead,(void *)pParseNode->ThisBody);
assert(pParseNode->ThisHead!=NULL);       
       printf("%p\n",pParseNode->ThisHead->SyntaxName);
       sp(depth); printf("   :%p:%p:%p:  :%p:%p:%p:\n",
       (void *)pParseNode->ParentParse,(void *)pParseNode->NextParse,
       (void *)pParseNode->PreviousParse,
       (void *)pParseNode->ThisParse,(void *)pParseNode->ThisHead,
       (void *)pParseNode->ThisBody);

       /*
       *  Is there anything at the level below?
       */
       if (pParseNode->ThisParse!=NULL)
       {
           PrintParseTree ( pParseNode->ThisParse, newDepth);
       }

       /*
       *  Go to next at this level
       */
       pParseNode = pParseNode->NextParse;
    }

    return;
}

void PrintSyntaxTree ( struct sSyntaxTableElement* SyntaxTable,
                       struct sAlternateTableElement* AlternateTable,
                       struct sSyntaxHead* pRootSyntax )
{
    struct sSyntaxHead* pSyntaxHead;
    struct sSyntaxAlt*  pSyntaxAlt;
    struct sSyntaxBody* pSyntaxBody;
    int iHead = 0;
    int iAlt  = 0;
    int iBody = 0;
    int i = 0;
    /* Start from the root syntax head  */
    pSyntaxHead = pRootSyntax;
    printf("------------------START----------------------\n");
    /* Loop round the syntax heads      */
    while ((pSyntaxHead!=NULL) && (iHead<20))
    {
        iHead++;
        printf("Head name :%s: address  :%p:",pSyntaxHead->SyntaxName,
               (void *)pSyntaxHead);
#ifdef PARDEBUG
        printf("   %s",(pSyntaxHead)->SyntaxHeadIdent);
#endif
        printf("\n");
        printf("Syntax counter in head  :%d:\n",pSyntaxHead->iSyntaxNumber);
        printf("First Alternate in Head :%p:\n",(void *)pSyntaxHead->FirstAlternate);
        pSyntaxAlt = pSyntaxHead->FirstAlternate;
        iAlt = 0;
        while ((pSyntaxAlt!=NULL) && (iAlt<10))
        {
            iAlt++;
            printf("    Start of alternate :%p:",(void *)pSyntaxAlt);
#ifdef PARDEBUG
            printf("   %s",(pSyntaxAlt)->SyntaxAltIdent);
#endif
            printf("\n");
            printf("    Alternate counter  :%d:\n",
                   pSyntaxAlt->iAlternateNumber);
            printf("    Syntax Counter in Alternate :%d:\n",
                   pSyntaxAlt->iSyntaxNumber);
            printf("    Body pointer in Alternate   :%p:\n",
                   (void *)pSyntaxAlt->ThisBody);
            /* For this syntax head, loop round the alternates */
            /* For this alternate, loop round the body items   */
            pSyntaxBody = pSyntaxAlt->ThisBody;
            iBody = 0;
            while ((pSyntaxBody!=NULL) && (iBody<20))
            {
                printf("        Start of body  :%p:",(void *)pSyntaxBody);
#ifdef PARDEBUG
                printf("   %s",(pSyntaxBody)->SyntaxBodyIdent);
#endif
                printf("\n");
                printf("        parent alt     :%p:\n",(void *)pSyntaxBody->ParentAlt);
                printf("        Body Contents  :%p:\n",(void *)pSyntaxBody->BodyContents);
                if (pSyntaxBody->BodyContents!=NULL)
                    printf("        == :%p: :%s:\n",(void *)pSyntaxBody->BodyContents,
                    (pSyntaxBody->BodyContents));
                printf("        Syntax Head    :%p:\n",(void *)pSyntaxBody->BodyHead);
                printf("        Syntax Number  :%d: alternate :%d:\n",
                    pSyntaxBody->iSyntaxNumber,pSyntaxBody->iAlternateNumber);
                printf("        End of body    :%p:\n",(void *)pSyntaxBody);
                pSyntaxBody = pSyntaxBody->NextBody;
                iBody++;
            }    /* End of body loop */
            printf("    End of alternate   :%p:\n",(void *)pSyntaxAlt);
            pSyntaxAlt = pSyntaxAlt->NextAlt;
        }   /* End of alternate loop */
        printf("End of Head %s\n",pSyntaxHead->SyntaxName);
        pSyntaxHead = pSyntaxHead-> pNextHead;
    }  /* End of syntax head loop */
    printf("---------------TABLES---------------------\n");
    printf("iNextSyntax=:%5.5d: iNextAlternate=:%5.5d:\n",iNextSyntax,
            iNextAlternate);
    for (i=0;(i<iNextSyntax);i++)
    {
        printf("%5.5d :%p: :%s:\n",i,(void *)SyntaxTable[i].pSyntaxPointer,
               SyntaxTable[i].SyntaxName);
    }
    printf("   ...................................\n");
    for (i=0;(i<iNextAlternate);i++)
    {
        printf("%5.5d :%p: :%5.5d:\n",i,(void *)AlternateTable[i].pAlternatePointer,
               AlternateTable[i].iSyntaxNumber);
    }
    printf("------------------END----------------------\n");
    return;
}

/* ---------------------------------------------------------------- */

void SetupRoutineNames ( struct sRoutineNameTableElement* RoutineNameTable )
{
    /*
    *  Set up the elements in the RoutineNames table. Each element
    *  contains:
    *    1)  the name of the routine, as a character string, and
    *    2) a pointer to that routine
    *  These routines are the Code Generation routines, and the
    *  Parse (speed-up) routines, and the Lexical phase routines
    */
    strcpy(RoutineNameTable[0].sNameBody,"genadd");
    RoutineNameTable[0].BodyRoutine = genadd;
    strcpy(RoutineNameTable[1].sNameBody,"gensubtract");
    RoutineNameTable[1].BodyRoutine = gensubtract;
    strcpy(RoutineNameTable[2].sNameBody,"genmultiply");
    RoutineNameTable[2].BodyRoutine = genmultiply;
    strcpy(RoutineNameTable[3].sNameBody,"gendivide");
    RoutineNameTable[3].BodyRoutine = gendivide;
    strcpy(RoutineNameTable[4].sNameBody,"genmodulus");
    RoutineNameTable[4].BodyRoutine = genmodulus;
    strcpy(RoutineNameTable[5].sNameBody,"genlt");
    RoutineNameTable[5].BodyRoutine = genlt;
    strcpy(RoutineNameTable[6].sNameBody,"genle");
    RoutineNameTable[6].BodyRoutine = genle;
    strcpy(RoutineNameTable[7].sNameBody,"gengt");
    RoutineNameTable[7].BodyRoutine = gengt;
    strcpy(RoutineNameTable[8].sNameBody,"genge");
    RoutineNameTable[8].BodyRoutine = genge;
    strcpy(RoutineNameTable[9].sNameBody,"geneq");
    RoutineNameTable[9].BodyRoutine = geneq;
    strcpy(RoutineNameTable[10].sNameBody,"genne");
    RoutineNameTable[10].BodyRoutine = genne;
    strcpy(RoutineNameTable[11].sNameBody,"genAND");
    RoutineNameTable[11].BodyRoutine = genAND;
    strcpy(RoutineNameTable[12].sNameBody,"genOR");
    RoutineNameTable[12].BodyRoutine = genOR;
    strcpy(RoutineNameTable[13].sNameBody,"genXOR");
    RoutineNameTable[13].BodyRoutine = genXOR;
    strcpy(RoutineNameTable[14].sNameBody,"genLAND");
    RoutineNameTable[14].BodyRoutine = genLAND;
    strcpy(RoutineNameTable[15].sNameBody,"genLOR");
    RoutineNameTable[15].BodyRoutine = genLOR;
    strcpy(RoutineNameTable[16].sNameBody,"parletter");
    RoutineNameTable[16].BodyRoutine = parletter;
    strcpy(RoutineNameTable[17].sNameBody,"pardigit");
    RoutineNameTable[17].BodyRoutine = pardigit;
    strcpy(RoutineNameTable[18].sNameBody,"paroctal");
    RoutineNameTable[18].BodyRoutine = paroctal;
    strcpy(RoutineNameTable[19].sNameBody,"parchar");
    RoutineNameTable[19].BodyRoutine = parchar;
    strcpy(RoutineNameTable[20].sNameBody,"genid");
    RoutineNameTable[20].BodyRoutine = NULL;
    strcpy(RoutineNameTable[21].sNameBody,"genconst");
    RoutineNameTable[21].BodyRoutine = NULL;
    strcpy(RoutineNameTable[22].sNameBody,"genplusplus");
    RoutineNameTable[22].BodyRoutine = NULL;
    /*
    *  Because we have 22 entries above, ensure that GLOBMAXNAME
    *  has the value 22
    */

    return;
}

/* ---------------------------------------------------------------- */

int main(int argc, char* argv[])
{
    int iStatus    = EXIT_SUCCESS;
    int bGoodSyntax= FALSE;
    int bGoodParse = FALSE;
    int bGoodOutput= FALSE;
    int globMaxName;
    int k          = 0;
    char* p        = NULL;
    char* pParseBuffer = NULL;

    struct sSyntaxHead* pRootSyntax;
    struct sParseNode*  pRootParse;

    struct sRoutineNameTableElement RoutineNameTable[MAX_ROUTINE_NAMES];
    struct sSyntaxTableElement SyntaxTable [MAX_SYNTAX_ITEMS];
    struct sAlternateTableElement AlternateTable [MAX_ALTERNATE_ITEMS];
    /* A work array for one definition: */
    char SyntaxLine[SYNTAX_LINE_LIMIT+1];
    /* A work buffer containing one line to parse: */
    char ParseBuffer[PARSE_BUFFER_LENGTH+1];


    printf("Parser: Version 1.0 20000616.03:30\n");

    if(argc<4)
    {
       printf("This program requires three arguments:\n"
              "     the name of the syntax file,\n"
              "     the name of the input file, and\n"
              "     the name of the output (code) file.\n");
       return(EXIT_FAILURE);
    }

    /*
    *  Set up the initial values for this program. Note that the
    *  pathnames are those I (IDKK) have used in one of the test
    *  environments on one of the test machines.
    *  Your mileage WILL vary.
    */
    iNextSyntax    = 0;   /* Zeroth item is special */
    iNextAlternate = 0;   /* Zeroth item is special */
    SyntaxTable[0].SyntaxName[0]  = '\0';
    SyntaxTable[0].pSyntaxPointer = NULL;
    SyntaxTable[0].iStartsWith    = 0;
    SyntaxTable[0].iMustContain   = 0;
    AlternateTable[0].pAlternatePointer = NULL;
    AlternateTable[0].iSyntaxNumber     = 0;
    AlternateTable[0].iStartsWith       = 0;
    AlternateTable[0].iMustContain      = 0;
    pRootSyntax    = NULL;
    pRootParse     = NULL;
    fSyntaxFile    = NULL;
/*    strcpy(sSyntaxFileName,"Syntax.txt"); */
    fInputFile     = NULL;
/*    strcpy(sInputFileName, "TestInput.txt"); */
    fOutputFile    = NULL;
/*    strcpy(sOutputFileName,"GeneratedCode.txt"); */

    /*
    *  Set up the names and routine pointers in the reference table
    */
    SetupRoutineNames ( RoutineNameTable );

    /*
    *  Read in the syntax definition, against which we are going
    *  to parse. In this implementation this code does not test
    *  the return value which indicates whether or not the syntax
    *  definition was loaded correctly. This is a simple modification
    *  which a production program should contain.
    */
#ifdef PARDEBUGPRINT
    printf("START OF PROGRAM================================================\n");
    printf("About to read syntax\n");
#endif
    bGoodSyntax = ReadSyntax ( SyntaxTable, AlternateTable, RoutineNameTable,
                               SyntaxLine, &pRootSyntax, argv[1] );
#ifdef PARDEBUGPRINT
/*#if 0*/
    PrintSyntaxTree( SyntaxTable, AlternateTable, pRootSyntax );
/*#endif*/
    printf("Have read syntax\n");
#endif
    /*
    *  At this point we have read in the syntax definition against
    *  which we are going to parse. So now we read in the series
    *  of arithmetic expressions which we are going to parse,
    *  issuing the parse function for each of them. The program
    *  finishes when there is no more input.
    *  Open the file containing the arithmetic expressions:
    */
    fInputFile = fopen(argv[2],"r");
    if (fInputFile==NULL)
    {
        /*
        *  There was an error in opening the input file. Set the
        *  error indicator, and return to caller. Note that we
        *  could issue a specific error message here, or choose a
        *  return value which is more specific than "EXIT_FAILURE"
        *  to indicate to the caller exactly what error was encount-
        *  ered. This may safely be added by youe, the reader, as
        *  appropriate to your context.
        */
        iStatus = EXIT_FAILURE;
        return iStatus;
    }

    /*
    *  We wish to generate code to a file: open that output file:
    */
    fOutputFile = fopen(argv[3],"w");
    if (fOutputFile==NULL)
    {
        /*
        *  There was an error in opening the output file. Set the
        *  error indicator, and return to caller:
        */
        iStatus = EXIT_FAILURE;
        return iStatus;
    }

    /*
    *  Now for each line in the input file, read it in, parse it,
    *  and generate the output code:
    */
    while (!feof(fInputFile))
    {
        /*
        *  Get one input line to parse:
        */
        p = fgets(ParseBuffer,PARSE_BUFFER_LENGTH,fInputFile);
        if(p!=NULL)
        {
            /*
            *  There is something in the input buffer. Remove from
            *  it all the extra spaces, perform a lexical analysis,
            *  then a syntax analysis. If all is still OK then we
            *  can generate the output code:
            */
            RemoveSpaces(ParseBuffer);
printf("ParseBuffer=[%.*s]\n",10,ParseBuffer);
/*WAIT;*/
            /* LexicalAnalyse(ParseBuffer) */
            k = 0;
            pParseBuffer = ParseBuffer;

            /* assert(NULL == pRootParse);  */

            bGoodParse = Parse ( &pParseBuffer, &k, pRootSyntax, &pRootParse );
#ifdef PARDEBUGPRINT
            PrintParseTree(pRootParse,0);
#endif
            /*
            *  At this point we can test the value in "bGoodParse" to see
            *  whether the previous parse worked correctly. If it did
            *  then we can go and generate the output code:
            */
            if (bGoodParse)
            {
                bGoodOutput = GenerateOutputCode ( pRootParse );
                /*
                *  Here we can test "bGoodOutput" to see whether the
                *  code generator had any problems.
                */
            }
            /*
            *  At this point a good program would release back the memory
            *  which was gotten to contain the parse tree:
            */
/* >>>> */
        }   /* end of "if/then" found a new line to parse */
    }       /* end of "while" reading the input expression file */

    /*
    *  Since we do not need the abstract syntax structure again, it
    *  should be released back:
    */
#if 0
    FreeWholeSyntax ( );
#endif

    /*
    *  At this point we could change the value of iStatus depending
    *  on the values in bGoodSyntax, bGoodParse and bGoodOutput, all
    *  of which should be TRUE. If any of these is not TRUE then we
    *  should change iStatus to EXIT_FAILURE. This code would be
    *  required in a production version, but from this demonstration
    *  it has been omitted. 
    */

    /*
    *  We have finished. All that remains is to close the input and
    *  output files, and return to the caller:
    */

    if (fSyntaxFile!=NULL)
    {
        fclose(fSyntaxFile);
    }
    if (fInputFile!=NULL)
    {
        fclose(fInputFile);
    }
    if (fOutputFile!=NULL)
    {
        fclose(fOutputFile);
    }

    return iStatus;
}

/*
*  Copyright (C) 2000  Ian D. K. Kelly,
*                      idkk Consultancy Ltd.
*                      Macmillan Computer Publishing
*  GNU General Public Licence: see copyright reference information
*  at the start of this file.
*/
