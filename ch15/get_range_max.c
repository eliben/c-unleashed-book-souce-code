/**************************************************************************
**  SP_GET_RANGE_MAX                                                     **
**                                                                       **
**    Retrieve the maximum range value associated with a passed          **
**  dimension.                                                           **
**                                                                       **
**  INPUT:                                                               **
**    sp -- The sparse matrix from which the range value is to be        **
**          retrieved.                                                   **
**    dim -- The dimension of the maximum range value                    **
**                                                                       **
**  OUTPUT:                                                              **
**    int -- The maximum range value                                     **
**                                                                       **
**  SIDE EFFECTS:                                                        **
**    The error_no field of the sparse matrix can be set to an error if  **
**  an error is encountered.  Whenever an error is encountered, this     **
**  value is set and a NULL pointer is returned.  Thus, if a NULL        **
**  pointer is returned from this function it is important to examine    **
**  the error_no of the associated sparse matrix.                        **
**                                                                       **
**  NOTES:                                                               **
**                                                                       **
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "sparse.h"

int sp_get_range_max(SPARSE_MATRIX *sp, int dim)
/* SPARSE_MATRIX *sp  The sparse matrix from which to retrieve the range 
                        value */
/* int dim            The dimension number */
{
  /* If the sparse matrix is empty, then a range value cannot be retrieved */
  if (sp == (SPARSE_MATRIX *)NULL)
  {
    return(0);
  }

  sp->error_no = SP_NOERR;

  /* Make sure that the dimension specified is legal */
  if ((sp->dimensions < dim) || (dim < (int)1))
  {
    sp->error_no = SP_DIM;
    return(0);
  }

  return(*(sp->hdr_ranges + ((dim - 1) * 2) + 1));
}
