#include <stdio.h>
#include <time.h>

int main(void)
{
    clock_t start, end;

    start = clock();

    /* The code you want timed goes here */

    end = clock();

    printf("Interval = %.2f seconds\n", (double)(end-start) /
                                           (double)CLOCKS_PER_SEC);
    return 0;
}

