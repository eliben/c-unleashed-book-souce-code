/*  genargs.c - code generator
 *
 *  GENARGS - argv parser generator
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


/* This program generates the GetArgs() function.
 * It takes as input (from stdin) a file with the
 * following grammar:
 *
 *  infile:
 *    specifier
 *    infile specifier
 *
 *  specifier: one of
 *    switchspecifier
 *    doublespecifier
 *    longspecifier
 *    stringspecifier
 *
 *  switchspecifier:
 *    -identifier
 *
 *  doublespecifier:
 *    identifier D
 *
 *  longspecifier:
 *    identifier L
 *
 *  stringspecifier:
 *    identifier S length
 *
 *  identifier:
 *    letter
 *    identifiertail letter
 *
 *  identifiertail:
 *    letterorunderscore identifiertail
 *    digit identifiertail
 *    letterorunderscore
 *    digit
 *
 *  letterorunderscore:
 *    letter
 *    _
 *
 *  letter: one of
 *    A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
 *    a b c d e f g h i j k l m n o p q r s t u v w x y z
 *
 *  digit: one of
 *    0 1 2 3 4 5 6 7 8 9
 *
 *  length:
 *    1 to 255
 *
 *
 * Example input:
 * -r infile S 20 outfile S 20 lbound D ubound D maxrec L
 *
 * Output: a program fragment on stdout.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "dllist.h"

#define INDENT_2  "  "
#define INDENT_4  "    "

#define MAX_IDENT_LEN 32
#define MAX_STR_LEN   255

#define LINES_PER_SCREEN 23



#define BOOL   'B'
#define DOUBLE 'D'
#define LONG   'L'
#define STRING 'S'

/* Arguments of type 'B' are switches,
 * which are either On or Off, so they
 * don't need a data type or a length.
 */
#define NON_SWITCH_TYPES "DLS"


#define GET_VARIABLE   'V'
#define GET_TYPE       'T'
#define GET_LEN        'L'
#define STOP           'S'

#define SWITCH_CHAR    '-'


typedef struct ARGUMENT
{
  char Name[MAX_IDENT_LEN];
  int Type;
  size_t Len; /* for strings */
} ARGUMENT;

