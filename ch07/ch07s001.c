#include <stdio.h>

int main(void)
{
  unsigned int count = 0;
  while(getchar() != EOF)
  {
    ++count;
  }
  printf("Character count is %u\n", count);
  return 0;
}
