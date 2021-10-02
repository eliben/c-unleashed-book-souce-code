#include <stdio.h>

int main(void)
{
  char CharacterSet[128] = {0}; /* init array to all 0 */
  int ch;
  int i;

  while((ch = getchar()) != EOF)
  {
    ++CharacterSet[ch];
  }

  printf("Char  Frequency\n");
  printf("----  ---------\n");
  for(i = 0; i < sizeof CharacterSet; i++)
  {
    printf("%3d  %d\n", i, CharacterSet[i]);
  }
  return 0;
}
