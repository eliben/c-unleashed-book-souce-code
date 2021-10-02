/**************************************/
/*                                    */
/*   Code from the book C Unleashed   */
/*   Macmillan, 2000                  */
/*   Chapter 6: Data Files            */
/*   Steve Summit 2000-03-17          */
/*                                    */
/**************************************/

#include <stdio.h>
#include <time.h>

#define TRUE 1
#define FALSE 0

void
timetprint(time_t t, FILE *ofp)
{
char buf[25];
struct tm *tp = localtime(&t);
strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", tp);
fputs(buf, ofp);
}

time_t
timetparse(char *str)
{
int y, mo, d;
int h, m, s = 0;
struct tm tm = {0};

if(sscanf(str, "%d-%d-%dT%d:%d:%d", &y, &mo, &d, &h, &m, &s)
	< 5 && sscanf(str, "%4d%2d%2dT%2d%2d%2d",
			&y, &mo, &d, &h, &m, &s) < 5)
	return (time_t)(-1);

tm.tm_year = y - 1900; tm.tm_mon = mo - 1; tm.tm_mday = d;
tm.tm_hour = h; tm.tm_min = m; tm.tm_sec = s;

return mktime(&tm);
}

void puttime(time_t t, FILE *ofp)
{
	int i;
	for(i = 0; i < 6; i++)
		{
		putc(t & 0xff, ofp);
		t >>= 8;
		}
}

int gettime(time_t *tp, FILE *ifp)
{
	time_t t;
	char buf[6];
	int i;
	if(fread(buf, 1, 6, ifp) != 6)
		return FALSE;
	t = buf[5];	/* sign extend, probably */
	for(i = 5-1; i >= 0; i--)
		t = (t << 8) | (buf[i] & 0xff);
	*tp = t;
	return TRUE;
}
