#include <stdio.h>

int foo(char *p)
{
  return (int)sizeof(p);
}

int bar(char s[100])
{
  return (int)sizeof(s);
}

int main(void)
{
  char baz[100];

  printf("sizeof char[100] in main() = %d\n",
          (int)sizeof baz);
  printf("sizeof char * in foo()     = %d\n", foo(baz));
  printf("sizeof char[100] in bar()  = %d\n", bar(baz));
  return 0;
}
