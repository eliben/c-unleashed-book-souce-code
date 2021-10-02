
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <float.h>

void swap(char ** array, int i, int j)
{
  char * tmp;

  tmp = array[i];
  array[i] = array[j];
  array[j] = tmp;
}

int compare(char * str1, char * str2)
{
  char *tmp1, *tmp2, *x, *y;
  int result;

  tmp1 = malloc(strlen(str1)+1);
  if (tmp1 == NULL)
  {
    perror("ran out of memory");
    exit(1);
  }

  x = str1;
  y = tmp1;
  while (*x != '\0')
    *y++ = toupper(*x++);

  tmp2 = malloc(strlen(str2)+1);
  if (tmp2 == NULL)
  {
    perror("ran out of memory");
    exit(1);
  }

  x = str2;
  y = tmp2;
  while (*x != '\0')
    *y++ = toupper(*x++);

  result = strcmp(tmp1, tmp2);
  free(tmp1);
  free(tmp2);
  return result;
}

void bubblesort(char ** array, int len)
{
  int i, j;

  for (i = 0; i < len - 1; i++)
  {
    for (j = 0; j < len - 1; j++)
    {
      if (compare(array[j], array[j+1]) > 1)
        swap(array, j, j+1);
    }
  }
}

int main(int argc, char ** argv)
{
  int i, count;
  char ** array;
  char buffer[500];
  FILE * file;

  if (argc < 2) exit(0);

  file = fopen(argv[1], "r");
  if (file == NULL)
  {
    perror(argv[1]);
    exit(1);
  }

  array = calloc(10000, sizeof(char *));
  for (i = 0; i < 10000; i++)
  {
    char * x;

    x = fgets(buffer, sizeof(buffer), file);
    if (x == NULL) break;

    array[i] = malloc(strlen(buffer)+1);
    strcpy(array[i], buffer);
  }
  count = i;

  fclose(file);

  bubblesort(array, count);

  for (i = 1; i < count; i ++)
    printf("%s", array[i]);

  free(array);
  return 0;
}
