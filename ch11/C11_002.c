int GetOrders(ORDER_LINE OrderArray[MAX_ORDER_ENTRIES])
{
  size_t i;
  int GotAnOrder = 1;
  for(i = 0; i < MAX_ORDER_ENTRIES && GotAnOrder; i++)
  {
    GotAnOrder = GetOneOrder(&OrderArray[i]);
  }
  return --i;
}
