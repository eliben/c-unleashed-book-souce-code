/* Listing 8.1 */

char *ReadTextFile(FILE *fp)
{
  size_t size = 0;
  size_t len;
  char *p = NULL;

  char buffer[128];

  while(fgets(buffer, sizeof buffer, fp))
  {
    len = strlen(buffer);
    p = realloc(p, size + len);
    strcpy(p + size, buffer);
    size += len;
  }
  return p;
}
