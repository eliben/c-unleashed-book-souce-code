  size_t len = 0;
  char buffer[1024];

  sink(&len, 0);
  if(fgets(buffer, sizeof buffer, stdin) != NULL)
  {
    len = strlen(buffer);
  /* ... */
