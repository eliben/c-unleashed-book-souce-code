/* Code by Ian D. K. Kelly for Chapter 19, "C Unleashed"    */

/* Ch19Par.h
* 
*  Prototypes for parser.
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

#ifndef PARDEBUG
#define PARDEBUG
#endif

#ifndef PARDEBUGPRINT
#define PARDEBUGPRINT
#endif

#ifdef SYNDEBUGPRINT
#undef SYNDEBUGPRINT
#endif

#ifndef Ch19_parser2_h
#define Ch19_parser2_h

/* The maximum length, in characters, of one definition: */
#define SYNTAX_LINE_LIMIT 1024
/* The maximum number of definitions: */
#define MAX_SYNTAX_ITEMS 128
/* The maximum number of alternates with the definitions */
#define MAX_ALTERNATE_ITEMS 1024
/* The maximum number of internal routines supported: */
#define MAX_ROUTINE_NAMES 30
/* The maximum length, in characters, of one syntax name: */
#define SYNTAX_NAME_LENGTH 32
/* The aximum length, in characters of a C routine name: */
#define ROUTINE_NAME_LENGTH 16
/*
*  The maximum length of a line for parsing. This should err
*  on the long side, rather than the short:
*/
#define PARSE_BUFFER_LENGTH 4096
/* The number of internal code-generation routines: */
#define GLOBMAXNAME 22

/*  Syntax Table -----------------------------------------------------*/

/*
*  These are the elements which contain the description of the abstract,
*  possible syntax. These are an expression of the BNF rules written in
*  the language definition. The ultimate root of this syntax tree is
*  pointed to by SyntaxRoot.
*/

struct sSyntaxHead;
struct sSyntaxAlt;
struct sSyntaxBody;
struct sSyntaxHead* SyntaxRoot;

/*
*  A syntax rule has a name, and a pointer to the first of its
*  possible definitions. It also has an int which is the subscript
*  within the Syntax Names table of this name. The other int fields
*  are the subscripts within the StartsWith and MustContain table
*  of the corresponding lines 
*/
struct sSyntaxHead {
#ifdef PARDEBUG
    char SyntaxHeadIdent[4];
#endif
    /* The name of this syntax item. This is duplicated in the name table */
    char SyntaxName [SYNTAX_NAME_LENGTH+1];
    /* The next pair of pointers is for a double-linked list of all the   */
    /* syntax head items. This can be used for housekeeping, if needed.   */
    struct sSyntaxHead* pNextHead;
    struct sSyntaxHead* pPreviousHead;
    /* This particular head has a number of alternates - at least one:    */
    struct sSyntaxAlt* FirstAlternate;
    /* This particular head has a unique number, which is the index into  */
    /* the names table:                                                   */
    int iSyntaxNumber;
    /* If the "starts with" and "must contain" have been calculated, then */
    /* these values will be subscripts (indexes into) the tables that show*/
    /* these logical values:                                              */
    int iStartsWith;
    int iMustContain;
    /* If there is a lexical routine to short-circuit this node, this is  */
    /* a pointer to it, together with a flag indicating that this is a    */
    /* lexical item - one that can be processed during the lexical scan:  */
    int (*LexRoutine) ( void* );
    int iIsLexical;
};

/* One alternate definition has the first element of recognition, and */
/* a pointer to the next alternate definition. If this pointer is null*/
/* then there are no further alternates. The int iStartsWith is the   */
/* subscript of the relevant row in the "starts with" array, and the  */
/* int iMustContain is the subscript of the relvant row in the "must  */
/* contain" array. If either of these is zero, then it represents no  */
/* specification. The remaining int field is the number of this alter-*/
/* nate, by which this alternate may be referenced elsewhere.         */
struct sSyntaxAlt {
#ifdef PARDEBUG
    char SyntaxAltIdent[4];
#endif
    /* This alternate must have at leastone body item - point to this:*/
    struct sSyntaxBody* ThisBody;
    /* The next pair of items construct a double-linked list, which   */
    /* is used for finding the next alternate during parsing. The back*/
    /* pointer could be used for housekeeping:                        */
    struct sSyntaxAlt*  NextAlt;
    struct sSyntaxAlt*  PreviousAlt;
    /* Point back to the parent SYntax Head item, for reference:      */
    struct sSyntaxHead* ParentHead;
    /* That parent head has a unique number - this is a copy of that: */
    int iSyntaxNumber;
    /* This alternate also has a unique number - the primary copy:    */
    int iAlternateNumber;
    /* If the "starts with" and "must contain" tables have been calc- */
    /* ulated, then these are the indexes into the tables that contain*/
    /* that information:                                              */
    int iStartsWith;
    int iMustContain;
};

