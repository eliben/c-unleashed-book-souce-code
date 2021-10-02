/**************************************************************************
**  SP_TUPLE_DIM                                                         **
**                                                                       **
**    Assign a dimension to a tuple.                                     **
**                                                                       **
**  INPUT:                                                               **
**    tuple -- The tuple which is to get a dimension                     **
**    dim -- The dimension of the tuple                                  **
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
#include <string.h>
#include "sparse.h"

SP_TUPLE *sp_tuple_dim(SP_TUPLE *tuple, int dim)
/* SP_TUPLE *tuple  The tuple which is to get the new dimension */
/* int dim          The dimension number */
{
  int *new_stack, curr_dim;

  /* If the tuple is empty, then no dimension is possible */
  if (tuple == (SP_TUPLE *)NULL)
  {
    return((SP_TUPLE *)NULL);
  }

  /* If the number of dimensions is illegal low, then fail */
  if (dim < (int)1)
  {
    return((SP_TUPLE *)NULL);
  }

  /* if the dimension isn't changing, then simply return */
  if (dim == tuple->dimensions)
  {
    return(tuple);
  }

  /* Attempt to allocate space for the new stack */
  new_stack = (int *)malloc(sizeof(int) * dim);
  if (new_stack == (int *)NULL)
  {
    return((SP_TUPLE *)NULL);
  }
  for (curr_dim = 0; curr_dim < dim; curr_dim++)
  {
    *((int *)(new_stack + (curr_dim))) = 0;
  }

  /* If there is currently a stack of sequences, 
     then the tuple is being redimensioned */
  if (tuple->seq != (int *)NULL)
  {
    /* If the old stack is bigger, then only copy the beginning */
    if (tuple->dimensions > dim)
    {
       (void *)memcpy((void *)new_stack, (void *)(tuple->seq),
                       sizeof(int) * dim);
    }
    else
    {
       (void *)memcpy((void *)new_stack, (void *)(tuple->seq),
                       sizeof(int) * (tuple->dimensions));
    }

    /* free the old stack */
    free(tuple->seq);
  }

  /* load in the new stack and dimension value */
  tuple->seq = new_stack;
  tuple->dimensions = dim;

  return(tuple);
}
