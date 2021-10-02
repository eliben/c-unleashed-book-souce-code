/* Listing 8.2 */
char *ReadTextFile(FILE *fp, int *Error)
{
  size_t size = 0;
  size_t len;
  char *p = NULL;
  char *q;
  char buffer[128];

  *Error = 0;

  while(fgets(buffer, sizeof buffer, fp))
  {
    len = strlen(buffer);
    q = realloc(p, size + len);
    if(q != NULL)
    {
      p = q;
      strcpy(p + size, buffer);
      size += len;
    }
    else
    {
      *Error = 1;
    }
  }
  return p;
}
