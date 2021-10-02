/**************************************/
/*                                    */
/*   Code from the book C Unleashed   */
/*   Macmillan, 2000                  */
/*   Chapter 6: Data Files            */
/*   Steve Summit 2000-03-17          */
/*                                    */
/**************************************/

#include <string.h>

int getcols(char *line, char *words[], int maxwords, int delim)
{
char *p = line, *p2;
int nwords = 0;

while(*p != '\0')
	{
	words[nwords++] = p;
	if(nwords >= maxwords)
		return nwords;
	p2 = strchr(p, delim);
	if(p2 == NULL)
		break;
	*p2 = '\0';
	p = p2 + 1;
	}
return nwords;
}
