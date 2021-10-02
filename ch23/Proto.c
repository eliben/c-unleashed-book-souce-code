/*  proto.c - CGI Sample Application
*
*
*  Copyright (C) 1999   Chad Dixon
*                       Macmillan Computer Publishing
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
*  Chad Dixon may be contacted by email at:
*     http://www.loopy.org
*
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cgi.h"

typedef struct PROTOTYPE
{
  char *Name;
  char *Prototype;
} PROTOTYPE;

int CompProtos(const void *p1, const void *p2)
{
  const PROTOTYPE *Pro1 = p1, *Pro2 = p2;
  return strcmp(Pro1->Name, Pro2->Name);
}

char *GetPrototype(char *funcname)
{
  static PROTOTYPE Prototype[] =
  {
      { "fopen",      "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   FILE *fopen(const char *"
                      "filename, const char *mode);"
                      "</h3>"},

      { "freopen",    "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   FILE *freopen(const char *"
                      "filename, const char *mode, "
                      "FILE *stream);</h3>" },
      
      { "fflush",     "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   int fflush(FILE *stream);"
                      "</h3>" },
      
      { "fclose",     "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   int fclose(FILE *stream);"
                      "</h3>" },
      
      { "remove",     "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   int remove(const char *"
                      "filename);</h3>" },
      
      { "rename",     "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   int rename(const char *"
                      "oldname, const char *newname);"
                      "</h3>" },
      
      { "tmpfile",    "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   FILE *tmpfile(void);</h3>"},
      
      { "tmpnam",     "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   char *tmpnam(char s[L_tmpnam]);"
                      "</h3>" },
      
      { "setvbuf",    "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   int setvbuf(FILE *stream,"
                      " char *buf, int mode, size_t size"
                      ");</h3>" },
      
      { "setbuf",     "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   void setbuf(FILE *stream"
                      ", char *buf);</h3>" },
      
      { "fprintf",    "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   int fprintf(FILE *stream,"
                      " const char *format, ...);</h3>"},
      
      { "printf",     "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   int printf(const char *format,"
                      " ...);</h3>" },
      
      { "sprintf",    "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   int    sprintf(char *s,"
                      " const char *format, ...);</h3>" },
      
      { "vprintf",    "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   int vprintf(const char *format,"
                      " va_list arg);</h3>" },
      
      { "vfprintf",   "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   int vfprintf(FILE *stream,"
                      " const char *format, va_list "
                      "arg);</h3>" },
      
      { "vsprintf",   "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   int vsprintf(char *s, const"
                      " char *format, va_list arg);"
                      "</h3>" },
      
      { "fscanf",     "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   int fscanf(FILE *stream,"
                      " const char *format, ...);</h3>" },
      
      { "scanf",      "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   int scanf(const char *format,"
                      " ...);</h3>" },
      
      { "sscanf",     "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   int sscanf(const char *s,"
                      " const char *format, ...);</h3>" },
      
      { "fgetc",      "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   int fgetc(FILE *stream);</h3>"},
      
      { "fgets",      "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   char *fgets(char *s, int n,"
                      " FILE *stream);</h3>" },
      
      { "fputc",      "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   int fputc(int c, FILE *stream);"
                      "</h3>" },
      
      { "fputs",      "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   int fputs(const char *s,"
                      " FILE *stream);</h3>" },
      
      { "getc",       "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   int getc(FILE *stream);</h3>" },
      
      { "getchar",    "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   int getchar(void);</h3>" },
      
      { "gets",       "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   char *gets(char *s);"
                      " /* care: many experts believe"
                      "this function <b>cannot</b> be"
                      " used safely */</h3>" },
      
      { "putc",       "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   int putc(int c, FILE *stream);"
                      "</h3>" },
      
      { "putchar",    "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   int putchar(int c);</h3>" },
      
      { "puts",       "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   int puts(const char *s);</h3>"},
      
      { "ungetc",     "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   int ungetc(int c, FILE *"
                      "stream);</h3>" },
      
      { "fread",      "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   size_t fread(void *ptr,"
                      " size_t size, size_t nobj,"
                      " FILE *stream);</h3>" },
      
      { "fwrite",     "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   size_t fwrite(const void *"
                      "ptr, size_t size, size_t nobj,"
                      " FILE *stream);</h3>" },
      
      { "fseek",      "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   int fseek(FILE *stream, long"
                      " offset, int origin);</h3>" },
      
      { "ftell",      "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   long ftell(FILE *stream);</h3>"},
      
      { "rewind",     "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   void rewind(FILE *stream);"
                      "</h3>"},
      
      { "fgetpos",    "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   int fgetpos(FILE *stream,"
                      " fpos_t *ptr);</h3>" },
      
      { "fsetpos",    "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   int fsetpos(FILE *stream,"
                      " const fpos_t *ptr);</h3>" },
      
      { "clearerr",   "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   void clearerr(FILE *stream);"
                      "</h3>" },
      
      { "feof",       "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   int feof(FILE *stream);</h3>"},
      
      { "ferror",     "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   int ferror(FILE *stream);"
                      "</h3>"},
      
      { "perror",     "<h2>&lt;stdio.h&gt</h2><p><h3>"
                      "   void   perror  (const char *s);"
                      "</h3>" },
      
      { "isalnum",    "<h2>&lt;ctype.h&gt</h2><p><h3>"
                      "   int isalnum(int c);</h3>" },
      
      { "isalpha",    "<h2>&lt;ctype.h&gt</h2><p><h3>"
                      "   int isalpha(int c);</h3>" },
                      
      { "iscntrl",    "<h2>&lt;ctype.h&gt</h2><p><h3>"
                      "   int iscntrl(int c);</h3>" },
                      
      { "isdigit",    "<h2>&lt;ctype.h&gt</h2><p><h3>"
                      "   int isdigit(int c);</h3>" },
                      
      { "isgraph",    "<h2>&lt;ctype.h&gt</h2><p><h3>"
                      "   int isgraph(int c);</h3>" },
                      
      { "islower",    "<h2>&lt;ctype.h&gt</h2><p><h3>"
                      "   int islower(int c);</h3>" },
                      
      { "isprint",    "<h2>&lt;ctype.h&gt</h2><p><h3>"
                      "   int isprint(int c);</h3>" },
                      
      { "ispunct",    "<h2>&lt;ctype.h&gt</h2><p><h3>"
                      "   int ispunct(int c);</h3>" },
                      
      { "isspace",    "<h2>&lt;ctype.h&gt</h2><p><h3>"
                      "   int isspace(int c);</h3>" },
                      
      { "isupper",    "<h2>&lt;ctype.h&gt</h2><p><h3>"
                      "   int isupper(int c);</h3>" },
                      
      { "isxdigit",   "<h2>&lt;ctype.h&gt</h2><p><h3>"
                      "   int isxdigit(int c);</h3>" },
                      
      { "tolower",    "<h2>&lt;ctype.h&gt</h2><p><h3>"
                      "   int tolower(int c);</h3>" },
                      
      { "toupper",    "<h2>&lt;ctype.h&gt</h2><p><h3>"
                      "   int toupper(int c);</h3>" },
                      
      { "strcpy",     "<h2>&lt;string.h&gt</h2><p><h3>"
                      "  char *strcpy(char *s, const"
                      " char *t);</h3>" },
                      
      { "strncpy",    "<h2>&lt;string.h&gt</h2><p><h3>"
                      "  char * strncpy(char *s, const"
                      " char *t, size_t n);</h3>" },
                      
      { "strcat",     "<h2>&lt;string.h&gt</h2><p><h3>"
                      "  char *strcat(char *s, const"
                      " char *t);</h3>" },
                      
      { "strncat",    "<h2>&lt;string.h&gt</h2><p><h3>"
                      "  char *strncat(char *s, const"
                      " char *t, size_t n);</h3>" },
                      
      { "strcmp",     "<h2>&lt;string.h&gt</h2><p><h3>"
                      "  int strcmp(const char *s, const"
                      " char *t);</h3>" },
                      
      { "strncmp",    "<h2>&lt;string.h&gt</h2><p><h3>"
                      "  int strncmp(const char *s,"
                      " const char *t, size_t n);</h3>" },
                      
      { "strchr",     "<h2>&lt;string.h&gt</h2><p><h3>"
                      "  char *strchr(const char *s,"
                      " int c);</h3>" },
                      
      { "strrchr",    "<h2>&lt;string.h&gt</h2><p><h3>"
                      "  char *strrchr(const char *s, "
                      "int c);</h3>" },
                      
      { "strspn",     "<h2>&lt;string.h&gt</h2><p><h3>"
                      "  size_t strspn(const char *s,"
                      " const char *t);</h3>" },
                      
      { "strcspn",    "<h2>&lt;string.h&gt</h2><p><h3>"
                      "  size_t strcspn(const char *s,"
                      " const char *t);</h3>" },
                      
      { "strpbrk",    "<h2>&lt;string.h&gt</h2><p><h3>"
                      "  char *strpbrk(const char *s,"
                      " const char *t);</h3>" },
                      
      { "strstr",     "<h2>&lt;string.h&gt</h2><p><h3>"
                      "  char *strstr(const char *s,"
                      " const char *t);</h3>" },
                      
      { "strlen",     "<h2>&lt;string.h&gt</h2><p><h3>"
                      "  int strlen(const char *s);"
                      "</h3>" },
                      
      { "strerror",   "<h2>&lt;string.h&gt</h2><p><h3>"
                      "  char *strerror(size_t n);"
                      "</h3>" },
                      
      { "strtok",     "<h2>&lt;string.h&gt</h2><p><h3>"
                      "  char *strtok(char *s, const"
                      " char *t);</h3>" },
                      
      { "memcpy",     "<h2>&lt;string.h&gt</h2><p><h3>"
                      "  void *memcpy(void *s, const"
                      " void *t, size_t n);</h3>" },
                      
      { "memmove",    "<h2>&lt;string.h&gt</h2><p><h3>"
                      "  void *memmove(void *s, const"
                      " void *t, size_t n);</h3>" },
                      
      { "memcmp",     "<h2>&lt;string.h&gt</h2><p><h3>"
                      "  int memcmp(const void *s,"
                      " const void *t, size_t n);"
                      "</h3>" },
                      
      { "memchr",     "<h2>&lt;string.h&gt</h2><p><h3>"
                      "  void *memchr(const void *s,"
                      " int c, size_t n);</h3>" },
                      
      { "memset",     "<h2>&lt;string.h&gt</h2><p><h3>"
                      "  void *memset(void *s, int c,"
                      " size_t n);</h3>" },
                      
      { "sin",        "<h2>&lt;math.h&gt</h2><p>  <h3>"
                      "  double sin(double x);</h3>" },
                      
      { "cos",        "<h2>&lt;math.h&gt</h2><p>  <h3>"
                      "  double cos(double x);</h3>" },
                      
      { "tan",        "<h2>&lt;math.h&gt</h2><p>  <h3>"
                      "  double tan(double x);</h3>" },
                      
      { "asin",       "<h2>&lt;math.h&gt</h2><p>  <h3>"
                      "  double asin(double x);</h3>" },
                      
      { "acos",       "<h2>&lt;math.h&gt</h2><p>  <h3>"
                      "  double acos(double x);</h3>" },
                      
      { "atan",       "<h2>&lt;math.h&gt</h2><p>  <h3>"
                      "  double atan(double x);</h3>" },
                      
      { "atan2",      "<h2>&lt;math.h&gt</h2><p>  <h3>"
                      "  double atan2(double y, double"
                      " x);</h3>" },
                      
      { "sinh",       "<h2>&lt;math.h&gt</h2><p>  <h3>"
                      "  double sinh(double x);</h3>" },
                      
      { "cosh",       "<h2>&lt;math.h&gt</h2><p>  <h3>"
                      "  double cosh(double x);</h3>" },
                      
      { "tanh",       "<h2>&lt;math.h&gt</h2><p>  <h3>"
                      "  double tanh(double x);</h3>" },
                      
      { "exp",        "<h2>&lt;math.h&gt</h2><p>  <h3>"
                      "  double exp(double x);</h3>" },
                      
      { "log",        "<h2>&lt;math.h&gt</h2><p>  <h3>"
                      "  double log(double x);</h3>" },
                      
      { "log10",      "<h2>&lt;math.h&gt</h2><p>  <h3>"
                      "  double log10(double x);</h3>" },
                      
      { "pow",        "<h2>&lt;math.h&gt</h2><p>  <h3>"
                      "  double pow(double x, double y);"
                      "</h3>" },
                      
      { "sqrt",       "<h2>&lt;math.h&gt</h2><p>  <h3>"
                      "  double sqrt(double x);</h3>" },
                      
      { "ceil",       "<h2>&lt;math.h&gt</h2><p>  <h3>"
                      "  double ceil(double x);</h3>" },
                      
      { "floor",      "<h2>&lt;math.h&gt</h2><p>  <h3>"
                      "  double floor(double x);</h3>" },
                      
      { "fabs",       "<h2>&lt;math.h&gt</h2><p>  <h3>"
                      "  double fabs(double x);</h3>" },
                      
      { "ldexp",      "<h2>&lt;math.h&gt</h2><p>  <h3>"
                      "  double ldexp(double x, int n);"
                      "</h3>" },
                      
      { "frexp",      "<h2>&lt;math.h&gt</h2><p>  <h3>"
                      "  double frexp(double x, int *"
                      "exp);</h3>" },
                      
      { "modf",       "<h2>&lt;math.h&gt</h2><p>  <h3>"
                      "  double modf(double x, double *"
                      "ip);</h3>" },
                      
      { "fmod",       "<h2>&lt;math.h&gt</h2><p>  <h3>"
                      "  double fmod(double x, double "
                      "y);</h3>" },
                      
      { "atof",       "<h2>&lt;stdlib.h&gt</h2><p><h3>"
                      "  double atof(const char *s);"
                      "</h3>" },
                      
      { "atoi",       "<h2>&lt;stdlib.h&gt</h2><p><h3>"
                      "  int atoi(const char *s);</h3>"},
                      
      { "atol",       "<h2>&lt;stdlib.h&gt</h2><p><h3>"
                      "  long atol(const char *s);</h3>"},
                      
      { "strtod",     "<h2>&lt;stdlib.h&gt</h2><p><h3>"
                      "  double strtod(const char *s,"
                      " char **endp);</h3>" },
                      
      { "strtol",     "<h2>&lt;stdlib.h&gt</h2><p><h3>"
                      "  long strtol(const char *s,"
                      " char **endp, int base);</h3>" },
                      
      { "strtoul",    "<h2>&lt;stdlib.h&gt</h2><p><h3>"
                      "  long strtoul(const char *s,"
                      " char **endp, int base);</h3>" },
                      
      { "rand",       "<h2>&lt;stdlib.h&gt</h2><p><h3>"
                      "  int rand(void);</h3>" },
                      
      { "srand",      "<h2>&lt;stdlib.h&gt</h2><p><h3>"
                      "void srand(unsigned int seed);"
                      "</h3>" },
                      
      { "calloc",     "<h2>&lt;stdlib.h&gt</h2><p><h3>"
                      "  void *calloc(size_t nobj,"
                      " size_t size);</h3>" },
                      
      { "malloc",     "<h2>&lt;stdlib.h&gt</h2><p><h3>"
                      "  void *malloc(size_t size);"
                      "</h3>" },
                      
      { "realloc",    "<h2>&lt;stdlib.h&gt</h2><p><h3>"
                      "  void *realloc(void *p, size_t"
                      " size);</h3>" },
                      
      { "free",       "<h2>&lt;stdlib.h&gt</h2><p><h3>"
                      "  void free(void *p);</h3>" },
                      
      { "abort",      "<h2>&lt;stdlib.h&gt</h2><p><h3>"
                      "  void abort(void);</h3>" },
                      
      { "exit",       "<h2>&lt;stdlib.h&gt</h2><p><h3>"
                      "  void exit(int status);</h3>" },
                      
      { "atexit",     "<h2>&lt;stdlib.h&gt</h2><p><h3>"
                      "  int atexit(void (*function)"
                      "(void));</h3>" },
                      
      { "system",     "<h2>&lt;stdlib.h&gt</h2><p><h3>"
                      "  int system(const char *s);"
                      "</h3>"},
                      
      { "getenv",     "<h2>&lt;stdlib.h&gt</h2><p><h3>"
                      "  int getenv(const char *name);"
                      "</h3>" },
                      
      { "bsearch",    "<h2>&lt;stdlib.h&gt</h2><p><h3>"
                      "  void *bsearch (const void *key,"
                      " const void *base, size_t n,"
                      " size_t size, int(*cmp)(const"
                      " void *keyval, const void *datum"
                      "));</h3>" },
                      
      { "qsort",      "<h2>&lt;stdlib.h&gt</h2><p><h3>"
                      "  void qsort(void *base, size_t n,"
                      " size_t size, int(*cmp)(const void"
                      " *, const void *));</h3>" },
                      
      { "abs",        "<h2>&lt;stdlib.h&gt</h2><p><h3>"
                      "int abs(int n);</h3>" },
                      
      { "labs",       "<h2>&lt;stdlib.h&gt</h2><p><h3>"
                      "  long labs(long n);</h3>" },
                      
      { "div",        "<h2>&lt;stdlib.h&gt</h2><p><h3>"
                      "  div_t div(int num, int denom);"
                      "</h3>" },
                      
      { "ldiv",       "<h2>&lt;stdlib.h&gt</h2><p><h3>"
                      "  ldiv_t div(long num, long"
                      " denom);</h3>" },
                      
      { "assert",     "<h2>&lt;assert.h&gt</h2><p><h3>"
                      "  void assert(int expression);"
                      "</h3>/* This is actually a macro,"
                      " not a function. It is not"
                      " compiled if NDEBUG is defined. "
                      "*/" },
                      
      { "va_start",   "<h2>&lt;stdarg.h&gt</h2><p><h3>"
                      "  [MACRO] va_start(va_list ap,"
                      " lastarg);</h3>" },
                      
      { "va_arg",     "<h2>&lt;stdarg.h&gt</h2><p><h3>"
                      "  [MACRO] type va_arg(va_list ap,"
                      " type);</h3>" },
                      
      { "va_end",     "<h2>&lt;stdarg.h&gt</h2><p><h3>"
                      "  [MACRO] void va_end(va_list ap);"
                      "</h3>" },
                      
      { "setjmp",     "<h2>&lt;setjmp.h&gt</h2><p><h3>"
                      "  int setjmp(jmp_buf env);</h3>" },
                      
      { "longjmp",    "<h2>&lt;setjmp.h&gt</h2><p><h3>"
                      "  void longjmp(jmp_buf env,"
                      " int val);</h3>" },
                      
      { "signal",     "<h2>&lt;signal.h&gt</h2><p><h3>"
                      "  void (*signal(int sig, void"
                      " (*handler)(int)))(int);</h3>" },
                      
      { "raise",      "<h2>&lt;signal.h&gt</h2><p><h3>"
                      "  int raise(int sig);</h3>" },
                      
      { "clock",      "<h2>&lt;time.h&gt</h2><p>  <h3>"
                      "  clock_t clock(void);</h3>" },
                      
      { "time",       "<h2>&lt;time.h&gt</h2><p>  <h3>"
                      "  time_t time(time_t *tp);</h3>" },
                      
      { "difftime",   "<h2>&lt;time.h&gt</h2><p>  <h3>"
                      "  double difftime(time_t time2,"
                      " time_t time1);</h3>" },
                      
      { "mktime",     "<h2>&lt;time.h&gt</h2><p>  <h3>"
                      "  time_t mktime(struct tm *tp)"
                      ";</h3>" },
                      
      { "asctime",    "<h2>&lt;time.h&gt</h2><p>  <h3>"
                      "  char *asctime(const struct "
                      "tm *tp);</h3>" },
                      
      { "ctime",      "<h2>&lt;time.h&gt</h2><p>  <h3>"
                      "  char *ctime(const time_t *tp);"
                      "</h3>" },
                      
      { "gmtime",     "<h2>&lt;time.h&gt</h2><p>  <h3>"
                      "  struct tm* gmtime(const time_t"
                      " *tp);</h3>" },
                      
      { "localtime",  "<h2>&lt;time.h&gt</h2><p>  <h3>"
                      "  struct tm* localtime(const"
                      " time_t *tp);</h3>" },
                      
      { "strftime",   "<h2>&lt;time.h&gt</h2><p>  <h3>"
                      "  size_t strftime(char *s,"
                      " size_t smax, const char *fmt,"
                      " const struct tm *tp);</h3>" },
  };
  
  size_t count = sizeof Prototype / sizeof Prototype[0];
  PROTOTYPE *p;
  PROTOTYPE Key;
  char *result = NULL;

  qsort(Prototype,
        count,
        sizeof Prototype[0],
        CompProtos);

  Key.Name = funcname;
  p = bsearch(&Key,
              Prototype,
              count,
              sizeof Prototype[0],
              CompProtos);

  if(p != NULL)
  {
    result = p->Prototype;
  }

  return result;
}

