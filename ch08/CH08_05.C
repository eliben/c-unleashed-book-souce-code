/* Listing 8.5 */

  FOO f = {0};
  FOO *p;

  p = malloc(n * sizeof *p);
  if(p != NULL)
  {
    for(i = 0; i < N; i++)
      p[i] = f;
  }
