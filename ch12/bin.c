/* bin - manipulates binary trees.
   Derived from libavl for manipulation of binary trees.
   Copyright (C) 1998-2000 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.

   The author may be contacted at <pfaffben@msu.edu> on the Internet,
   or as Ben Pfaff, 12167 Airport Rd, DeWitt MI 48820, USA through
   more mundane means. */

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* A binary tree node. */
struct bin_node {
  int data;
  struct bin_node *left;
  struct bin_node *right;
};

/* A binary tree. */
struct bin_tree {
  struct bin_node *root;
  int count;
};

/* Creates and returns a new binary tree.  Returns a null pointer if a
   memory allocation error occurs. */
struct bin_tree *bin_create(void)
{
  struct bin_tree *tree = malloc(sizeof *tree);
  if (tree == NULL)
    return NULL;
  tree->root = NULL;
  tree->count = 0;
  return tree;
}

/* Searches TREE for matching ITEM.  Returns 1 if found, 0
   otherwise. */
int bin_search(const struct bin_tree *tree, int item)
{
  const struct bin_node *node;

  assert(tree != NULL);
  node = tree->root;
  for (;;) {
    if (node == NULL)
      return 0;
    else if (item == node->data)
      return 1;
    else if (item > node->data)
      node = node->right;
    else
      node = node->left;
  }
}

/* Inserts ITEM into TREE.  Returns 1 if the item was inserted, 2 if
   an identical item already existed in TREE, or 0 if a memory
   allocation error occurred. */
int bin_insert(struct bin_tree *tree, int item)
{
  struct bin_node *node, **new;

  assert(tree != NULL);
  new = &tree->root;
  node = tree->root;
  for (;;) {
    if (node == NULL) {
      node = *new = malloc(sizeof *node);
      if (node != NULL) {
	node->data = item;
	node->left = node->right = NULL;
	tree->count++;
	return 1;
      }
      else
	return 0;
    }
    else if (item == node->data)
      return 2;
    else if (item > node->data) {
      new = &node->right;
      node = node->right;
    }
    else {
      new = &node->left;
      node = node->left;
    }
  }
}

/* Deletes any item matching ITEM from TREE.  Returns 1 if such an
   item was deleted, 0 if none was found. */
int bin_delete(struct bin_tree *tree, int item)
{
  struct bin_node **q, *z;

  assert(tree != NULL);
  q = &tree->root;
  z = tree->root;
  for (;;) {
    if (z == NULL)
      return 0;
    else if (item == z->data)
      break;
    else if (item > z->data) {
      q = &z->right;
      z = z->right;
    }
    else {
      q = &z->left;
      z = z->left;
    }
  }

  if (z->right == NULL)
    *q = z->left;
  else {
    struct bin_node *y = z->right;
    if (y->left == NULL) {
      y->left = z->left;
      *q = y;
    }
    else {
      struct bin_node *x = y->left;
      while (x->left != NULL) {
	y = x;
	x = y->left;
      }
      y->left = x->right;
      x->left = z->left;
      x->right = z->right;
      *q = x;
    }
  }

  tree->count--;
  free(z);
  return 1;
}

/* Helper function for bin_walk().  Recursively prints data from each
   node in tree rooted at NODE in in-order. */
static void walk(const struct bin_node *node)
{
  if (node == NULL)
    return;
  walk(node->left);
  printf("%d ", node->data);
  walk(node->right);
}

/* Prints all the data items in TREE in in-order. */
void bin_walk(const struct bin_tree *tree)
{
  assert(tree != NULL);
  walk(tree->root);
}

/* Prints all the data items in TREE in in-order, using an iterative
   algorithm. */
void bin_traverse(const struct bin_tree *tree)
{
  struct bin_node *stack[32];
  int count;

  struct bin_node *node;

  assert(tree != NULL);
  count = 0;
  node = tree->root;
  for (;;) {
    while (node != NULL) {
      stack[count++] = node;
      node = node->left;
    }
    if (count == 0)
      return;
    node = stack[--count];
    printf("%d ", node->data);
    node = node->right;
  }
}

/* Destroys tree rooted at NODE. */
static void destroy(struct bin_node *node)
{
  if (node == NULL)
    return;
  destroy(node->left);
  destroy(node->right);
  free(node);
}

/* Destroys TREE. */
void bin_destroy(struct bin_tree *tree)
{
  assert(tree != NULL);
  destroy(tree->root);
  free(tree);
}

/* Returns the number of data items in TREE. */
int bin_count(const struct bin_tree *tree)
{
  assert(tree != NULL);
  return tree->count;
}

/* Print the structure of node NODE of an binary tree, which is LEVEL
   levels from the top of the tree.  Uses different delimiters to
   visually distinguish levels. */
void print_structure(struct bin_node *node, int level)
{
  assert(level <= 100);

  if (node == NULL) {
    printf(" nil");
    return;
  }
  printf(" %c%d", "([{`/"[level % 5], node->data);
  if (node->left || node->right) {
    print_structure(node->left, level + 1);
    if (node->right)
      print_structure(node->right, level + 1);
  }
  printf("%c", ")]}'\\"[level % 5]);
}

