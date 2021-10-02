#include <stdio.h>
#include "sparse.h"

void print_tuple(SP_TUPLE *tuple)
{
  int curr_dim;

  if (tuple == (SP_TUPLE *)NULL)
  {
    fprintf(stderr, "print_tuple: NULL tuple passed\n");
    return;
  }

  fprintf(stdout, "Printing out a tuple of dimension %d\n", 
                   tuple->dimensions);

  if (tuple->dimensions < 1)
  {
    fprintf(stderr, "print_tuple: Invalid dimension of %d\n",
                    tuple->dimensions);
    return;
  }

  for (curr_dim = 0; curr_dim < tuple->dimensions; curr_dim++)
  {
    fprintf(stdout, "Dimension %d: %d\n", curr_dim + 1,
                    *((int *)(tuple->seq + (curr_dim))));
  }

  return;
}

void tuple_test()
{
  SP_TUPLE my_tuple, *tuple_ptr;

  fprintf(stdout, "\n");
  fprintf(stdout, "Testing the tuple functions\n");
  fprintf(stdout, "---------------------------\n");
  fprintf(stdout, "* Printing an empty tuple\n");
  print_tuple(&my_tuple);
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)2);
  fprintf(stdout, "* Printing the structure passed by reference\n");
  print_tuple(&my_tuple);
  fprintf(stdout, "* Printing the returned pointer\n");
  print_tuple(tuple_ptr);

  fprintf(stdout,"\n");
  fprintf(stdout, "* Inserting 1 into first sequence number\n");
  tuple_ptr = sp_add_tuple(&my_tuple, (int)1, (int)1);
  print_tuple(&my_tuple);
  fprintf(stdout, "* Inserting 2 into second sequence number\n");
  tuple_ptr = sp_add_tuple(&my_tuple, (int)2, (int)2);
  print_tuple(&my_tuple);

  fprintf(stdout, "\n");
  fprintf(stdout, "* Redimensioning the tuple to three dimensions\n");
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)3);
  print_tuple(&my_tuple);
  fprintf(stdout, "* Redimensioning the tuple to one dimension\n");
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)1);
  print_tuple(&my_tuple);
  fprintf(stdout, "* Redimensioning the tuple to three dimensions\n");
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)3);
  print_tuple(&my_tuple);

  return;
}

void print_header(SP_HDR_ELEMENT *header)
{
  if (header == (SP_HDR_ELEMENT *)NULL)
  {
    fprintf(stderr, "print_header: NULL header passed\n");
    return;
  }

  fprintf(stdout, "Printing out header sequence number %d\n",
                  header->sequence);

  if (header->previous == (SP_HDR_ELEMENT *)NULL)
  {
    fprintf(stderr, "print_header: NULL previous pointer\n");
    return;
  }
  else
  {
    fprintf(stdout, "Previous sequence number %d\n", 
                    header->previous->sequence);
  }

  if (header->next == (SP_HDR_ELEMENT *)NULL)
  {
    fprintf(stderr, "print_header: NULL next pointer\n");
    return;
  }
  else
  {
    fprintf(stdout, "Next sequence number %d\n", 
                    header->next->sequence);
  }
  
  if (header->first == (SP_NODE *)NULL)
  {
    fprintf(stdout, "First node is NULL\n");
  }
  else
  {
    fprintf(stdout, "First node is not NULL\n");
  }

  if (header->last == (SP_NODE *)NULL)
  {
    fprintf(stdout, "Last node is NULL\n");
  }
  else
  {
    fprintf(stdout, "Last node is not NULL\n");
  }

  return;
}

void print_hdr_list(SP_HDR_ELEMENT *hdr)
{
  SP_HDR_ELEMENT *curr_hdr = hdr;
  int curr_el = 1;

  if (hdr == (SP_HDR_ELEMENT *)NULL)
  {
    fprintf(stderr, "print_hdr_list: NULL header passed\n");
    return;
  }

  fprintf(stdout, "Printing a header list:\n");

  do
  {
    fprintf(stdout, "Element %d:\n", curr_el++);
    print_header(curr_hdr);
    curr_hdr = curr_hdr->next;
  }
  while (curr_hdr != hdr);

  return;
}

