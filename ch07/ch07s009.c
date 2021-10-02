/* Care! Several bugs here! */
char *strdup(char *s)
{
  return strcpy(malloc(strlen(s)), s);
}