int ParseInput(DLLIST **ArgList,
               FILE *InStream,
               FILE *LogStream)
{
  int Status = EXIT_SUCCESS;

  ARGUMENT Arg = {0};

  char InBuff[256];
  char *VarName;
  char *VarType;
  char *VarLen;
  char *EndPtr;

  const char Delims[] = " ,\t\n";
  char *Data;
  char *NextToken;
  int HaveData = 0;
  int State = GET_VARIABLE;

  while(EXIT_SUCCESS == Status &&
        State != STOP)
  {
    fprintf(LogStream,
            "Status report: Data? %s\n",
            HaveData ? "Yes" : "No");

    if(!HaveData)
    {
      Data = fgets(InBuff, sizeof InBuff, InStream);
      if(Data != NULL)
      {
        fprintf(LogStream,
                "Status report: Got data %s\n",
                InBuff);
        HaveData = 1;
        NextToken = InBuff;
      }
      else
      {
        if(State != GET_VARIABLE)
        {
          fprintf(LogStream,
                  "Syntax error - unexpected "
                  "end of stream.\n");
          Status = EXIT_FAILURE;
        }
        State = STOP;
      }
    }

    fprintf(LogStream,
            "Status report: Current State: %c\n",
            State);
      
    switch(State)
    {
      case STOP:
        break;

      case GET_VARIABLE:
        VarName = strtok(NextToken, Delims);
        NextToken = NULL;

        if(VarName == NULL)
        {
          fprintf(LogStream,
                  "Status report: Couldn't find "
                  "variable name. Looking again...\n");
          HaveData = 0;
        }
        else
        {
          fprintf(LogStream,
                  "Status report: Found variable name "
                  "[%s].\n",
                  VarName);

          if(VarName[0] == SWITCH_CHAR)
          {
            if(strlen(VarName + 1) > MAX_IDENT_LEN)
            {
              fprintf(LogStream,
                      "Constraint violation -"
                      " Identifier name too long.\n");
              Status = EXIT_FAILURE;
              State = STOP;
            }
            else
            {
              strcpy(Arg.Name, VarName + 1);
              Arg.Type = BOOL;
              Arg.Len = 0;
              if(DL_SUCCESS != DLAppend(ArgList,
                                        0,
                                        &Arg,
                                        sizeof Arg))
              {
                /* A */
                fprintf(LogStream,
                        "Program error A - Couldn't"
                        " add to list (memory alloc"
                        "ation failure?).\n");
                Status = EXIT_FAILURE;
                State = STOP;
              }
            }
          }
          else
          {
            if(strlen(VarName) > MAX_IDENT_LEN)
            {
              fprintf(LogStream,
                      "Constraint violation - "
                      "Identifier name too long.\n");
              Status = EXIT_FAILURE;
              State = STOP;
            }
            else
            {
              strcpy(Arg.Name, VarName);
              State = GET_TYPE;
            }
          }
        }
        break;

      case GET_TYPE:
        VarType = strtok(NextToken, Delims);
        NextToken = NULL;
        if(VarType == NULL)
        {
          fprintf(LogStream,
                  "Status report: Couldn't find variable"
                  " type. Looking again...\n");
          HaveData = 0;
        }
        else
        {
          fprintf(LogStream,
                  "Status report: Found variable"
                  " type [%s].\n",
                  VarType);

          if(VarType[1] != '\0' ||
             strchr(NON_SWITCH_TYPES, VarType[0]) == NULL)
          {
            fprintf(LogStream,
                    "Syntax error - Unknown type %s.\n",
                    VarType);

            State = STOP;
            Status = EXIT_FAILURE;
          }
          else
          {
            fprintf(LogStream,
                    "Status report: saving var [%s]\n",
                    VarName);

            Arg.Type = VarType[0];
            Arg.Len = 0;
            if(VarType[0] == STRING)
            {
              State = GET_LEN;
            }
            else
            {
              if(DL_SUCCESS != DLAppend(ArgList,
                                        0,
                                        &Arg,
                                        sizeof Arg))
              {
                /* B */
                fprintf(LogStream,
                        "Program error B - Couldn't add"
                        " to list (memory allocation fa"
                        "ilure?).\n");
                Status = EXIT_FAILURE;
                State = STOP;
              }
              else
              {
                State = GET_VARIABLE;
              }
            }
          }
        }
        break;
      case GET_LEN:
        VarLen = strtok(NextToken, Delims);
        NextToken = NULL;

        if(VarLen == NULL)
        {
          fprintf(LogStream,
                  "Status report: Couldn't find string "
                  "length. Looking again...\n");
          HaveData = 0;
        }
        else
        {
          fprintf(LogStream,
                  "Status report: Found string"
                  " length %s.\n",
                  VarLen);

          Arg.Len = (size_t)strtoul(VarLen, &EndPtr, 10);
          if(EndPtr == VarLen ||
             Arg.Len == 0     ||
             Arg.Len > MAX_STR_LEN)
          {
            fprintf(LogStream,
                    "Constraint violation - "
                    "illegal string length.\n");
            State = STOP;
            Status = EXIT_FAILURE;
          }
          else
          {
            if(DL_SUCCESS != DLAppend(ArgList,
                                      0,
                                      &Arg,
                                      sizeof Arg))
            {
              /* C */
              fprintf(LogStream,
                      "Program error C - Couldn't"
                      " add to list (memory alloc"
                      "ation failure?).\n");

              Status = EXIT_FAILURE;
              State = STOP;
            }
            else
            {
              State = GET_VARIABLE;
            }
          }
        }
        break;
      default:
        /* D */
        fprintf(LogStream,
                "Program error D - Unknown state %c.\n",
                State);
        fflush(LogStream);
        assert(0); /* This program is broken */
        State = STOP;
        State = EXIT_FAILURE;
        break;
    }
  }
  
  return Status;
}

