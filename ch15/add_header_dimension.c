/**************************************************************************
**  SP_ADD_HEADER_DIMENSION                                              **
**                                                                       **
**    Adds a header dimensions to an existing empty sparse matrix.       **
**                                                                       **
**  INPUT:                                                               **
**    sp -- The sparse matrix to which the dimension is to be added      **
**    dim -- The dimension of the header list being added                **
**    dim_min -- The minimum value for elements in this dimension        **
**    dim_max -- The maximum value for elements in this dimension        **
**                                                                       **
**  OUTPUT:                                                              **
**    SPARSE_MATRIX * -- A pointer to the modified sparse matrix         **
**                                                                       **
**  SIDE EFFECTS:                                                        **
**    The error_no field of the sparse matrix can be set to an error if  **
**  an error is encountered.  Whenever an error is encountered, this     **
**  value is set and a NULL pointer is returned.  Thus, if a NULL        **
**  pointer is returned from this function it is important to examine    **
**  the error_no of the associated sparse matrix.                        **
**    If the new dimension is larger than the current dimension size of  **
**  the sparse array, then portions of the array (the stacks) are        **
**  reallocated and the maximum dimension for the array changed.         **
**                                                                       **
**  NOTES:                                                               **
**                                                                       **
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sparse.h"

SPARSE_MATRIX *sp_add_header_dimension(SPARSE_MATRIX *sp, 
                                       int           dim, 
                                       int           dim_min, 
                                       int           dim_max)
/*SPARSE_MATRIX *sp The sparse matrix in which to add the header dimension */
/*int dim           The dimension number */
/*    dim_min       The minimum dimension element value */
/*    dim_max       The maximum dimension element value */
{
  SP_HDR_ELEMENT *new_header, *new_hdr_stack;
  int curr_dim, *new_rng_stack, *new_rng;

  /* If the sparse matrix is empty, then a dimension cannot be added */
  if (sp == (SPARSE_MATRIX *)NULL)
  {
    return((SPARSE_MATRIX *)NULL);
  }

  sp->error_no = SP_NOERR;

  /* Make sure that the max is larger than the min */
  if (dim_max < dim_min)
  {
    sp->error_no = SP_DLOW;
    return(sp);
  }

  /* Make sure that the dimension is not too small */
  if (dim < (int)1)
  {
    sp->error_no = SP_DIM;
    return(sp);
  }

  /* Make sure that the sparse array is empty */
  for (curr_dim = 0; curr_dim < sp->dimensions; curr_dim++)
  {
    new_header = (SP_HDR_ELEMENT *)(sp->hdr_stack + curr_dim);
    if ((new_header->previous != new_header) ||
        (new_header->next     != new_header) ||
        (new_header->first    != (SP_NODE *)NULL) ||
        (new_header->last     != (SP_NODE *)NULL))
    {
      sp->error_no = SP_INSFAIL;
      return(sp);
    }
  }
  
  /* Check to see if this new dimension is within the existing range
     of the dimensions for the matrix */
  if (dim > sp->dimensions)
  {
    /* Attempt to allocate a new header stack of the appropriate size */
    new_hdr_stack = (SP_HDR_ELEMENT *)malloc(sizeof(SP_HDR_ELEMENT)*dim);
    if (new_hdr_stack == (SP_HDR_ELEMENT *)NULL)
    {
      sp->error_no = SP_MEMLOW;
      return(sp);
    }
   
    /* Attempt to allocate a new range stack of the appropriate size */ 
    new_rng_stack = (int *)malloc(sizeof(int)*dim*2);
    if (new_rng_stack == (int *)NULL)
    {
      free(new_hdr_stack);
      sp->error_no = SP_MEMLOW;
      return(sp);
    }

    /* Copy the old header stack values into the new stack */
    (void *)memcpy((void *)new_hdr_stack, (void *)(sp->hdr_stack),
                   sizeof(SP_HDR_ELEMENT) * (sp->dimensions));

    /* Copy the old range stack values into the new stack */
    (void *)memcpy((void *)new_rng_stack, (void *)(sp->hdr_ranges),
                   sizeof(int) * (sp->dimensions) * 2);

    /* Initialize the new elements */
    for (curr_dim = 0; curr_dim < dim; curr_dim++)
    {
      new_header = (SP_HDR_ELEMENT *)(new_hdr_stack + curr_dim);
      new_header->previous = new_header;
      new_header->next = new_header;
      new_header->first = (SP_NODE *)NULL;
      new_header->last = (SP_NODE *)NULL;
      if (curr_dim >= sp->dimensions)
      {
        new_header->sequence = 0;
        new_rng = (int *)(new_rng_stack + (2 * curr_dim));
        *new_rng = 0;
        new_rng ++;
        *new_rng = 0;
      }
      new_header->detail = (SP_HEADER_DATA *)NULL;
    }

    /* insert the new values */
    sp->dimensions = dim;
    free(sp->hdr_ranges);
    free(sp->hdr_stack);
    sp->hdr_ranges = new_rng_stack;
    sp->hdr_stack = new_hdr_stack;
  }

  /* Insert the information for the added dimension */
  new_header = (SP_HDR_ELEMENT *)(sp->hdr_stack + (dim - 1));
  new_header->sequence = dim_min;
  new_rng = (int *)(sp->hdr_ranges + (2 * (dim - 1)));
  *new_rng = dim_min;
  new_rng ++;
  *new_rng = dim_max;

  return(sp);
}
