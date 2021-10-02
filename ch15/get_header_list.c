/**************************************************************************
**  SP_GET_HEADER_LIST                                                   **
**                                                                       **
**    Retrieves a pointer to the header list associated with the passed  **
**  dimension.                                                           **
**                                                                       **
**  INPUT:                                                               **
**    sp -- The sparse matrix which contains the header list stack       **
**    dim -- The dimension of the header list being requested            **
**                                                                       **
**  OUTPUT:                                                              **
**    SP_HDR_ELEMENT -- A pointer to the header list element             **
**                                                                       **
**  SIDE EFFECTS:                                                        **
**    The error_no field of the sparse matrix can be set to an error if  **
**  an error is encountered.  Whenever an error is encountered, this     **
**  value is set and a NULL pointer is returned.  Thus, if a NULL        **
**  pointer is returned from this function it is important to examine    **
**  the error_no of the associated sparse matrix before assuming that    **
**  the header list is empty.                                            **
**                                                                       **
**  NOTES:                                                               **
**    It is possible for the header list associated with a dimension to  **
**  be empty if no values are in the matrix.  Therefore, the error_no    **
**  value should be checked in the associated sparse matrix whenever a   **
**  NULL pointer is returned.                                            **
**                                                                       **
**************************************************************************/

#include <stdio.h>
#include "sparse.h"

SP_HDR_ELEMENT *sp_get_header_list(SPARSE_MATRIX *sp, int dim)
/* SPARSE_MATRIX *sp  The sparse matrix in which to find the header list */
/* int dim            The dimension number of the header list */
{
  /* If the sparse matrix passed is empty, then there are no header lists to be found */
  if (sp == (SPARSE_MATRIX *)NULL)
    return ((SP_HDR_ELEMENT *)NULL);

  sp->error_no = SP_NOERR;

  /* Make sure that the dimension is not too large */
  if ((sp->dimensions) < dim)
    {
    sp->error_no = SP_DHIGH;
    return ((SP_HDR_ELEMENT *)NULL);
    }

  /* Make sure that the dimension is not too small */
  if (dim < (int)1)
    {
    sp->error_no = SP_DLOW;
    return ((SP_HDR_ELEMENT *)NULL);
    }

  /* Make sure that the header stack has values */
  if (sp->hdr_stack == (SP_HDR_ELEMENT *)NULL)
    {
    sp->error_no = SP_BRHDR;
    return ((SP_HDR_ELEMENT *)NULL);
    }

  /* Return a pointer to the offset in the stack that corresponds
     to the header at this dimension */
  return((SP_HDR_ELEMENT *)(sp->hdr_stack + (dim - 1)));
}
