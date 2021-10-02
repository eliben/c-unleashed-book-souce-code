/**************************************/
/*                                    */
/*   Code from the book C Unleashed   */
/*   Macmillan, 2000                  */
/*   Chapter 6: Data Files            */
/*   Steve Summit 2000-03-17          */
/*                                    */
/**************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 *  This version of inifetch() differs from the one printed
 *  in the chapter in that it only opens the .ini file, and
 *  searches for the requested section, when necessary
 *  (not redundantly on each call).
 */

#define TRUE 1
#define FALSE 0

#define MAXLINE 1000

static FILE *fp = NULL;
static char *curfile = NULL;
static char *cursect = NULL;
static long int sectoffset;

static int inisearch(FILE *, const char *);

char *
inifetch(const char *file, const char *sect, const char *key)
{
static char line[MAXLINE];
char *p, *retp = NULL;
int len;

if(fp == NULL || curfile == NULL || strcmp(curfile, file) != 0)
	{
	if(fp != NULL)
		fclose(fp);
	if(curfile != NULL)
		{ free(curfile); curfile = NULL; }
	if(cursect != NULL)
		{ free(cursect); cursect = NULL; }
	fp = fopen(file, "r");
	if(fp == NULL)
		return NULL;

	curfile = malloc(strlen(file) + 1);
	if(curfile != NULL)
		strcpy(curfile, file);
	}

if(cursect != NULL && strcmp(cursect, sect) == 0)
	fseek(fp, sectoffset, SEEK_SET);
else	{
	if(cursect != NULL)
		{ free(cursect); cursect = NULL; }
	rewind(fp);
	if(!inisearch(fp, sect))
		return NULL;
	sectoffset = ftell(fp);

	cursect = malloc(strlen(sect) + 1);
	if(cursect != NULL)
		strcpy(cursect, sect);
	}

/* search for key */
len = strlen(key);
while(fgets(line, MAXLINE, fp) != NULL)
	{
	if(*line == '[')
		break;
	if(strncmp(line, key, len) == 0 &&
				line[len] == '=')
		{
		retp = &line[len+1];
		if((p = strrchr(retp, '\n')) != NULL)
			*p = '\0';
		break;
		}
	}

return retp;
}

static int
inisearch(FILE *fp, const char *sect)
{
char line[MAXLINE];
int len = strlen(sect);
while(fgets(line, MAXLINE, fp) != NULL)
	{
	if(*line != '[')
		continue;
	if(strncmp(&line[1], sect, len) == 0 &&
					line[1+len] == ']')
		return TRUE;	/* found it */
	}

return FALSE;
}
