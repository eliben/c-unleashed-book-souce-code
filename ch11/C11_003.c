int GetOrders(ORDER_LINE OrderArray[MAX_ORDER_ENTRIES],
              size_t MaxEntries)
{
  size_t i;
  int GotAnOrder = 1;
  for(i = 0; i < MaxEntries && GotAnOrder; i++)
  {
    GotAnOrder = GetOneOrder(&OrderArray[i]);
  }
  return --i;
}