int WalkArgs(int Tag, void *p, void *Parms)
{
  ARGUMENT *Arg = p;
  FILE *fp = Parms;

  switch(Arg->Type)
  {
    case BOOL:
      fprintf(fp,
              "-%s\n",
              Arg->Name);
      break;
    case STRING:
      fprintf(fp,
              "%s, %c, %u\n",
              Arg->Name,
              Arg->Type,
              Arg->Len);
      break;
    default:
      fprintf(fp,
              "%s, %c\n",
              Arg->Name,
              Arg->Type);
      break;
  }

  return 0;
}

int WriteHeaders(FILE *OutStream, char *OutFile)
{
  fprintf(OutStream, "/* %s */\n", OutFile);
  fprintf(OutStream,
          "/* automatically generated file. Do not\n");
  fprintf(OutStream,
          " * modify this file by hand. Change\n");
  fprintf(OutStream,
          " * genargs.c and regenerate the file\n");
  fprintf(OutStream,
          " * instead.\n");
  fprintf(OutStream,
          " */\n");

  fprintf(OutStream, "#include <stdio.h>\n");
  fprintf(OutStream, "#include <stdlib.h>\n");
  fprintf(OutStream, "#include <string.h>\n\n");

  fprintf(OutStream,
          "#define SWITCH_CHAR '%c'\n\n",
          SWITCH_CHAR);

  return EXIT_SUCCESS;
}

int WriteTypedef(FILE *OutStream, DLLIST *ArgList)
{
  ARGUMENT *Arg;
  fprintf(OutStream, "typedef struct ARG\n{\n");

  while(ArgList)
  {
    Arg = DLGetData(ArgList, NULL, NULL);

    fprintf(OutStream, "  ");

    switch(Arg->Type)
    {
      case BOOL:
        fprintf(OutStream, "int    %s;\n", Arg->Name);
        break;
      case LONG:
        fprintf(OutStream, "long   %s;\n", Arg->Name);
        break;
      case DOUBLE:
        fprintf(OutStream, "double %s;\n", Arg->Name);
        break;
      case STRING:
        fprintf(OutStream, "char   %s[%u];\n",
               Arg->Name,
               (unsigned)(Arg->Len + 1));
        break;

    }

    ArgList = DLGetNext(ArgList);
  }

  fprintf(OutStream, "} ARG;\n\n");

  return EXIT_SUCCESS;
}

int CountBools(DLLIST *ArgList)
{
  int Count = 0;

  ARGUMENT *Arg;

  while(ArgList)
  {
    Arg = DLGetData(ArgList, NULL, NULL);

    if(BOOL == Arg->Type)
    {
      ++Count;
    }

    ArgList = DLGetNext(ArgList);
  }

  return Count;
}


