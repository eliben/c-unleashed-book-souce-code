/* Listing 8.4 */
  static char *buffer = NULL;
  static size_t bufsize = 0;
  size_t len = strlen(s) + 1;

  while(len > bufsize)
  {
    p = realloc(buffer, bufsize * 2);
    if(p != NULL)
    {
      bufsize *= 2;
      buffer = p;
    }
    else
    {
      p = realloc(buffer, len);
      if(p != NULL)
      {
        bufsize = len;
        buffer = p;
      }
      else
      {
        /* What to do next depends very heavily
         * on the nature of the application.
         */
        printf("What we need here is a design decision!\n");
      }
    }
  }