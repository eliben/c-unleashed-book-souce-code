void PopulateLoadingArray(double **Loading,
                         size_t MaxAge,
                         size_t NumGenders)
{
  size_t Age, Gender;
  for(Age = 0; Age < MaxAge; Age++)
  {
    for(Gender = 0; Gender < NumGenders; Gender++)
    {
      Loading[Age][Gender] = CalcLoading(Age, Gender);
    }
  }
}
