int GetLoginNameFromUser(char *buffer)
{
  int Result = 0;
  if(fgets(buffer, sizeof buffer, stdin) == NULL)
  {
    Result = -1;
  }
  return Result;
}
