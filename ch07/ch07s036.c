  char *p;

  p = malloc(len);

  foomem(p);

  /* ... */
}

void foomem(char *p)
{
  assert(p != NULL);
  dosomethingwith(p);
}
