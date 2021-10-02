#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void)
{
  clock_t start, end;

  unsigned long count = 0;
  start = clock();

  while(malloc(1024))
  {
    ++count;
    if(count % 16 == 0)
    {
      end = clock();

      printf("%7f CPU seconds: %d kilobytes allocated\n",
             (double)(end - start) / CLOCKS_PER_SEC,
             count);
    }
  }
  printf("Out of memory (no more kilobytes).\n");
  return 0;
}
