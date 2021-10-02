/**************************************/
/*                                    */
/*   Code from the book C Unleashed   */
/*   Macmillan, 2000                  */
/*   Chapter 6: Data Files            */
/*   Steve Summit 2000-03-17          */
/*                                    */
/**************************************/

#define TRUE 1
#define FALSE 0

#include <stddef.h>

int
csvburst(char *line, char *arr[], int narr)
{
	char *p;
	int na = 0;
	char prevc = ',';   /* force recognizing first field */
	char *dp = NULL;
	int inquote = FALSE;

	for(p = line; *p != '\0'; prevc = *p, p++)
		{
		if(prevc == ',' && !inquote)
			{
			/* start new field */
			if(dp != NULL)
				*dp = '\0';  /*terminate prev*/
			if(na >= narr)
				return na;
			arr[na++] = p;
			dp = p;
			if(*p == '"')
				{
				inquote = TRUE;
				continue;      /* skip quote */
				}
			}

		if(inquote && *p == '"')
			{
			/* doubled quote goes to one quote; */
			/* otherwise quote ends quote mode */
			if(*(p+1) != '"')
				inquote = FALSE;
			p++;		/* skip first quote */
			}

		if(*p != ',' || inquote)
			*dp++ = *p;
		}

	if(dp != NULL)
		*dp = '\0';

	if(na < narr)
		arr[na] = NULL;

	return na;
}