int WriteFunction(FILE *OutStream, DLLIST *ArgList)
{
  DLLIST *Start;
  ARGUMENT *Arg;
  int CompArgs;
  int OptArgs;
  int ThisArg;
  int ThisCompArg;

  char *Indent = INDENT_2;

  OptArgs = CountBools(ArgList);
  CompArgs = DLCount(ArgList) - OptArgs;

  Start = DLGetFirst(ArgList);

  fprintf(OutStream,
          "int GetArgs(int argc,"
          " char **argv, ARG *argp)\n");
  fprintf(OutStream, "{\n");
  fprintf(OutStream, "  int ThisArg;\n");
  fprintf(OutStream, "  int CompArg;\n");

  fprintf(OutStream, "\n");
  fprintf(OutStream, "  if(argc <= %d)\n", CompArgs);
  fprintf(OutStream, "  {\n");
  fprintf(OutStream, "    return -1;\n");
  fprintf(OutStream, "  }\n");
  fprintf(OutStream, "  if(argc > %d)\n", CompArgs +
                                          OptArgs + 1);
  fprintf(OutStream, "  {\n");
  fprintf(OutStream, "    return -2;\n");
  fprintf(OutStream, "  }\n");

  fprintf(OutStream, "  for(ThisArg = 1, CompArg = 0;"
         " ThisArg < argc; ThisArg++)\n");
  fprintf(OutStream, "  {\n");


  if(OptArgs > 0)
  {
    fprintf(OutStream, "    if(argv[ThisArg][0] == "
                       "SWITCH_CHAR)\n");
    fprintf(OutStream, "    {\n      ");

    do
    {
      Arg = DLGetData(ArgList, NULL, NULL);
      if(Arg->Type == BOOL)
      {
        fprintf(OutStream,
                "if(strcmp(\"%c%s\","
                " argv[ThisArg]) == 0)\n",
                SWITCH_CHAR,
                Arg->Name);

        fprintf(OutStream, "      {\n");

        fprintf(OutStream,
                "        argp->%s = 1;\n",
                Arg->Name);

        fprintf(OutStream, "      }\n");
        fprintf(OutStream, "      else ");
      }

      ArgList = DLGetNext(ArgList);
    } while(ArgList != NULL);

    fprintf(OutStream, "\n");
    fprintf(OutStream, "      {\n");

    fprintf(OutStream,
           "        printf(\"Unknown switch "
           "%%s\\n\", argv[ThisArg]);\n");

    fprintf(OutStream, "        return ThisArg;\n");
    fprintf(OutStream, "      }\n");

    ArgList = Start;

    fprintf(OutStream, "    }\n");
    fprintf(OutStream, "    else\n");
    fprintf(OutStream, "    {\n");

    Indent = INDENT_4;
  }

  fprintf(OutStream, "%s  switch(CompArg)\n", Indent);
  fprintf(OutStream, "%s  {\n", Indent);

  for(ThisArg = 0, ThisCompArg = 0;
      ThisCompArg < CompArgs;
      ThisArg++)
  {
    Arg = DLGetData(ArgList, NULL, NULL);

    if(Arg->Type != BOOL)
    {
      fprintf(OutStream,
              "%s    case %d:\n",
              Indent,
              ThisCompArg);

      fprintf(OutStream,
              "%s    {\n",
              Indent);

      switch(Arg->Type)
      {
        case LONG:
          fprintf(OutStream,
                  "%s        char *EndPtr;\n",
                  Indent);

          fprintf(OutStream,
                  "%s        argp->%s = "
                  "strtol(argv[ThisArg]"
                  ", &EndPtr, 10);\n",
                  Indent,
                  Arg->Name);

          fprintf(OutStream,
                  "%s        if(EndPtr =="
                  " argv[ThisArg])\n",
                  Indent);

          fprintf(OutStream,
                  "%s        {\n",
                  Indent);

          fprintf(OutStream,
                  "%s          return ThisArg;\n",
                  Indent);

          fprintf(OutStream,
                  "%s        }\n",
                  Indent);
          break;
        case DOUBLE:
          fprintf(OutStream,
                  "%s        char *EndPtr;\n",
                  Indent);

          fprintf(OutStream,
                  "%s        argp->%s = strtod"
                  "(argv[ThisArg], &EndPtr);\n",
                  Indent,
                  Arg->Name);

          fprintf(OutStream,
                  "%s        if(EndPtr "
                  "== argv[ThisArg])\n",
                 Indent);

          fprintf(OutStream,
                  "%s        {\n",
                  Indent);

          fprintf(OutStream,
                  "%s          return ThisArg;\n",
                  Indent);

          fprintf(OutStream,
                  "%s        }\n",
                  Indent);
          break;
        case STRING:
          fprintf(OutStream,
                  "%s        if(strlen(argv"
                  "[ThisArg]) > %d)\n",
                  Indent,
                  Arg->Len);
          fprintf(OutStream,
                  "%s        {\n",
                  Indent);
          fprintf(OutStream,
                  "%s          return ThisArg;\n",
                  Indent);

          fprintf(OutStream,
                  "%s        }\n",
                  Indent);

          fprintf(OutStream,
                  "%s        strcpy(argp->%s, "
                  "argv[ThisArg]);\n",
                  Indent,
                  Arg->Name);
          break;
        default:
          /* Unsupported type, already validated. */
          assert(0);
          break;
      }
      fprintf(OutStream,
              "%s        break;\n",
              Indent);

      fprintf(OutStream,
              "%s    }\n",
              Indent);

      ++ThisCompArg;
    }

    ArgList = DLGetNext(ArgList);
  }

  fprintf(OutStream, "%s  }\n", Indent);
  fprintf(OutStream, "\n");
  fprintf(OutStream, "%s  ++CompArg;\n", Indent);

  if(OptArgs > 0)
  {
    fprintf(OutStream, "    }\n");
  }

  fprintf(OutStream, "  }\n");

  fprintf(OutStream, "  return 0;\n");
  fprintf(OutStream, "}\n\n");

  return EXIT_SUCCESS;
}

