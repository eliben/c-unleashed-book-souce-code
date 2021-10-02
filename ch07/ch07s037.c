#include <stdio.h>
#include <assert.h>

int main(void)
{
  char buff[32];
  printf("Type your name\n");
  assert(NULL != fgets(buff, sizeof buff, stdin));
  printf("Your name is %s\n", buff);
  return 0;
}
