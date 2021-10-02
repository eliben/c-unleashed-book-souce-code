/**************************************************************************
**  SP_DEL_HEADER_ELEMENT                                                **
**                                                                       **
**    Deletes a header list element from an existing empty sparse        **
**  matrix.                                                              **
**                                                                       **
**  INPUT:                                                               **
**    sp -- The sparse matrix from which the header list element is to   **
**          be deleted.                                                  **
**    dim -- The dimension of the header list whose element is to be     **
**           deleted.                                                    **
**    seq -- The sequence number of the header element                   **
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
**                                                                       **
**  NOTES:                                                               **
**                                                                       **
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "sparse.h"

SPARSE_MATRIX *sp_del_header_element(SPARSE_MATRIX *sp, int dim, int seq)
/* SPARSE_MATRIX *sp The sparse matrix from which the delete occurs */
/* int dim           The dimension number */
/*     seq           The sequence number of the header element */
{
  SP_HDR_ELEMENT *old_header, *list_top;

  /* If the sparse matrix is empty, then a delete is meaningless */
  if (sp == (SPARSE_MATRIX *)NULL)
  {
    return((SPARSE_MATRIX *)NULL);
  }

  sp->error_no = SP_NOERR;

  /* Make sure that the dimension is not too small */
  if (dim < (int)1)
  {
    sp->error_no = SP_DIM;
    return(sp);
  }

  /* Make sure that the dimension is not too large */
  if (dim > sp->dimensions)
  {
    sp->error_no = SP_DIM;
    return(sp);
  }

  /* Make sure that the sequence number is not too small.  Also check
     to see if an effort is being made to delete the first header
     element as that would be a large problem. */
  if (seq <= sp_get_range_min(sp, dim))
  {
    sp->error_no = SP_DLOW;
    return(sp);
  }

  /* Make sure that the sequence number is not too large */
  if (seq > sp_get_range_max(sp, dim))
  {
    sp->error_no = SP_DHIGH;
    return(sp);
  }

  /* Find the header to be deleted */
  list_top = sp_get_header_list(sp, dim);
  if (sp->error_no != SP_NOERR)
  {
    return(sp);
  }

  /* The first value is definitely not the one to be deleted, so
     search for subsequent values */
  old_header = list_top;
  do
  {
    old_header = old_header->next;
  } 
  while ((old_header->sequence < seq) && (old_header != list_top));

  /* if the old_header doesn't have the sequence number being sought, then
     there is no header element with that value */
  if (old_header->sequence != seq)
  {
    return(sp);
  }

  /* Make sure that there are no nodes associated with the header */
  if ((old_header->first != (SP_NODE *)NULL) ||
      (old_header->last != (SP_NODE *)NULL))
  {
    sp->error_no = SP_NFULL;
    return(sp);
  }

  /* Adjust the pointers for the previous node */
  old_header->previous->next = old_header->next;

  /* Adjust the pointers for the next node */
  old_header->next->previous = old_header->previous;

  /* Free the allocated space */
  free(old_header->detail);
  free(old_header);

  return(sp);
}
