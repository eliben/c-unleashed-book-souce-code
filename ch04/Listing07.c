/******************************************************************************
 * weeknum_ISO8601.c       December 1999       L.Kirby
 *
 * Calculates the week number of a day in the year based on the ISO8601 week
 * numbering scheme. The arguments are:
 *
 * t - a pointer to a struct tm in which the following members must be set and
 *     normalised to the standard ranges specified (as standard library
 *     functions gmtime, localtime and mktime should do):
 *     
 *     tm_wday - The day of the week of the day in question:
 *               0-Sunday -> 6-Saturday
 *
 *     tm_yday - The day of the year of the day in question: 0 -> 365
 *               January 1st is day 0.
 *
 *     tm_year - The year since 1900.
 *
 * firstDOW - This defines the day of the week on which a week starts:
 *            0-Sunday -> 6-Saturday. Normal ISO8601 weeks start on
 *            a Monday in which case this should be 1.
 *
 ******************************************************************************
 *
 * The week number is a value between 1 and 53 inclusive defined according to
 * the following rules:
 *
 * 1. The Gregorian calendar is assumed.
 *
 * 2. There are always 7 consecutive days with the same week number.
 *
 * 3. January 4th is defined to be in week 1. Equivalently week 1 is the
 *    first week of the year which has at least 4 days in that year.
 *
 * 4. firstDOW defines the day of the week which starts a new week i.e. has
 *    a different week number from the previous day.
 *
 * 5. Week numbers increase in sequence from week 1 until the week that is
 *    defined to be week 1 of the following year.
 *
 * It follows that:
 *
 * 6. Up to January 3rd may be in either week 1 of the current year or in
 *    weeks 52 or 53 of the previous year.
 *
 * 7. 29th December onwards may be in either weeks 52 or 53 of the current
 *    year or week 1 of the following year.
 *
 * XPG4 specifies a slight difference to the ISO rules which is implemented
 * if the XPG4_WEEKNUMS macro is defined
 *
 ******************************************************************************
 */

#include <time.h>

#define is_leap(year) (!((year) % 4) && (((year) % 100) || !((year) % 400)))

int weeknum_ISO8601(const struct tm *t, int firstDOW)
{
    const int tmp1 = firstDOW - t->tm_wday;
    const int tmp2 = t->tm_yday + ((tmp1 > 0) ? 3 : 10) + tmp1;
    const int fourthdaynum = tmp2 % 7;
    int       weeknum = tmp2 / 7;

    if (weeknum == 0) {
#ifdef XPG4_WEEKNUMS
        weeknum = 53;
#else
        const int yearnum = t->tm_year + (1900 % 400)-1;

        weeknum = (fourthdaynum + is_leap(yearnum) >= 6) ? 53 : 52;
#endif
    } else if (weeknum == 53) {
        const int yearnum = t->tm_year + (1900 % 400);

        if (fourthdaynum > is_leap(yearnum))
            weeknum = 1;
    }

    return weeknum;
}
