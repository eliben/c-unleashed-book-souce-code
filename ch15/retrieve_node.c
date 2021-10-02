/**************************************************************************
**  SP_RETRIEVE_NODE                                                     **
**                                                                       **
**    Retrieves a node from a sparse matrix.                             **
**                                                                       **
**  INPUT:                                                               **
**    sp -- The sparse matrix into which the node is to be inserted.     **
**    tuple -- The tuple used to define the location of the node.        **
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
#include <string.h>
#include "sparse.h"

SP_NODE *sp_retrieve_node(SPARSE_MATRIX *sp, SP_TUPLE *tuple)
/* SPARSE_MATRIX *sp  The sparse matrix from which the node is to be 
                        retrieved */
/* SP_TUPLE *tuple    The tuple used to define the location of the node */
{
  SP_NODE *new_list, *node_pos;
  SP_HDR_ELEMENT **new_hdr_list, *hdr_pos, *hdr_list;
  int curr_dim, *curr_seq, node_found=0;

  /* If the sparse matrix is empty, then a node cannot be retrieved */
  if (sp == (SPARSE_MATRIX *)NULL)
  {
    return((SP_NODE *)NULL);
  }

  sp->error_no = SP_NOERR;

  /* If the tuple is empty, then no node can be retrieved */
  if (tuple == (SP_TUPLE *)NULL)
  {
    sp->error_no = SP_BADDIM;
    return((SP_NODE *)NULL);
  }

  /* Make sure that the number of dimensions in the tuple matches the
     number of dimensions in the sparse matrix */
  if (sp->dimensions != tuple->dimensions)
  {
    sp->error_no = SP_BADDIM;
    return((SP_NODE *)NULL);
  }

  /* Make sure that the tuple specified is legal */
  for(curr_dim = 0; curr_dim < sp->dimensions; curr_dim++)
  {
    curr_seq = (int *)(tuple->seq + (curr_dim));
    if ((*curr_seq < sp_get_range_min(sp, curr_dim + 1)) || 
        (sp->error_no != SP_NOERR))
    {
      sp->error_no = SP_DLOW;
      return((SP_NODE *)NULL);
    }
    if ((*curr_seq > sp_get_range_max(sp, curr_dim + 1)) ||
        (sp->error_no != SP_NOERR))
    {
      sp->error_no = SP_DHIGH;
      return((SP_NODE *)NULL);
    }
  }

  /* Get the node list associated with the first dimension */
  new_list = sp_get_node_list(sp, (int)1, *(tuple->seq));
  if ((new_list == (SP_NODE *)NULL) || (sp->error_no != SP_NOERR))
  {
    return((SP_NODE *)NULL);
  }

  /* Build a header stack */
  new_hdr_list = (SP_HDR_ELEMENT **)malloc(sizeof(SP_HDR_ELEMENT *) * sp->dimensions);
  if (new_hdr_list == (SP_HDR_ELEMENT **)NULL)
  {
    sp->error_no = SP_MEMLOW;
    return((SP_NODE *)NULL);
  }

  /* For each dimension in the tuple, seek out the corresponding header list element */
  for (curr_dim = 0; curr_dim < sp->dimensions; curr_dim++)
  {
    /* Get the sequence number associated with the current dimension */
    curr_seq = ((int *)(tuple->seq + (curr_dim)));

    /* Get the header list element */
    hdr_pos = sp_hdr_list_element_get(sp, curr_dim + 1, *curr_seq);
    if ((hdr_pos == (SP_HDR_ELEMENT *)NULL) || (sp->error_no != SP_NOERR))
    {
      free(new_hdr_list);
      return((SP_NODE *)NULL);
    }

    /* Insert this header into the list */
    *((SP_HDR_ELEMENT **)(new_hdr_list + (curr_dim))) = hdr_pos;
  }

  /* Search for a matching tuple in the node list */
  node_pos = new_list;
  node_found = 1;
  if (memcmp((void *)node_pos->hdr_stack, (void *)new_hdr_list,
             (sizeof(SP_HDR_ELEMENT *) * sp->dimensions)))
  {
    do
    {
      /* The first pointer in the dimension_stack points to the next node in the
         first dimension, since that is what we want, we can use the shorthand
         below */
      node_pos = *(node_pos->dimension_stack);
      node_found = !memcmp((void *)node_pos->hdr_stack, (void *)new_hdr_list,
                           (sizeof(SP_HDR_ELEMENT *) * sp->dimensions));
    }
    while ((node_pos != new_list) && (!node_found));
  }

  free(new_hdr_list);
  if (node_found)
  {
    return(node_pos);
  }
  else
  {
    return((SP_NODE *)NULL);
  }
}
