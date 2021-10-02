#include <stdlib.h>

typedef int T;

T *****Alloc5DArrayOfT(size_t m,
                       size_t n,
                       size_t p,
                       size_t q,
                       size_t r)
{
  T *****Array = NULL;
  int Success = 1;
  size_t a, b, c, d;

  Array =
      malloc(m * sizeof *Array);
  for(a = 0; a < m; a++)
  {
    Array[a] =
        malloc(n * sizeof *Array[0]);
    for(b = 0; b < n; b++)
    {
      Array[a][b] =
          malloc(p * sizeof *Array[0][0]);
      for(c = 0; c < p; c++)
      {
        Array[a][b][c] =
            malloc(q * sizeof *Array[0][0][0]);
        for(d = 0; d < q; d++)
        {
          Array[a][b][c][d] =
              malloc(r * sizeof *Array[0][0][0][0]);
        }
      }
    }
  }

  return Array;
}

/* end of c11_019.c */
