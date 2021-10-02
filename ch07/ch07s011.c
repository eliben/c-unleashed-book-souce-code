int CalcDiffSum(int *arr, size_t numelems)
{
  int Sum = 0;
  size_t i;
  for(i = 0; i < numelems; i++)
  {
    Sum += *arr - *(arr + 1);
  }

  return Sum;
}