int WriteMain(FILE *OutStream,
              FILE *HelpStream,
              DLLIST *ArgList,
              int InternalApp)
{
  DLLIST *Arg;
  ARGUMENT *Data;

  char *MainText[] =
  {
    "",
    "%", /* write prototype for ApplicationMain() */
    "",
    "void Help(void);",
    "",
    "int main(int argc, char **argv)",
    "{",
    "  int Status;",
    "  int ArgResult;",
    "  ARG ArgList = {0};",
    "",
    "  ArgResult = GetArgs(argc, argv, &ArgList);",
    "  if(ArgResult != 0)",
    "  {",
    "    Help();",
    "    Status = EXIT_FAILURE;",
    "  }",
    "  else",
    "  {",
    "    /* Calling your program... */",
    "^", /* Write function call for ApplicationMain() */
    "  }",
    "",
    "  return Status;",
    "}",
    "",
    "void Help(void)",
    "{",
    "!",
    "}",
    "",
    "&", /* Write function body for ApplicationMain() */
    NULL
  };

  int i;
  int j;
  char buffer[MAX_STR_LEN];
  char *p;

  for(i = 0; MainText[i] != NULL; i++)
  {
    switch(MainText[i][0])
    {
      case '!':
        /* Generate the Help() function */
        fprintf(OutStream, "  char buffer[8];\n\n");

        j = 0;

        while(fgets(buffer, sizeof buffer, HelpStream))
        {
          p = strchr(buffer, '\n');
          if(p != NULL)
          {
            *p = '\0';
          }
          fprintf(OutStream,
                  "  printf(\"%s\\n\");\n",
                  buffer);

          ++j;

          if(j == LINES_PER_SCREEN)
          {
            fprintf(OutStream,
                    "  fprintf(stderr, \"Press ENTER to"
                    " continue...\\n\");\n");
            fprintf(OutStream,
                    "  fgets(buffer, sizeof buffer, "
                    "stdin);\n");
            j = 0;
          }
        }
        break;
      case '%':
      case '&':
        if(MainText[i][0] == '&')
        {
          if(!InternalApp)
          {
            break;
          }
          fprintf(OutStream,
                  "/* Write, or call, your"
                  " application here.\n");
          fprintf(OutStream,
                  " * ApplicationMain must"
                  " return int. 0 indicates\n");
          fprintf(OutStream,
                  " * success. Any other value "
                  "indicates failure.\n");
          fprintf(OutStream,
                  " */\n\n");
        }

        /* Generate the prototype */
        fprintf(OutStream, "int ApplicationMain(");

        j = 0;

        for(Arg = ArgList;
            Arg != NULL;
            Arg = DLGetNext(Arg))
        {
          Data = DLGetData(Arg, NULL, NULL);
          switch(Data->Type)
          {
            case BOOL:
              fprintf(OutStream,
                      "%sint    ",
                      j == 0 ?
                           "" :
                           ", \n        "
                           "            ");
              break;
            case LONG:
              fprintf(OutStream,
                      "%slong   ",
                      j == 0 ?
                           "" :
                           ", \n        "
                           "            ");
              break;
            case DOUBLE:
              fprintf(OutStream,
                      "%sdouble ",
                      j == 0 ?
                           "" :
                           ", \n        "
                           "            ");
              break;
            case STRING:
              fprintf(OutStream,
                      "%schar * ",
                      j == 0  ?
                           "" :
                           ", \n        "
                           "            ");
              break;
            default:
              fprintf(stderr,
                      "program error in fun"
                      "ction WriteMain()\n");
              assert(0);
              break;
          }

          ++j;

          fprintf(OutStream, "%s", Data->Name);
        }

        fprintf(OutStream,
                ")%s\n",
                MainText[i][0] == '%' ? ";" : "");

        if(MainText[i][0] == '&')
        {
          fprintf(OutStream, "{\n  return 0;\n}\n\n");
        }

        break;
      case '^':
        /* Generate the call */
        fprintf(OutStream,
                "    Status = (ApplicationMain(");

        j = 0;

        for(Arg = ArgList;
            Arg != NULL;
            Arg = DLGetNext(Arg))
        {
          Data = DLGetData(Arg, NULL, NULL);

          fprintf(OutStream, "%sArgList.%s",
                  j == 0 ?
                  "" :
                  ",\n                              ",
                  Data->Name);
          ++j;
        }

        fprintf(OutStream,
                ") == 0) ?\n     EXIT_SUCCESS "
                ":\n     EXIT_FAILURE;\n");

        break;
      default:
        fprintf(OutStream, "%s\n", MainText[i]);
        break;
    }
  }

  return EXIT_SUCCESS;
}

