/**************************************/
/*                                    */
/*   Code from the book C Unleashed   */
/*   Macmillan, 2000                  */
/*   Chapter 6: Data Files            */
/*   Steve Summit 2000-03-17          */
/*                                    */
/**************************************/

/*
 *  Takes a string (line) and builds an array of pointers to each word in it.
 *  Words are separated by spaces as defined by isspace().  At most maxwords
 *  pointers are calculated.  \0's are inserted in line, so that each word
 *  becomes a string in its own right.  The number of words is returned.
 */

#include <stddef.h>
#include <ctype.h>

int getwords(char *line, char *words[], int maxwords)
{
char *p = line;
int nwords = 0;

while(1)
	{
	while(isspace(*p)) p++;
	if(*p == '\0') return nwords;
	words[nwords++] = p;
	while(!isspace(*p) && *p != '\0') p++;
	if(*p == '\0') return nwords;
	if(nwords >= maxwords) return nwords;
	*p++ = '\0';
	}
}
