/**************************************/
/*                                    */
/*   Code from the book C Unleashed   */
/*   Macmillan, 2000                  */
/*   Chapter 6: Data Files            */
/*   Steve Summit 2000-03-17          */
/*                                    */
/**************************************/

#include <stdio.h>
#include <string.h>

void csvwrite(char *arr[], int narr, FILE *ofp)
{
	int i;
	for(i = 0; i < narr; i++)
		{
		if(strpbrk(arr[i], ",\"\n") == NULL)
			fputs(arr[i], ofp);
		else	{
			char *p;
			putc('"', ofp);
			for(p = arr[i]; *p != '\0'; p++)
				{
				if(*p == '"')
					fputs("\"\"", ofp);
				else if(*p == '\n')
					putc(' ', ofp);
				else	putc(*p, ofp);
				}
			putc('"', ofp);
			}
		putc(i < narr-1 ? ',' : '\n', ofp);
		}
}
