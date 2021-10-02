/**************************************************************************
**  SP_DEL_NODE                                                          **
**                                                                       **
**    Deletes the node associated with the passed tuple.                 **
**                                                                       **
**  INPUT:                                                               **
**    sp -- The sparse matrix which contains the node to be deleted      **
**    tuple -- The tuple of the node to be deleted                       **
**                                                                       **
**  OUTPUT:                                                              **
**    void                                                               **
**                                                                       **
**  SIDE EFFECTS:                                                        **
**                                                                       **
**  NOTES:                                                               **
**                                                                       **
**************************************************************************/

#include <stdio.h>
#include "sparse.h"

SP_NODE *sp_del_node(SPARSE_MATRIX *sp, SP_TUPLE *tuple)
/* SPARSE_MATRIX *sp  The sparse matrix in which to find the node */
/* SP_TUPLE *tuple    The tuple of the node to be deleted */
{
  SP_NODE *del_node, *prev_node, *next_node;
  SP_HDR_ELEMENT *header;
  int curr_dim;

  /* If the sparse matrix passed is empty, then there are no nodes to be found */
  if (sp == (SPARSE_MATRIX *)NULL)
    return;

  /* Make sure that the number of dimensions in the tuple matches the
     number of dimensions in the sparse matrix */
  if (sp->dimensions != tuple->dimensions)
  {
    sp->error_no = SP_BADDIM;
    return;
  }

  /* retrieve the node to be deleted */
  del_node = sp_retrieve_node(sp, tuple);
  if ((del_node == (SP_NODE *)NULL) || (sp->error_no != SP_NOERR))
  {
    return;
  }

  /* rearrange the pointers around this node in the existing lists in each dimension */
  for (curr_dim = 0; curr_dim < sp->dimensions; curr_dim++)
  {
    next_node = *((SP_NODE **)(del_node->dimension_stack + (2 * curr_dim)));
    prev_node = *((SP_NODE **)(del_node->dimension_stack +
                               (2 * curr_dim) + 1));

    /* Set the next pointer on the prev_node to next_node */
    *((SP_NODE **)(prev_node->dimension_stack + (2 * curr_dim))) =
      next_node;

    /* Set the previous pointer on the next_node to prev_node */
    *((SP_NODE **)(next_node->dimension_stack +
                   (2 * curr_dim) + 1)) = prev_node;

    /* The pointers in the header may be effected */
    header = *(SP_HDR_ELEMENT **)(del_node->hdr_stack + curr_dim);

    /* If this is the last node in the list then the first and last pointer 
       in the header should be set to NULL */
    if ((next_node == prev_node) && (next_node = del_node))
    {

      header->first = (SP_NODE *)NULL;
      header->last = (SP_NODE *)NULL;
    }
    else
    {
      /* Adjust the first pointer if del_node was the head of the list */
      if (header->first == del_node)
      {
        header->first = next_node;
      }

      /* Adjust the last pointer if del_node was the tail of the list */
      if (header->last == del_node)
      {
        header->last = prev_node;
      }
    }
  }

  /* Now the node is no longer in any of the linked lists, so it can be freed */
  free(del_node->dimension_stack);
  free(del_node->hdr_stack);
  free(del_node);

  return;
}