/* CGI program to provide ANSI C Standard
 * Library Function Prototype, given a
 * function name.
 *
 * Note: an input string of "version" will
 * produce, not an ANSI C standard library
 * function prototype (oddly enough), but
 * the version information for this program.
 *
 * Note: This code is generic, and can be
 * used in pretty well all CGI programs. The
 * application-specific bit (which is gratifyingly
 * short) is all in App().
 *
 */

void App(CGI_LIST *List)
{
  char *Proto = NULL;
  int GettingVersionInfo = 0;
  char *VersionInfo =
    "C Unleashed CGI Demo v1.05";
  char *ScriptName = getenv("SCRIPT_NAME");

  /* We are only expecting one item in the list,
   * so let's use it.
   */

  if(strcmp(List->Value, "version") == 0)
  {
    GettingVersionInfo = 1;
  }

  printf("<HTML>\n");
  printf("  <HEAD>\n");
  printf("    <TITLE>\n      ");
  if(GettingVersionInfo)
  {
    printf("Version information for CGI Demo\n");
  }
  else
  {
    printf("Function prototype for %s\n", List->Value);
  }
  printf("    </TITLE>\n");
  printf("  </HEAD>\n");
  printf("  <BODY>\n");
  if(GettingVersionInfo)
  {
    printf("<H1>Version information for CGI Demo<H1>\n");
    printf("<P><P><H2>%s</H2>\n", VersionInfo);
  }
  else
  {
    printf("  <H1><B>%s</B></H1><P>\n", List->Value);

    Proto = GetPrototype(List->Value);
    if(NULL == Proto)
    {
      printf("<I>%s not found. Please"
             " check spelling.</I>\n",
             List->Value);
    }
    else
    {
      printf("<I>%s</I>\n", Proto);
    }
  }

  if(ScriptName != NULL)
  {
    printf("    <BR>\n");
    printf("    <FORM METHOD=\"POST\""
                " ACTION=\"%s\">\n",
                ScriptName);

    printf("      <BR><HR><BR>\n");
    printf("      <CENTER>\n");
    printf("        <FONT SIZE=4>\n");
    printf("          Next search:\n");
    printf("          <BR>\n<BR>\n");
    printf("          Please type in the name "
                      "of the function\n");
    printf("          whose prototype you"
                      " wish to see.\n");
    printf("        </FONT>\n");

    printf("        <BR><BR>\n");
    printf("        <INPUT TYPE=\"text\" NAME="
           "\"function\" SIZE=\"20\">\n");
    printf("        <BR><BR>\n");

    printf("        <INPUT TYPE=\"submit\">\n");
    printf("      </CENTER>\n");
    printf("    </FORM>\n");
  }
    
  printf("  </BODY>\n");
  printf("</HTML>\n");
}

