void sink(void *p, int i)
{
  if(i > 0)
    sink(p, 0);
}
