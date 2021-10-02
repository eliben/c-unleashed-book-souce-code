/**************************************************************************
**  SP_INS_HEADER_ELEMENT                                                **
**                                                                       **
**    Adds a header element to an existing defined dimension of a sparse **
**  matrix.                                                              **
**                                                                       **
**  INPUT:                                                               **
**    sp -- The sparse matrix to which the header element is to be       **
**          inserted.                                                    **
**    dim -- The dimension of the header list being added                **
**    seq -- The sequence number of this element within the header       **
**    detail -- The user defined detail which is to be associated with   **
**              this header element.                                     **
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

SPARSE_MATRIX *sp_ins_header_element(SPARSE_MATRIX *sp, 
                                     int dim, 
                                     int seq, 
                                     SP_HEADER_DATA *detail)
/* SPARSE_MATRIX *sp       The sparse matrix in which to add the header 
                             dimension */
/* int dim                 The dimension number */
/*     seq                 The sequence number of this element within the 
                             header */
/* SP_HEADER_DATA *detail  The user defined detail to be inserted in 
                             the header */
{
  SP_HDR_ELEMENT *new_header, *prev_header;

  /* If the sparse matrix is empty, then a header element cannot be inserted */
  if (sp == (SPARSE_MATRIX *)NULL)
  {
    return((SPARSE_MATRIX *)NULL);
  }

  sp->error_no = SP_NOERR;

  /* Make sure that the dimension specified is legal */
  if ((sp->dimensions < dim) || (dim < (int)1))
  {
    sp->error_no = SP_DIM;
    return(sp);
  }

  /* Make sure that the sequence number is above the minimum dimension range */
  if ((sp_get_range_min(sp, dim) > seq) || (sp->error_no != SP_NOERR))
  {
    sp->error_no = SP_DLOW;
    return(sp);
  }

  /* Make sure that the sequence number is below the minimum dimension range */
  if ((sp_get_range_max(sp, dim) < seq) || (sp->error_no != SP_NOERR))
  {
    sp->error_no = SP_DHIGH;
    return(sp);
  }

  /* Get the header list for the passed dimension */
  new_header = sp_get_header_list(sp, dim);
  if (sp->error_no != SP_NOERR)
  {
    return(sp);
  }

  /* find the position in the doubly linked list for the new header element */
  do
  {
  prev_header = new_header;
  new_header = new_header->next;
  }
  while ((prev_header->sequence < new_header->sequence) && 
         (new_header->sequence < seq));

  /* Check to see if this is the end of the list */
  if (seq > new_header->sequence)
  {
    if (prev_header->sequence < new_header->sequence)
    {
      /* Now the prev_header entry is definitely smaller */
      prev_header = new_header; 
    }
  }

  /* Check to see if the sequence number for this element already exists */
  if ((prev_header->sequence == seq) || (new_header->sequence == seq))
  {
    return(sp);
  }

  /* Try to allocate space for the new header element */
  new_header = (SP_HDR_ELEMENT *)malloc(sizeof(SP_HDR_ELEMENT));
  if (new_header == (SP_HDR_ELEMENT *)NULL)
  {
    sp->error_no = SP_MEMLOW;
    return(sp);
  }


  /* Now update the pointers for the new header element */
  new_header->previous = prev_header;
  new_header->next = prev_header->next;
  new_header->first = (SP_NODE *)NULL;
  new_header->last = (SP_NODE *)NULL;
  new_header->sequence = seq;
  new_header->detail = detail;

  /* Now update the pointers for the previous header element */
  prev_header->next = new_header;

  /* Now update the pointers for the next header element */
  (new_header->next)->previous = new_header;

  return(sp);
}
