#include <stdio.h>
#include <string.h>

int main(void)
{
  size_t len = 0;
  char buffer[1024] = {0};
  if(fgets(buffer, sizeof buffer, stdin) != NULL)
  {
    len = strlen(buffer);
    printf("The input string is %u bytes long.\n",
           (unsigned)len);
  }

  return 0;
}
