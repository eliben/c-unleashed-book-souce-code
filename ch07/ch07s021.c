#include <stdio.h>
#include <stdlib.h>

int main(void)
{
  char *filename;
  FILE *fp;
  int Result = EXIT_SUCCESS;
  int i;

  printf("This program calculates perfect squares.\n");
  printf("Please enter a filename for the output:\n");

  fgets(filename, 13, stdin);
  fp = fopen(filename, "w");
  if(fp != NULL)
  {
    fprintf(fp, "Perfect squares\n");
    for(i = 0; i < 100; i++)
    {
      fprintf(fp, "%2d   %4d\n", i, i * i);
    }
    fclose(fp);
  }
  else
  {
    printf("Couldn't open file %s\n", filename);
    Result = EXIT_FAILURE;
  }
  return 0;
}
