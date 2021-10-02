/**************************************************************************
**  SP_ADD_TUPLE                                                         **
**                                                                       **
**    Adds a sequence number into a tuple.                               **
**                                                                       **
**  INPUT:                                                               **
**    tuple -- The tuple which is to get the new sequence number         **
**    dim -- The dimension getting a new sequence value                  **
**    seq -- The sequence number being added                             **
**                                                                       **
**  OUTPUT:                                                              **
**    SP_TUPLE * -- A pointer to the modified tuple                      **
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

SP_TUPLE *sp_add_tuple(SP_TUPLE *tuple, int dim, int seq)
/* SP_TUPLE *tuple  The tuple which is to get the new sequence number */
/* int dim             The dimension number */
/*     seq             The sequence number being added */
{
  int *new_seq;

  /* If the tuple is empty, then no sequence can be added */
  if (tuple == (SP_TUPLE *)NULL)
  {
    return((SP_TUPLE *)NULL);
  }

  /* If the passed dimension is outside the range of the tuple dimension, then
     the insert fails */
  if ((tuple->dimensions < dim) || (dim < (int)1))
  {
    return((SP_TUPLE *)NULL);
  }

  new_seq = (int *)(tuple->seq + ((dim - 1)));
  *new_seq = seq;
  return(tuple);
}
