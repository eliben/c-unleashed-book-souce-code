#include <stdio.h>

typedef struct ARRAY_INT_4_6
{
  int array[4][6];
} ARRAY_INT_4_6;

int SumArray(ARRAY_INT_4_6 *);

int main(void)
{
  ARRAY_INT_4_6 Array =
  {
    {
      { 1, 2, 3, 4, 5, 6 },
      { 2, 3, 4, 5, 6, 7 },
      { 3, 4, 5, 6, 7, 8 },
      { 4, 5, 6, 7, 8, 9 }
    }
  };
  int Total;

  Total = SumArray(&Array);

  printf("Total is %d\n", Total);

  return 0;
}

int SumArray(ARRAY_INT_4_6 *a)
{
  size_t outer, inner;
  int Total = 0;

  for(outer = 0;
      outer < sizeof a->array / sizeof a->array[0];
      outer++)
  {
    for(inner = 0;
        inner < sizeof a->array[0] / sizeof a->array[0][0];
        inner++)
    {
      Total += a->array[outer][inner];
    }
  }
  return Total;
}
