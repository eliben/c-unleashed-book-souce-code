int CalcTotal(int *arr, size_t numelems)
{
  size_t i;
  int total = 0;

  for(i = 0; i <= numelems; i++)
  {
    total += *arr++;
  }
  return total;
}