/* One element of recognition contains a pointer to the next element  */
/* of recognition. If this is null then there are no further elements.*/
/* It also may contains a pointer to a function which detects whether */
/* this element is recognized. The two parameters to that function are*/
/* the subscript of the first character which corresponds in the input*/
/* text, and the last. The final procedure pointer is the code-genera-*/
/* tion procedure, which is called only if this is a final good branch*/
/* The code generation procedure also takes two parameters, whare are */
/* the subscript of the first and last characters in the input.       */
struct sSyntaxBody {
#ifdef PARDEBUG
    char SyntaxBodyIdent[4];
#endif
    /* The next pair of items are the links in a double-linked list of*/
    /* body items. The forward (next) pointer is used during the      */
    /* parsing process. The back pointer could be used in housekeeping*/
    struct sSyntaxBody* NextBody;
    struct sSyntaxBody* PreviousBody;
    /* This body item lies within a specific alternate: point to that:*/
    struct sSyntaxAlt*  ParentAlt;
    /* Only one of the two folliwing pointers wil be set, as this body*/
    /* item is EITHER for a named syntax head OR for a specific char- */
    /* acter string. If both of these are NULL it is an error, and if */
    /* neither of these is NULL it is also an error:                  */
    struct sSyntaxHead* BodyHead;
    char* BodyContents;
    /* There are two optional routines that may be associated with    */
    /* this body item during the parse process - one to short-circuit */
    /* the recognition process, and the other to contruct the lexical */
    /* information. These will usually both be NULL:                  */
    int (*BodyCheck) ( void* );
    int (*LexCheck)  ( void* );
    /* If this item is associated with code generation, then this is  */
    /* a pointer to the code generation routine to be called at this  */
    /* point in the analysis:                                         */
    int (*CodeGenerate) ( void* );
    /* This body item lies within a specific Syntax Head, which has a */
    /* unique number, and a specific alternate, which also has a un-  */
    /* iue number - these are repeated here, for reference:           */
    int iSyntaxNumber;
    int iAlternateNumber;
    /* If the "starts with" and "must contain" tables are computed    */
    /* then these are the pointers [indexes] to the array elements    */
    /* that record those values:                                      */
    int iStartsWith;
    int iMustContain;
};


/*  Parse Table ------------------------------------------------------*/

/*
*  These contain the result of a parse. That is, these elements are used
*  to show how a particular input text matches to the abstract syntax.
*  The Parse of the current text is the tree rooted in pRootParse.
*/
struct sParseNode;

/*
*  The root of a parse structure points to the abstract syntax that it
*  matches, and the particular branch (alternate). It also indicates
*  start and end character of recognition.
*  As part of the working of the parser we need to remember which
*  part of the abstract sytax we are working on. Hence there is a
*  working pointer back to a body item in the abstract syntax. When
*  this is NULL we have either not yet started, or we have finished.
*/
struct sParseNode {
#ifdef PARDEBUG
    char ParseNodeIdent[4];
#endif
    /* This parse node matches a particular syntax node, within an   */
    /* alternate. All parse nodes are chained together for house-    */
    /* keeping, and point to the parent parse node, and the next node*/
    /* at this level in the parse tree.                              */
    struct sParseNode*  ParentParse;
    struct sParseNode*  NextParse;
    struct sParseNode*  PreviousParse;
    /* If this node has a daughter (or set of daughter) node(s) then */
    /* point to the first of these. If this pointer is NULL then no  */
    /* daughter exists for this node - it is a leaf of the parse tree*/
    struct sParseNode*   ThisParse;
    /* This parse head also corresponds to one particular syntax     */
    /* body item within a syntax head - point to these:              */
    struct sSyntaxHead*  ThisHead;
    struct sSyntaxBody*  ThisBody;
    /* If the matching syntax item indicates a code generate, then   */
    /* point to that routine:                                        */
    int (*CodeGenerate) ( void* );
    /* This parse node has a start and end character - indexes within*/
    /* the input (parse) buffer:                                     */
    int iFirstChar;
    int iLastChar;
};