int main(void)
{
  CGI_LIST *List = NULL;
  char *CopyOfQueryString = NULL;
  int ErrorCode = 0;

  /* The next line is required, for HTTP
   * protocol reasons. It will not appear
   * on your output HTML page.
   */
  printf("Content-Type: text/html\n\n");   

  CopyOfQueryString = ReadCGIData(&ErrorCode);

  if(NULL == CopyOfQueryString)
  {
    switch(ErrorCode)
    {
      case CGI_NULL_REQ_METHOD:
        printf("No CGI request method "
               "could be identified.\n");
        break;
      case CGI_UNKNOWN_METHOD:
        printf("Unsupported CGI request method.\n");
        break;
      case CGI_NO_QUERY_STRING:
        printf("No CGI query string found.\n");
        break;
      case CGI_NO_MEMORY:
        printf("Memory allocation failure.\n");
        break;
      case CGI_BAD_CONTENT_LENGTH:
        printf("Missing or invalid CONTENT_LENGTH.\n");
        break;
      case CGI_NO_DATA:
        printf("No CGI input data could be found.\n");
        break;
      default:
        printf("Unknown CGI Error.\n");
        break;
    }

    /* We can't proceed, so we might as well exit. */
    return EXIT_FAILURE;
  }

  List = CGICreateList(CopyOfQueryString);

  /* Whether that worked or not, we don't need
   * CopyOfQueryString any more, so let's dump it.
   */
  free(CopyOfQueryString);
  
  if(NULL == List)
  {
    printf("Can't parse CGI data.\n");
    return EXIT_FAILURE;
  }

  /****************************
   *                          *
   * This is where your CGI   *
   * application gets called! *
   *                          *
   ****************************/
  App(List);

  /* Let's not forget to clean up */
  CGIDestroyList(List);

  return 0;
}