void header_test()
{
  SP_HDR_ELEMENT hdr1, hdr2, hdr3, *header_ptr;

  fprintf(stdout, "\n");
  fprintf(stdout, "Testing the header functions\n");
  fprintf(stdout, "----------------------------\n");
  fprintf(stdout, "* Printing initialized header 1\n");
  hdr1.previous = &hdr1;
  hdr1.next = &hdr1;
  hdr1.first = (SP_NODE *)NULL;
  hdr1.last = (SP_NODE *)NULL;
  hdr1.sequence = 1;
  hdr1.detail = (SP_HEADER_DATA *)NULL;
  print_header(&hdr1);
  fprintf(stdout, "* Printing initialized header 2\n");
  hdr2.previous = &hdr2;
  hdr2.next = &hdr2;
  hdr2.first = (SP_NODE *)NULL;
  hdr2.last = (SP_NODE *)NULL;
  hdr2.sequence = 2;
  hdr2.detail = (SP_HEADER_DATA *)NULL;
  print_header(&hdr2);
  fprintf(stdout, "* Printing initialized header 3\n");
  hdr3.previous = &hdr3;
  hdr3.next = &hdr3;
  hdr3.first = (SP_NODE *)NULL;
  hdr3.last = (SP_NODE *)NULL;
  hdr3.sequence = 3;
  hdr3.detail = (SP_HEADER_DATA *)NULL;
  print_header(&hdr3);

  fprintf(stdout, "\n");
  fprintf(stdout, "* Linking header 1 and header 2\n");
  hdr2.previous = &hdr1;
  hdr2.next = &hdr1;
  hdr1.previous = &hdr2;
  hdr1.next = &hdr2;
  print_header(&hdr1);
  print_header(&hdr2);
  fprintf(stdout, "* Adding header 3\n");
  hdr3.previous = &hdr2;
  hdr3.next = hdr2.next;
  hdr2.next = &hdr3;
  print_header(&hdr1);
  print_header(&hdr2);
  print_header(&hdr3);

  return;
}

void print_sparse(SPARSE_MATRIX *sp)
{
  int curr_dim;

  if (sp == (SPARSE_MATRIX *)NULL)
  {
    fprintf(stderr, "print_sparse: NULL sparse matrix passed\n");
    return;
  }
  
  fprintf(stdout, "Printing out a sparse_matrix of dimension %d\n", 
                   sp->dimensions);

  if (sp->dimensions < 1)
  {
    fprintf(stderr, "print_sparse: Invalid dimension %d\n",
                   sp->dimensions);
    return;
  }

  for (curr_dim = 0; curr_dim < sp->dimensions; curr_dim++)
  {
    fprintf(stdout, "Dimension %d:\n", curr_dim+1);
    fprintf(stdout, "\tHeader element sequence number: %d\n",
                    ((SP_HDR_ELEMENT *)(sp->hdr_stack + curr_dim))->sequence);
    fprintf(stdout, "\tError number: %d\n", sp->error_no);
    fprintf(stdout, "\tMin for the range: %d\n", 
                    *((int *)(sp->hdr_ranges + (2 * curr_dim))));
    fprintf(stdout, "\tMax for the range: %d\n", 
                    *((int *)(sp->hdr_ranges + (2 * curr_dim) + 1)));
  }

  return;
}

void print_node(SP_NODE *node, int dim)
{
  int curr_dim=0;

  if (node == (SP_NODE *)NULL)
  {
    fprintf(stderr, "print_node: NULL node passed\n");
    return;
  }
 
  if (dim < (int)1)
  {
    fprintf(stderr, "print_node: Invalid dimension of %d passed\n", dim);
    return;
  }

  fprintf(stdout, "Printing out node with value %d:\n", node->value);

  for (curr_dim = 0; curr_dim < dim; curr_dim++)
  {
    fprintf(stdout, "Dimension %d:\n", curr_dim+1);
    fprintf(stdout, "\tHeader Sequence Number: %d\n",
                    (*(node->hdr_stack + curr_dim))->sequence);
    fprintf(stdout, "\tNext Node Value: %d\n",
                    (*(node->dimension_stack + (2 * curr_dim)))->value);
    fprintf(stdout, "\tPrevious Node Value: %d\n",
                    (*(node->dimension_stack + (2 * curr_dim)))->value);
  }

  return;
}