/*
*  Table of Syntax names, and pointers to their tree items. This table
*  is NOT normalized, in that there is duplication here of some informa-
*  tion that is also in the tree element itself. This is for speedier
*  access, and is not (in theory) required.
*/
struct sSyntaxTableElement {
    char SyntaxName[SYNTAX_NAME_LENGTH+1];
    struct sSyntaxHead* pSyntaxPointer;
    int iStartsWith;
    int iMustContain;
};

/*
*  Any given alternate has a number, and a possible "starts with" and
*  a possible "must contain". If these are zero, then they are not set.
*/
struct sAlternateTableElement {
    struct sSyntaxAlt* pAlternatePointer;
    int iSyntaxNumber;
    int iStartsWith;
    int iMustContain;
};

/*
*  The following table matches character strings against routine
*  names. It is used at run time to determine which of the parser
*  code generation (etc.) routines should be called according to
*  the specified syntax. Every routine returns an int, and takes
*  as parameter two int pointers.
*/
struct sRoutineNameTableElement {
    char sNameBody[ROUTINE_NAME_LENGTH+1];
    int (*BodyRoutine) ( void* );
};

/* ------------------------- GLOBAL VARIABLES ----------------------- */
/*
*  GLOBAL VARIABLES:
*  There is a lot of argument as to whether global variables are
*  ever justified. All of the following variables are used throughout
*  the parser and code generator, and are declared here as global.
*  one alternative would be to declare them as subordinate to a
*  private structure, and to pass the address of that structure to
*  every routine as an additional parameter. IMHO the manner of writing
*  used here is clearer.
*/

static int iNextSyntax;
static int iNextAlternate;
/*
*  The files which contains the syntax definitions, the expressions
*  to be parsed, and the file to contain the output:
*/
static FILE * fSyntaxFile;
/*static char sSyntaxFileName[FILENAME_MAX+1];*/
static FILE * fInputFile;
/*static char sInputFileName[FILENAME_MAX+1];*/
static FILE * fOutputFile;
/*static char sOutputFileName[FILENAME_MAX+1];*/


/* ----------------------- END GLOBAL VARIABLES --------------------- */

/* ------ Prototypes -------------------------------------------------*/

/*
*  The prototypes for the functions:
*/

int ReadSyntax        ( struct sSyntaxTableElement* SyntaxTable,
                        struct sAlternateTableElement* AlternateTable,
                        struct sRoutineNameTableElement* RoutineNameTable,
                        char* SyntaxLine,
                        struct sSyntaxHead** pRoot,
                        char* sSyntaxFileName );
int FindSyntaxName    ( struct sSyntaxTableElement* SyntaxTable,
                        char*  SyntaxName );
int ProcessSyntaxLine ( struct sSyntaxTableElement* SyntaxTable,
                        struct sAlternateTableElement* AlternateTable,
                        struct sRoutineNameTableElement* RoutineNameTable,
                        char* SyntaxLine,
                        struct sSyntaxHead* pRootSyntax );
void FreeWholeSyntax  ( struct sSyntaxTableElement* SyntaxTable );
int GetNewSyntaxHead  ( struct sSyntaxTableElement* SyntaxTable,
                        struct sRoutineNameTableElement* RoutineNameTable,
                        struct sSyntaxHead** ppNewSyntaxHead,
                        char*  Identifier );
int CreateSyntaxTableEntry (struct sSyntaxTableElement* SyntaxTable,
                        char* pSyntaxName);
void FreeSyntaxHead   ( struct sSyntaxHead*  pFreeHead );
int GetAlternates     ( struct sSyntaxTableElement* SyntaxTable,
                        struct sAlternateTableElement* AlternateTable,
                        struct sRoutineNameTableElement* RoutineNameTable,
                        char* SyntaxLine,
                        int*   j,
                        struct sSyntaxHead*  pNewSyntaxHead );