/* Examine NODE in a binary tree.  *COUNT is increased by the number
   of nodes in the tree, including the current one.  If the node is
   the root of the tree, PARENT should be INT_MIN, otherwise it should
   be the parent node value.  If this node is a left child of its
   parent node, DIR should be -1.  Otherwise, if it is not the root,
   it is a right child and DIR must be +1.  Sets *OKAY to 0 if an
   error is found. */
void
recurse_tree(struct bin_node *node, int *count, int parent, int dir, int *okay)
{
  if (node == NULL)
    return;

  assert(count != NULL);
  (*count)++;

  recurse_tree(node->left, count, node->data, -1, okay);
  recurse_tree(node->right, count, node->data, +1, okay);

  if (parent != INT_MIN) {
    assert(dir == -1 || dir == +1);
    if (dir == -1 && node->data > parent) {
      printf(" Node %d is smaller than its left child %d.\n",
	     parent, node->data);
      *okay = 0;
    }
    else if (dir == +1 && node->data < parent) {
      printf(" Node %d is larger than its right child %d.\n",
	     parent, node->data);
      *okay = 0;
    }
  }
}

/* Checks that TREE's structure is kosher and verifies that the values
   in ARRAY are actually in the tree.  There must be as many elements
   in ARRAY as there are nodes in the tree.  Exits the program if an
   error was encountered. */
void verify_tree(struct bin_tree *tree, int array[])
{
  int count = 0;
  int okay = 1;

  recurse_tree(tree->root, &count, INT_MIN, 0, &okay);
  if (count != tree->count) {
    printf(" Tree has %d nodes, but tree count is %d.\n", count, tree->count);
    okay = 0;
  }

  if (okay) {
    int i;

    for (i = 0; i < tree->count; i++)
      if (!bin_search(tree, array[i])) {
	printf("Tree does not contain expected value %d.\n", array[i]);
	okay = 0;
      }
  }

  if (!okay) {
    printf("Error(s) encountered, aborting execution.\n");
    exit(EXIT_FAILURE);
  }
}

/* Arrange the N elements of ARRAY in random order. */
void shuffle(int *array, int n)
{
  int i;

  for (i = 0; i < n; i++) {
    int j = i + rand() % (n - i);
    int t = array[j];
    array[j] = array[i];
    array[i] = t;
  }
}

/* Choose and display an initial random seed.
   Based on code by Lawrence Kirby <fred@genesis.demon.co.uk>. */
void randomize(int argc, char **argv)
{
  unsigned seed;

  /* Obtain a seed value from the command line if provided, otherwise
     from the computer's realtime clock. */
  if (argc < 2) {
    time_t timeval = time(NULL);
    unsigned char *ptr = (unsigned char *) &timeval;
    size_t i;

    seed = 0;
    for (i = 0; i < sizeof timeval; i++)
      seed = seed * (UCHAR_MAX + 2U) + ptr[i];
  }
  else
    seed = strtoul(argv[1], NULL, 0);

  /* It is more convenient to deal with small seed values when
     debugging by hand. */
  seed %= 32768;

  printf("Seed value = %d\n", seed);
  srand(seed);
}

/* Size of tree used for testing. */
#define TREE_SIZE 16

/* Simple stress test procedure for the binary tree routines.  Does
   the following:

   * Generate a random number seed.  By default this is generated from
   the current time.  You can also pass an integer seed value on the
   command line if you want to test the same case.  The seed value is
   displayed.

   * Create a tree and insert the integers from 0 up to TREE_SIZE - 1
   into it, in random order.  Verifies and displays the tree structure
   after each insertion.
   
   * Removes each integer from the tree, in a different random order.
   Verifies and displays the tree structure after each deletion.

   * Destroys the tree.

   This is pretty good test code if you write some of your own binary
   tree routines.  If you do so you will probably want to modify the
   code below so that it increments the random seed and goes on to new
   test cases automatically. */
int main(int argc, char **argv)
{
  int array[TREE_SIZE];
  struct bin_tree *tree;
  int i;

  randomize(argc, argv);

  for (i = 0; i < TREE_SIZE; i++)
    array[i] = i;
  shuffle(array, TREE_SIZE);

  tree = bin_create();

  for (i = 0; i < TREE_SIZE; i++) {
    int result = bin_insert(tree, array[i]);
    if (result != 1) {
      printf("Error %d inserting element %d, %d, into tree.\n",
	     result, i, array[i]);
      exit(EXIT_FAILURE);
    }

    printf("Inserted %d: ", array[i]);
    /* print_structure(tree->root, 0); */
    bin_walk(tree);
    putchar('\n');

    verify_tree(tree, array);
  }

  shuffle(array, TREE_SIZE);
  for (i = 0; i < TREE_SIZE; i++) {
    if (bin_delete(tree, array[i]) == 0) {
      printf("Error removing element %d, %d, from tree.\n", i, array[i]);
      exit(EXIT_FAILURE);
    }

    printf("Removed %d: ", array[i]);
    /* print_structure(tree->root, 0); */
    bin_traverse(tree);
    putchar('\n');

    verify_tree(tree, array + i + 1);
  }

  bin_destroy(tree);
  printf("Success!\n");

  return EXIT_SUCCESS;
}