void sparse_test()
{
  SPARSE_MATRIX sp, *sp_ptr;
  SP_HDR_ELEMENT *hdr;
  SP_TUPLE my_tuple, *tuple_ptr;
  SP_NODE *node;

  fprintf(stdout, "\n");
  fprintf(stdout, "Testing the sparse functions\n");
  fprintf(stdout, "----------------------------\n");
  fprintf(stdout, "* Printing an empty matrix\n");
  sp.dimensions = 0;
  sp.hdr_ranges = (int *)NULL;
  sp.error_no = SP_NOERR;
  sp.hdr_stack = (SP_HDR_ELEMENT *)NULL;
  print_sparse(&sp);

  fprintf(stdout, "\n");
  fprintf(stdout, "* Adding the first dimension to the matrix\n");
  sp_ptr = sp_add_header_dimension(&sp, 1, 0, 10);
  print_sparse(&sp);
  fprintf(stdout, "* Adding the second dimension to the matrix\n");
  sp_ptr = sp_add_header_dimension(&sp, 2, 0, 10);
  print_sparse(&sp);
  fprintf(stdout, "* Insert a header element in dimension 1\n");
  sp_ptr = sp_ins_header_element(&sp, 1, 1, (SP_HEADER_DATA *)NULL);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr,"test_sparse: Unable to insert header element\n");
    print_sparse(&sp);
    return;
  }
  print_sparse(&sp);
  hdr = sp_get_header_list(&sp, 1);
  print_hdr_list(hdr);
  print_sparse(&sp);
  fprintf(stdout, "* Retrieving dimension 1 minimum\n");
  fprintf(stdout, "Minimum Range = %d\n", sp_get_range_min(&sp, (int)1));
  print_sparse(&sp);
  fprintf(stdout, "* Retrieving dimension 1 maximum\n");
  fprintf(stdout, "Maximum Range = %d\n", sp_get_range_max(&sp, (int)1));
  print_sparse(&sp);
  fprintf(stdout, "* Retrieving dimension 2 minimum\n");
  fprintf(stdout, "Minimum Range = %d\n", sp_get_range_min(&sp, (int)2));
  print_sparse(&sp);
  fprintf(stdout, "* Retrieving dimension 2 maximum\n");
  fprintf(stdout, "Maximum Range = %d\n", sp_get_range_max(&sp, (int)2));
  print_sparse(&sp);

  fprintf(stdout, "\n");
  fprintf(stdout, "* Fill out the matrix to 6x6\n");
  sp_ptr = sp_ins_header_element(&sp, 1, 2, (SP_HEADER_DATA *)NULL);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr,"test_sparse: Unable to fully allocate array\n");
    print_sparse(&sp);
    return;
  }
  sp_ptr = sp_ins_header_element(&sp, 1, 3, (SP_HEADER_DATA *)NULL);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr,"test_sparse: Unable to fully allocate array\n");
    print_sparse(&sp);
    return;
  }
  sp_ptr = sp_ins_header_element(&sp, 1, 4, (SP_HEADER_DATA *)NULL);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr,"test_sparse: Unable to fully allocate array\n");
    print_sparse(&sp);
    return;
  }
  sp_ptr = sp_ins_header_element(&sp, 1, 5, (SP_HEADER_DATA *)NULL);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr,"test_sparse: Unable to fully allocate array\n");
    print_sparse(&sp);
    return;
  }
  sp_ptr = sp_ins_header_element(&sp, 2, 1, (SP_HEADER_DATA *)NULL);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr,"test_sparse: Unable to fully allocate array\n");
    print_sparse(&sp);
    return;
  }
  sp_ptr = sp_ins_header_element(&sp, 2, 2, (SP_HEADER_DATA *)NULL);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr,"test_sparse: Unable to fully allocate array\n");
    print_sparse(&sp);
    return;
  }
  sp_ptr = sp_ins_header_element(&sp, 2, 3, (SP_HEADER_DATA *)NULL);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr,"test_sparse: Unable to fully allocate array\n");
    print_sparse(&sp);
    return;
  }
  sp_ptr = sp_ins_header_element(&sp, 2, 4, (SP_HEADER_DATA *)NULL);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr,"test_sparse: Unable to fully allocate array\n");
    print_sparse(&sp);
    return;
  }
  sp_ptr = sp_ins_header_element(&sp, 2, 5, (SP_HEADER_DATA *)NULL);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr,"test_sparse: Unable to fully allocate array\n");
    print_sparse(&sp);
    return;
  }
  print_sparse(&sp);
  hdr = sp_get_header_list(&sp, 1);
  print_hdr_list(hdr);
  hdr = sp_get_header_list(&sp, 2);
  print_hdr_list(hdr);

  fprintf(stdout, "\n");
  fprintf(stdout, "* Attempt to delete the the first element in dimension 1\n");
  sp_ptr = sp_del_header_element(&sp, 1, 0);
  print_sparse(&sp);
  sp.error_no = SP_NOERR;
  fprintf(stdout, "* Attempt to delete the the second element in dimension 1\n");
  sp_ptr = sp_del_header_element(&sp, 1, 1);
  print_sparse(&sp);
  hdr = sp_get_header_list(&sp, 1);
  print_hdr_list(hdr);
  fprintf(stdout, "* Put it back in\n");
  sp_ptr = sp_ins_header_element(&sp, 1, 1, (SP_HEADER_DATA *)NULL);
  print_sparse(&sp);
  hdr = sp_get_header_list(&sp, 1);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr,"test_sparse: Unable to reinsert header element\n");
    print_sparse(&sp);
    return;
  }
  print_hdr_list(hdr);

  fprintf(stdout, "\n");
  fprintf(stdout, "* Inserting a node at (1,1)\n");
  my_tuple.seq = (int *)NULL;
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)1, (int)1);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)2, (int)1);
  print_tuple(&my_tuple);
  sp_ptr = sp_ins_node(&sp, &my_tuple, (int)11);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr, "test_sparse: Failure to insert\n");
    print_sparse(&sp);
    return;
  }
  print_sparse(&sp);
  hdr = sp_get_header_list(&sp, 1);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr, "test_sparse: Unable to retrieve header for first dimension\n");
    print_sparse(&sp);
    return;
  }
  print_hdr_list(hdr);
  fprintf(stdout, "* Retrieving a node at (1,1)\n");
  node = sp_retrieve_node(&sp, &my_tuple);
  if ((sp.error_no != SP_NOERR) || (node == (SP_NODE *)NULL))
  {
    fprintf(stderr, "test_sparse: Unable to retrieve node\n");
    print_sparse(&sp);
    return;
  }
  print_node(node, (int)2);
  fprintf(stdout, "* Inserting a node at (0,0)\n");
  my_tuple.seq = (int *)NULL;
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)1, (int)0);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)2, (int)0);
  print_tuple(&my_tuple);
  sp_ptr = sp_ins_node(&sp, &my_tuple, (int)0);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr, "test_sparse: Failure to insert\n");
    print_sparse(&sp);
    return;
  }
  fprintf(stdout, "* Retrieving a node at (0,0)\n");
  node = sp_retrieve_node(&sp, &my_tuple);
  if ((sp.error_no != SP_NOERR) || (node == (SP_NODE *)NULL))
  {
    fprintf(stderr, "test_sparse: Unable to retrieve node\n");
    print_sparse(&sp);
    return;
  }
  print_node(node, (int)2);
  fprintf(stdout, "* Filling out matrix to 6x6\n");
  my_tuple.seq = (int *)NULL;
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)1, (int)0);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)2, (int)1);
  print_tuple(&my_tuple);
  sp_ptr = sp_ins_node(&sp, &my_tuple, (int)1);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr, "test_sparse: Failure to insert\n");
    print_sparse(&sp);
    return;
  }
  my_tuple.seq = (int *)NULL;
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)1, (int)0);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)2, (int)2);
  print_tuple(&my_tuple);
  sp_ptr = sp_ins_node(&sp, &my_tuple, (int)2);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr, "test_sparse: Failure to insert\n");
    print_sparse(&sp);
    return;
  }
  my_tuple.seq = (int *)NULL;
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)1, (int)0);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)2, (int)3);
  print_tuple(&my_tuple);
  sp_ptr = sp_ins_node(&sp, &my_tuple, (int)3);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr, "test_sparse: Failure to insert\n");
    print_sparse(&sp);
    return;
  }
  my_tuple.seq = (int *)NULL;
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)1, (int)0);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)2, (int)4);
  print_tuple(&my_tuple);
  sp_ptr = sp_ins_node(&sp, &my_tuple, (int)4);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr, "test_sparse: Failure to insert\n");
    print_sparse(&sp);
    return;
  }
  my_tuple.seq = (int *)NULL;
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)1, (int)0);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)2, (int)5);
  print_tuple(&my_tuple);
  sp_ptr = sp_ins_node(&sp, &my_tuple, (int)5);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr, "test_sparse: Failure to insert\n");
    print_sparse(&sp);
    return;
  }
  my_tuple.seq = (int *)NULL;
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)1, (int)1);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)2, (int)0);
  print_tuple(&my_tuple);
  sp_ptr = sp_ins_node(&sp, &my_tuple, (int)10);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr, "test_sparse: Failure to insert\n");
    print_sparse(&sp);
    return;
  }
  my_tuple.seq = (int *)NULL;
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)1, (int)1);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)2, (int)2);
  print_tuple(&my_tuple);
  sp_ptr = sp_ins_node(&sp, &my_tuple, (int)12);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr, "test_sparse: Failure to insert\n");
    print_sparse(&sp);
    return;
  }
  my_tuple.seq = (int *)NULL;
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)1, (int)1);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)2, (int)3);
  print_tuple(&my_tuple);
  sp_ptr = sp_ins_node(&sp, &my_tuple, (int)13);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr, "test_sparse: Failure to insert\n");
    print_sparse(&sp);
    return;
  }
  my_tuple.seq = (int *)NULL;
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)1, (int)1);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)2, (int)4);
  print_tuple(&my_tuple);
  sp_ptr = sp_ins_node(&sp, &my_tuple, (int)14);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr, "test_sparse: Failure to insert\n");
    print_sparse(&sp);
    return;
  }
  my_tuple.seq = (int *)NULL;
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)1, (int)1);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)2, (int)5);
  print_tuple(&my_tuple);
  sp_ptr = sp_ins_node(&sp, &my_tuple, (int)15);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr, "test_sparse: Failure to insert\n");
    print_sparse(&sp);
    return;
  }
  my_tuple.seq = (int *)NULL;
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)1, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)2, (int)0);
  print_tuple(&my_tuple);
  sp_ptr = sp_ins_node(&sp, &my_tuple, (int)20);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr, "test_sparse: Failure to insert\n");
    print_sparse(&sp);
    return;
  }
  my_tuple.seq = (int *)NULL;
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)1, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)2, (int)1);
  print_tuple(&my_tuple);
  sp_ptr = sp_ins_node(&sp, &my_tuple, (int)21);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr, "test_sparse: Failure to insert\n");
    print_sparse(&sp);
    return;
  }
  my_tuple.seq = (int *)NULL;
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)1, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)2, (int)2);
  print_tuple(&my_tuple);
  sp_ptr = sp_ins_node(&sp, &my_tuple, (int)22);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr, "test_sparse: Failure to insert\n");
    print_sparse(&sp);
    return;
  }
  my_tuple.seq = (int *)NULL;
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)1, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)2, (int)3);
  print_tuple(&my_tuple);
  sp_ptr = sp_ins_node(&sp, &my_tuple, (int)23);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr, "test_sparse: Failure to insert\n");
    print_sparse(&sp);
    return;
  }
  my_tuple.seq = (int *)NULL;
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)1, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)2, (int)4);
  print_tuple(&my_tuple);
  sp_ptr = sp_ins_node(&sp, &my_tuple, (int)24);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr, "test_sparse: Failure to insert\n");
    print_sparse(&sp);
    return;
  }
  my_tuple.seq = (int *)NULL;
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)1, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)2, (int)5);
  print_tuple(&my_tuple);
  sp_ptr = sp_ins_node(&sp, &my_tuple, (int)25);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr, "test_sparse: Failure to insert\n");
    print_sparse(&sp);
    return;
  }
  my_tuple.seq = (int *)NULL;
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)1, (int)3);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)2, (int)0);
  print_tuple(&my_tuple);
  sp_ptr = sp_ins_node(&sp, &my_tuple, (int)30);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr, "test_sparse: Failure to insert\n");
    print_sparse(&sp);
    return;
  }
  my_tuple.seq = (int *)NULL;
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)1, (int)3);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)2, (int)1);
  print_tuple(&my_tuple);
  sp_ptr = sp_ins_node(&sp, &my_tuple, (int)31);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr, "test_sparse: Failure to insert\n");
    print_sparse(&sp);
    return;
  }
  my_tuple.seq = (int *)NULL;
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)1, (int)3);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)2, (int)2);
  print_tuple(&my_tuple);
  sp_ptr = sp_ins_node(&sp, &my_tuple, (int)32);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr, "test_sparse: Failure to insert\n");
    print_sparse(&sp);
    return;
  }
  my_tuple.seq = (int *)NULL;
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)1, (int)3);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)2, (int)3);
  print_tuple(&my_tuple);
  sp_ptr = sp_ins_node(&sp, &my_tuple, (int)33);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr, "test_sparse: Failure to insert\n");
    print_sparse(&sp);
    return;
  }
  my_tuple.seq = (int *)NULL;
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)1, (int)3);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)2, (int)4);
  print_tuple(&my_tuple);
  sp_ptr = sp_ins_node(&sp, &my_tuple, (int)34);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr, "test_sparse: Failure to insert\n");
    print_sparse(&sp);
    return;
  }
  my_tuple.seq = (int *)NULL;
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)1, (int)3);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)2, (int)5);
  print_tuple(&my_tuple);
  sp_ptr = sp_ins_node(&sp, &my_tuple, (int)35);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr, "test_sparse: Failure to insert\n");
    print_sparse(&sp);
    return;
  }
  my_tuple.seq = (int *)NULL;
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)1, (int)4);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)2, (int)0);
  print_tuple(&my_tuple);
  sp_ptr = sp_ins_node(&sp, &my_tuple, (int)40);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr, "test_sparse: Failure to insert\n");
    print_sparse(&sp);
    return;
  }
  my_tuple.seq = (int *)NULL;
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)1, (int)4);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)2, (int)1);
  print_tuple(&my_tuple);
  sp_ptr = sp_ins_node(&sp, &my_tuple, (int)41);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr, "test_sparse: Failure to insert\n");
    print_sparse(&sp);
    return;
  }
  my_tuple.seq = (int *)NULL;
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)1, (int)4);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)2, (int)2);
  print_tuple(&my_tuple);
  sp_ptr = sp_ins_node(&sp, &my_tuple, (int)42);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr, "test_sparse: Failure to insert\n");
    print_sparse(&sp);
    return;
  }
  my_tuple.seq = (int *)NULL;
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)1, (int)4);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)2, (int)3);
  print_tuple(&my_tuple);
  sp_ptr = sp_ins_node(&sp, &my_tuple, (int)43);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr, "test_sparse: Failure to insert\n");
    print_sparse(&sp);
    return;
  }
  my_tuple.seq = (int *)NULL;
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)1, (int)4);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)2, (int)4);
  print_tuple(&my_tuple);
  sp_ptr = sp_ins_node(&sp, &my_tuple, (int)44);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr, "test_sparse: Failure to insert\n");
    print_sparse(&sp);
    return;
  }
  my_tuple.seq = (int *)NULL;
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)1, (int)4);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)2, (int)5);
  print_tuple(&my_tuple);
  sp_ptr = sp_ins_node(&sp, &my_tuple, (int)45);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr, "test_sparse: Failure to insert\n");
    print_sparse(&sp);
    return;
  }
  my_tuple.seq = (int *)NULL;
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)1, (int)5);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)2, (int)0);
  print_tuple(&my_tuple);
  sp_ptr = sp_ins_node(&sp, &my_tuple, (int)50);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr, "test_sparse: Failure to insert\n");
    print_sparse(&sp);
    return;
  }
  my_tuple.seq = (int *)NULL;
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)1, (int)5);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)2, (int)1);
  print_tuple(&my_tuple);
  sp_ptr = sp_ins_node(&sp, &my_tuple, (int)51);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr, "test_sparse: Failure to insert\n");
    print_sparse(&sp);
    return;
  }
  my_tuple.seq = (int *)NULL;
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)1, (int)5);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)2, (int)2);
  print_tuple(&my_tuple);
  sp_ptr = sp_ins_node(&sp, &my_tuple, (int)52);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr, "test_sparse: Failure to insert\n");
    print_sparse(&sp);
    return;
  }
  my_tuple.seq = (int *)NULL;
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)1, (int)5);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)2, (int)3);
  print_tuple(&my_tuple);
  sp_ptr = sp_ins_node(&sp, &my_tuple, (int)53);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr, "test_sparse: Failure to insert\n");
    print_sparse(&sp);
    return;
  }
  my_tuple.seq = (int *)NULL;
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)1, (int)5);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)2, (int)4);
  print_tuple(&my_tuple);
  sp_ptr = sp_ins_node(&sp, &my_tuple, (int)54);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr, "test_sparse: Failure to insert\n");
    print_sparse(&sp);
    return;
  }
  my_tuple.seq = (int *)NULL;
  tuple_ptr = sp_tuple_dim(&my_tuple, (int)2);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)1, (int)5);
  tuple_ptr = sp_add_tuple(&my_tuple, (int)2, (int)5);
  print_tuple(&my_tuple);
  sp_ptr = sp_ins_node(&sp, &my_tuple, (int)55);
  if (sp.error_no != SP_NOERR)
  {
    fprintf(stderr, "test_sparse: Failure to insert\n");
    print_sparse(&sp);
    return;
  }
/*WBF*/

  return;
}

int main()
{
  tuple_test();
  header_test();
  sparse_test();
  return (0);
}
