/* Listing 8.3 */

  static char *buffer = NULL;
  static size_t bufsize = 0;
  while(strlen(s) >= bufsize)
  
    p = realloc(buffer, bufsize * 2);
    if(p != NULL)
    {
      bufsize *= 2;
      buffer = p;
    }
    else
    {
      printf("er...now what?\n");
    }
  }
