#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void)
{
    time_t timeval;
    struct tm *tmptr;

    timeval = time(NULL);

    printf("Time as local time is %s", ctime(&timeval));

    if ((tmptr = gmtime(&timeval)) == NULL)
        printf("UTC time not available\n");
    else
        printf("Time as UTC time is %s", asctime(tmptr));
    
    return 0;
}
