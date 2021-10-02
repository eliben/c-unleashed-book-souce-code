/* argproc.c */
/* automatically generated file. Do not
 * modify this file by hand. Change
 * genargs.c and regenerate the file
 * instead.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SWITCH_CHAR '-'

typedef struct ARG
{
  int    internal;
  char   InFile[256];
  char   OutFile[256];
  char   LogFile[256];
  char   HelpFile[256];
} ARG;

int GetArgs(int argc, char **argv, ARG *argp)
{
  int ThisArg;
  int CompArg;

  if(argc <= 4)
  {
    return -1;
  }
  if(argc > 6)
  {
    return -2;
  }
  for(ThisArg = 1, CompArg = 0; ThisArg < argc; ThisArg++)
  {
    if(argv[ThisArg][0] == SWITCH_CHAR)
    {
      if(strcmp("-internal", argv[ThisArg]) == 0)
      {
        argp->internal = 1;
      }
      else 
      {
        printf("Unknown switch %s\n", argv[ThisArg]);
        return ThisArg;
      }
    }
    else
    {
      switch(CompArg)
      {
        case 0:
        {
            if(strlen(argv[ThisArg]) > 255)
            {
              return ThisArg;
            }
            strcpy(argp->InFile, argv[ThisArg]);
            break;
        }
        case 1:
        {
            if(strlen(argv[ThisArg]) > 255)
            {
              return ThisArg;
            }
            strcpy(argp->OutFile, argv[ThisArg]);
            break;
        }
        case 2:
        {
            if(strlen(argv[ThisArg]) > 255)
            {
              return ThisArg;
            }
            strcpy(argp->LogFile, argv[ThisArg]);
            break;
        }
        case 3:
        {
            if(strlen(argv[ThisArg]) > 255)
            {
              return ThisArg;
            }
            strcpy(argp->HelpFile, argv[ThisArg]);
            break;
        }
      }

      ++CompArg;
    }
  }
  return 0;
}


int ApplicationMain(int    internal, 
                    char * InFile, 
                    char * OutFile, 
                    char * LogFile, 
                    char * HelpFile);

void Help(void);

int main(int argc, char **argv)
{
  int Status;
  int ArgResult;
  ARG ArgList = {0};

  ArgResult = GetArgs(argc, argv, &ArgList);
  if(ArgResult != 0)
  {
    Help();
    Status = EXIT_FAILURE;
  }
  else
  {
    /* Calling your program... */
    Status = (ApplicationMain(ArgList.internal,
                              ArgList.InFile,
                              ArgList.OutFile,
                              ArgList.LogFile,
                              ArgList.HelpFile) == 0) ?
     EXIT_SUCCESS :
     EXIT_FAILURE;
  }

  return Status;
}

void Help(void)
{
  char buffer[8];

  printf("genargs - C program generator, R Heathfield, January 2000\n");
  printf("\n");
  printf("Usage:\n");
  printf("genargs [-internal] infile outfile logfile helpfile\n");
  printf("\n");
  printf("Purpose: creates a C program with\n");
  printf("         primitive argv checking\n");
  printf("\n");
  printf("   -internal: optional switch. If\n");
  printf("              specified, generates\n");
  printf("              an application function.\n");
  printf("\n");
  printf("   infile:    specifies the arguments\n");
  printf("              expected by the program,\n");
  printf("              using the grammar detailed below.\n");
  printf("\n");
  printf("   outfile:   filename in which to store output.\n");
  printf("\n");
  printf("   logfile:   parse errors recorded here.\n");
  printf("\n");
  printf("   helpfile:  used for writing Help() function,\n");
  printf("              displayed when args are incorrect.\n");
  printf("\n");
  fprintf(stderr, "Press ENTER to continue...\n");
  fgets(buffer, sizeof buffer, stdin);
  printf("--- infile grammar ---\n");
  printf("\n");
  printf("infile:\n");
  printf("  specifier\n");
  printf("  infile specifier\n");
  printf("\n");
  printf("specifier: one of\n");
  printf("  switchspecifier\n");
  printf("  doublespecifier\n");
  printf("  longspecifier\n");
  printf("  stringspecifier\n");
  printf("\n");
  printf("switchspecifier:\n");
  printf("  -identifier\n");
  printf("\n");
  printf("doublespecifier:\n");
  printf("  identifier D\n");
  printf("\n");
  printf("longspecifier:\n");
  printf("  identifier L\n");
  printf("\n");
  printf("stringspecifier:\n");
  printf("  identifier S length\n");
  fprintf(stderr, "Press ENTER to continue...\n");
  fgets(buffer, sizeof buffer, stdin);
  printf("\n");
  printf("identifier:\n");
  printf("  letter\n");
  printf("  identifiertail letter\n");
  printf("  \n");
  printf("identifiertail:\n");
  printf("  letterorunderscore identifiertail\n");
  printf("  digit identifiertail\n");
  printf("  letterorunderscore\n");
  printf("  digit\n");
  printf("  \n");
  printf("letterorunderscore:\n");
  printf("  letter\n");
  printf("  _\n");
  printf("\n");
  printf("letter: one of\n");
  printf("  A B C D E F G H I J K L M N O P Q R S T U V W X Y Z\n");
  printf("  a b c d e f g h i j k l m n o p q r s t u v w x y z\n");
  printf("\n");
  printf("digit: one of\n");
  printf("  0 1 2 3 4 5 6 7 8 9\n");
  printf("\n");
  printf("length:\n");
  fprintf(stderr, "Press ENTER to continue...\n");
  fgets(buffer, sizeof buffer, stdin);
  printf("  1 to 255\n");
  printf("\n");
}