int ApplicationMain(int internal, 
                    char * InFile, 
                    char * OutFile, 
                    char * LogFile, 
                    char * HelpFile)
{
  int Status;

  DLLIST *ArgList = NULL;

  FILE *fpIn, *fpOut, *fpLog, *fpHelp;

  fpLog = fopen(LogFile, "w");
  if(fpLog == NULL)
  {
    fprintf(stderr,
            "Can't open file %s for logging.\n",
            LogFile);
    fprintf(stderr, "Using stderr.\n");
    fpLog = stderr;
  }
  fpIn = fopen(InFile, "r");
  if(fpIn == NULL)
  {
    fprintf(stderr,
            "Can't open file %s for reading.\n",
            InFile);
    fprintf(stderr, "Using stdin.\n");
    fpIn = stdin;
  }
  fpOut = fopen(OutFile, "w");
  if(fpOut == NULL)
  {
    fprintf(fpLog,
            "Can't open file %s for writing.\n",
            OutFile);

    fprintf(fpLog, "Using stdout.\n");
    fpOut = stdout;
  }
  fpHelp = fopen(HelpFile, "r");
  if(fpHelp == NULL)
  {
    fprintf(stderr,
            "Can't open file %s for reading.\n",
            HelpFile);
    fprintf(stderr, "Using stdin.\n");
    fpHelp = stdin;
  }

  Status = ParseInput(&ArgList, fpIn, fpLog);

  if(EXIT_SUCCESS == Status)
  {
    DLWalk(ArgList, WalkArgs, fpLog);

    Status = WriteHeaders(fpOut, OutFile);
  }

  if(EXIT_SUCCESS == Status)
  {
    Status = WriteTypedef(fpOut, ArgList);
  }
  if(EXIT_SUCCESS == Status)
  {
    Status = WriteFunction(fpOut, ArgList);
  }
  if(EXIT_SUCCESS == Status)
  {
    Status = WriteMain(fpOut, fpHelp, ArgList, internal);
  }

  if(fpLog != stderr)
  {
    fclose(fpLog);
  }
  if(fpIn != stdin)
  {
    fclose(fpIn);
  }
  if(fpOut != stdout)
  {
    fclose(fpOut);
  }
  if(fpHelp != stdin)
  {
    fclose(fpHelp);
  }

  DLDestroy(&ArgList);

  return Status;
}
