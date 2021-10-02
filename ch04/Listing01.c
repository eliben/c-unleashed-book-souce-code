#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void)
{
    time_t start, end;

    start = time(NULL);
    if (start == (time_t)-1) {
        printf("Sorry, the time is unavailable on this system\n");
        exit(EXIT_FAILURE);
    }

    printf("Please wait a few seconds and then press your ENTER/RETURN key");
    fflush(stdout);

    getchar();

    end = time(NULL);
    printf("\nThe interval was %.2f seconds\n", difftime(end, start));

    return 0;
}
