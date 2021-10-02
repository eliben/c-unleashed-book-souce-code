  while(p->Next != NULL)
  {
    free(p);
    free(p->Data);
    p = p->Next;
  }
