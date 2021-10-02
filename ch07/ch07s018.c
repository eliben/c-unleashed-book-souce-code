#include <stdio.h>
#include <string.h>

int main(void)
{
  char Name[12];
  char First[12];
  char Second[12];

  printf("First name?\n");
  fgets(First, sizeof First, stdin);
  printf("Last name?\n");
  fgets(Second, sizeof Second, stdin);

  strcat(Name, First);
  strcat(Name, " ");
  strcat(Name, Second);

  printf("Full name: %s\n", Name);

  return 0;
}
