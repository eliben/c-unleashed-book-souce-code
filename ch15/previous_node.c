/**************************************************************************
**  SP_PREVIOUS_NODE                                                     **
**                                                                       **
**    Returns the previous node in a given dimension.                    **
**                                                                       **
**  INPUT:                                                               **
**    sp -- The sparse matrix which contains the nodes                   **
**    dim -- The dimension of the node being requested                   **
**    node -- The current node                                           **
**                                                                       **
**  OUTPUT:                                                              **
**    SP_NODE -- A pointer to the node                                   **
**                                                                       **
**  SIDE EFFECTS:                                                        **
**    The error_no field of the sparse matrix can be set to an error if  **
**  an error is encountered.  Whenever an error is encountered, this     **
**  value is set and a NULL pointer is returned.  Thus, if a NULL        **
**  pointer is returned from this function it is important to examine    **
**  the error_no of the associated sparse matrix before assuming that    **
**  the node list is empty.                                              **
**                                                                       **
**  NOTES:                                                               **
**                                                                       **
**************************************************************************/

#include <stdio.h>
#include "sparse.h"

SP_NODE *sp_previous_node(SPARSE_MATRIX *sp, int dim, SP_NODE *node)
/* SPARSE_MATRIX *sp  The sparse matrix in which to find the node */
/* int dim            The dimension number of the previous node to be 
                        found */
/* SP_NODE *node      The current node */
{
  /* If the sparse matrix passed is empty, then there are no nodes to be found */
  if (sp == (SPARSE_MATRIX *)NULL)
    return ((SP_NODE *)NULL);

  sp->error_no = SP_NOERR;

  /* Make sure the dimension is within range */
  if (dim > sp->dimensions)
  {
    sp->error_no = SP_DIM;
    return((SP_NODE *)NULL);
  }

  /* The stack has next, then previous node pointers for each dimension in that 
     order, so there are 2 pointers for each dimension.  The previous is the second
     of the two, so the offset is (2 * dim) + 1 */
  return(*((SP_NODE **)(node->dimension_stack + (2 * (dim - 1))) + 1));
}
