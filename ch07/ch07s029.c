/* use cfollow.c, not this copy of it */

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>

#include "cfollow.h"

/*
 * GetOutputFileName():
 * This function retrieves a filename for use by fopen.
 * It's static, so it won't pollute your namespace.
 *
 * This function, although a conforming ANSI C routine, is
 * nevertheless platform-specific, because it builds
 * a filename. The filename it builds works perfectly
 * well for:
 *
 * MS-DOS     Windows   Unix
 * Linux      AmigaDOS  TOS (Atari ST)
 *
 * systems, but you may have problems if you are running
 * it on some other system. If you are running MVS, I
 * suggest you simply use an ordinary DDNAME as a
 * filename, and control the filename itself via JCL.
 * If you are running some other system, you may want
 * to tweak the sprintf to get a format appropriate to
 * your system.
 *
 * If your implementation uses 32-bit time_t epoched
 * at 1/1/1970, this code breaks in 2038. By then,
 * hopefully, you'll be using 64-bit time_t - so just
 * recompile.
 */

static void GetOutputFileName(char *OutFileName)
{
  time_t    CurrentTime;
  struct tm Now;
  long      DaysSince1999;
  long      YearsSince1999;

  time(&CurrentTime);
  memcpy(&Now, localtime(&CurrentTime), sizeof Now);

  YearsSince1999 = (long)(Now.tm_year - 99);

  DaysSince1999 = YearsSince1999 * 365 +
                 (YearsSince1999 + 3) / 4;

  /* 2100, 2200, etc are not leap years */
  if(YearsSince1999 > 100)
  {
    DaysSince1999 -= YearsSince1999 % 100;
  }
  /* 2400, 2800, etc are leap years */
  if(YearsSince1999 > 400)
  {
    DaysSince1999 += YearsSince1999 % 400;
  }

  DaysSince1999 += Now.tm_yday + 1;

  /* ensure the number fits into 4 digits */
  DaysSince1999 %= 10000;

  sprintf(OutFileName,
          "C%04ld%02d%01d.%01d%02d",
          DaysSince1999,
          Now.tm_hour,
          Now.tm_min / 10,
          Now.tm_min % 10,
          Now.tm_sec);
}


void CFollow(char * InFileName,
             int    LineNumber,
             int    DepthModifier,
             char * FormatString,
             ...)
{
  static FILE *fp = NULL;
  static long Counter = 0;
  static int Depth = 0;
  const int TabWidth = 4;
  static char OutFileName[FILENAME_MAX] = "";
  int i;
  static int CounterWidth = 0;
  long cwloop;

  va_list ArgList;

  if(DepthModifier < 0)
  {
    Depth += DepthModifier;
  }

  va_start(ArgList, FormatString);

  if(NULL == fp)
  {
    GetOutputFileName(OutFileName);
    fp = fopen(OutFileName, "w");
    if(NULL == fp)
    {
      /* We can't record in a file.
       * Bite the bullet and use stdout.
       */
      fp = stdout;
    }
    CounterWidth = 0;
    for(cwloop = 1;
        cwloop <= CFOLLOW_MAXLINE;
        cwloop *= 10L)
    {
      ++CounterWidth;
    }
  }

  if(NULL == FormatString)
  {
    if(fp != stdout)
    {
      fclose(fp);
    }
  }
  else
  {
    if(Counter < CFOLLOW_MAXLINE)
    {
      fprintf(fp,
          "%0*ld %12s (%5d) : ",
          CounterWidth,
          ++Counter,
          InFileName,
          LineNumber);

      for(i = 0; i < TabWidth * Depth; i++)
      {
        fprintf(fp, " ");
      }

      vfprintf(fp, FormatString, ArgList);
      fprintf(fp, "\n");
      fflush(fp);
    }
  }

  if(DepthModifier > 0)
  {
    Depth += DepthModifier;
  }

  va_end(ArgList);
}

/* end of cfollow.c */