int GetOneAlternate   ( struct sSyntaxTableElement* SyntaxTable,
                        struct sRoutineNameTableElement* RoutineNameTable,
                        char* SyntaxLine,
                        int*   j,
                        struct sSyntaxAlt*   pNewAlternate );
void FreeAlternates   ( struct sSyntaxAlt**  pFreeAlt );
int GetSyntaxItem     ( struct sSyntaxTableElement* SyntaxTable,
                        struct sRoutineNameTableElement* RoutineNameTable,
                        char* SyntaxLine,
                        int*   j,
                        struct sSyntaxBody*  pNewBody );
void FreeSyntaxItem   ( struct sSyntaxBody** pFreeBody );
void SkipSpaces       ( char*  SyntaxLine,
                        int*   j );
int GetSyntaxName     ( struct sSyntaxTableElement* SyntaxTable,
                        char*  SyntaxLine,
                        int*   j );
void RemoveSpaces     ( char*  InputBuffer );
int TryMatchParse     ( char** ppInputBuffer,
                        int*   k,
                        struct sSyntaxBody*  pSyntaxP,
                        struct sParseNode**  ppParseBody );
int GetNewParseBody (   struct sParseNode*   pParentHead,
                        struct sParseNode*   pPreviousBody,
                        struct sParseNode**  ppNewBody );
int BuildNewParseBody ( struct sParseNode*   pParentHead,
                        struct sParseNode*   pPreviousBody,
                        struct sParseNode**  ppNewBody,
                        int    iFirstChar,
                        int    iLastChar );
int Parse             ( char** ppInputBuffer,
                        int*   k,
                        struct sSyntaxHead*  pRootS,
                        struct sParseNode**  ppRootP );
int GenerateOutputCode( struct sParseNode*   pRootP );
int GetNextSyntax     ( struct sSyntaxBody**  ppSyntaxP );
int SkipNextSyntax    ( struct sSyntaxBody**  ppSyntaxP );
#if 0
  struct sSyntaxBody* NextParseItem (struct sParseNode* pRootP );
#endif
void PrintSyntaxTree  ( struct sSyntaxTableElement* SyntaxTable,
                        struct sAlternateTableElement* AlternateTable,
                        struct sSyntaxHead* pRootSyntax );
int isidchar          ( char   testchar );
int GetIdentifier     ( char*  InputBuffer,
                        char*  Identifier,
                        int*   j );
int GetLexName        ( struct sSyntaxTableElement* SyntaxTable,
                        char*  SyntaxLine,
                        char*  Identifier,
                        int*   j,
                        int    k );
int ProcessOutputNode ( struct sParseNode** ppParseHead,
                        struct sParseNode** ppParseBody );
int GetRoutinePointer ( struct sRoutineNameTableElement* RoutineNameTable,
                        char*  pszRoutineName,
                        int    (**FoundRoutine) (void*) );
int parletter         ( void* );
int pardigit          ( void* );
int paroctal          ( void* );
int parchar           ( void* );
int gencomparison     ( char* pszComparator );
int testgen           ( void* );
int genlt             ( void* );
int genle             ( void* );
int gengt             ( void* );
int genge             ( void* );
int geneq             ( void* );
int genne             ( void* );
int genAND            ( void* );
int genOR             ( void* );
int genXOR            ( void* );
int genLAND           ( void* );
int genLOR            ( void* );
int genoperate        ( char*  pszOperator );
int genadd            ( void* );
int gensubtract       ( void* );
int genmutiply        ( void* );
int gendivide         ( void* );
int genmodulus        ( void* );
void SetupRoutineNames ( struct sRoutineNameTableElement* RoutineNameTable );

#endif  /* end of double copy protection, Ch19_parser2_h */

/* End of parser prototypes. */

/*
*  Copyright (C) 2000  Ian D. K. Kelly,
*                      idkk Consultancy Ltd.
*                      Macmillan Computer Publishing
*  GNU General Public Licence: see copyright reference information
*  at the start of this file.
*/

