  typedef struct FOO
  {
    unsigned int i: 1;
    unsigned int j: 1;
  } FOO;

  FOO foo;

  unsigned int *p;

  p = &foo.j;
