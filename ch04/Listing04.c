#include <stdio.h>
#include <time.h>

int main(void)
{
    time_t timeval;
    struct tm tmval;
    char inbuff[30];

    for (;;) {
        printf("\nEnter a date and time formatted as YYYY/MM/DD HH:MM:SS\n");
        printf("or q to quit\n");
        fflush(stdout);
        if (fgets(inbuff, sizeof inbuff, stdin) == NULL)
            break;
        if (inbuff[0] == 'q' && inbuff[1] == '\n')
            break;

        if (sscanf(inbuff, "%d/%d/%d %d:%d:%d",
                   &tmval.tm_year, &tmval.tm_mon, &tmval.tm_mday,
                   &tmval.tm_hour, &tmval.tm_min, &tmval.tm_sec) != 6) {
            printf("Invalid date format\n");
            continue;
        }
        tmval.tm_year -= 1900;
        tmval.tm_mon--;
        tmval.tm_isdst = -1;

        if ((timeval = mktime(&tmval)) == (time_t)-1) {
            printf("time_t cannot represent that date and time\n");
            continue;
        }

        printf("The date and time you entered is %s", asctime(&tmval));
    }

    return 0;
}
