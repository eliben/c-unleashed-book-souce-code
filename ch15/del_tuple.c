/**************************************************************************
**  SP_DEL_TUPLE                                                         **
**                                                                       **
**    Delete a tuple                                                     **
**                                                                       **
**  INPUT:                                                               **
**    tuple -- The tuple which is to be deleted                          **
**                                                                       **
**  OUTPUT:                                                              **
**    void                                                               **
**                                                                       **
**  SIDE EFFECTS:                                                        **
**    None.                                                              **
**                                                                       **
**  NOTES:                                                               **
**                                                                       **
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "sparse.h"

void sp_del_tuple(SP_TUPLE *tuple)
/* SP_TUPLE *tuple The tuple which is to be deleted */
{
  /* If the tuple is empty, then no deletion is necessary */
  if (tuple == (SP_TUPLE *)NULL)
  {
    return;
  }

  /* If there is data in the seq_stack, free it */
  if (tuple->seq != (int *)NULL)
  {
    free(tuple->seq);
  }

  free(tuple);
  return;
}
