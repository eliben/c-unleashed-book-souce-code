#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <time.h>

int main(void)
{
    time_t timeval;
    struct tm *tmptr;
    char *localename, *category;
    char format[100];
    char timestr[30];

    if ((localename = getenv("LOCALE")) != NULL) {
        if ((category = setlocale(LC_TIME, localename)) == NULL)
            printf("Warning - locale change failed\n");
        else
            printf("Locale changed to %s (%s)\n", localename, category);
    }

    for (;;) {
        printf("Enter a strftime() format string or q to quit\n");
        fflush(stdout);
        if (fgets(format, sizeof format, stdin) == NULL)
            break;
        if (format[0] == 'q' && format[1] == '\n')
            break;

        timeval = time(NULL);
        tmptr = localtime(&timeval);
        if (strftime(timestr, sizeof timestr, format, tmptr) == 0)
            printf("The timestr array isn't big enough\n");
        else
            printf("%s", timestr);
    }
    return 0;
}
