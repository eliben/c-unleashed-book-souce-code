/* caution - gets() is an unsafe function. Never use it */
#include <stdio.h>

int main(void)
{
  char s[13];
  printf("Please type in your name\n");
  gets(s);
  printf("Your name is %s\n", s);
  return 0;
}
