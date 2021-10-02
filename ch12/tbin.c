/* tbin - manipulates threaded binary trees.
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

/* A threaded binary tree node. */
struct tbin_node {
  int data;
  struct tbin_node *left;
  struct tbin_node *right;
  unsigned l_thread:1;
  unsigned r_thread:1;
};

/* A binary tree. */
struct tbin_tree {
  struct tbin_node *root;
  int count;
};

/* Creates and returns a new threaded binary tree.  Returns a null
   pointer if a memory allocation error occurs. */
struct tbin_tree *tbin_create(void)
{
  struct tbin_tree *tree = malloc(sizeof *tree);
  if (tree == NULL)
    return NULL;
  tree->root = NULL;
  tree->count = 0;
  return tree;
}

/* Searches TREE for matching ITEM.  Returns 1 if found, 0
   otherwise. */
int tbin_search(const struct tbin_tree *tree, int item)
{
  const struct tbin_node *node;

  assert(tree != NULL);

  node = tree->root;
  if (node == NULL)
    return 0;

  for (;;) {
    if (item == node->data)
      return 1;
    else if (item > node->data) {
      if (node->r_thread == 0)
	node = node->right;
      else
	return 0;
    }
    else {
      if (node->l_thread == 0)
	node = node->left;
      else
	return 0;
    }
  }
}

/* Allocates a new node in TREE.  Sets the node's data to ITEM and
   initializes the other fields appropriately. */
static struct tbin_node *new_node(struct tbin_tree *tree,
				  int item)
{
  struct tbin_node *node = malloc(sizeof *node);
  if (node == NULL)
    return NULL;

  node->data = item;
  tree->count++;
  return node;
}

/* Inserts ITEM into TREE.  Returns 1 if the item was inserted, 2 if
   an identical item already existed in TREE, or 0 if a memory
   allocation error occurred. */
int tbin_insert(struct tbin_tree *tree, int item)
{
  struct tbin_node *z, *y;

  assert(tree != NULL);

  z = tree->root;
  if (z == NULL) {
    y = tree->root = new_node(tree, item);
    if (y == NULL)
      return 0;

    y->left = y->right = NULL;
    y->l_thread = y->r_thread = 1;
    return 1;
  }

  for (;;) {
    if (item == z->data)
      return 2;
    else if (item > z->data) {
      if (z->r_thread == 1) {
	y = new_node(tree, item);
	if (y == NULL)
	  return 0;

	y->right = z->right;
	y->r_thread = 1;
	z->right = y;
	z->r_thread = 0;
	y->left = z;
	y->l_thread = 1;

	return 1;
      }

      z = z->right;
    }
    else {
      if (z->l_thread == 1) {
	y = new_node(tree, item);
	if (y == NULL)
	  return 0;

	y->left = z->left;
	y->l_thread = 1;
	z->left = y;
	z->l_thread = 0;
	y->right = z;
	y->r_thread = 1;

	return 1;
      }

      z = z->left;
    }
  }
}

/* Deletes any item matching ITEM from TREE.  Returns 1 if such an
   item was deleted, 0 if none was found. */
int tbin_delete(struct tbin_tree *tree, int item)
{
  struct tbin_node **q, *s, *t;

  assert(tree != NULL);

  t = tree->root;
  if (t == NULL)
    return 0;
  s = NULL;

  for (;;) {
    if (item == t->data)
      break;
    else if (item > t->data) {
      if (t->r_thread == 1)
	return 0;
      s = t;
      t = t->right;
    }
    else {
      if (t->l_thread == 1)
	return 0;
      s = t;
      t = t->left;
    }
  }

  if (s == NULL)
    q = &tree->root;
  else if (item > s->data)
    q = &s->right;
  else
    q = &s->left;

  if (t->l_thread == 0 && t->r_thread == 1) {
    struct tbin_node *r = t->left;
    while (r->r_thread == 0)
      r = r->right;
    if (r->right == t)
      r->right = t->right;

    *q = t->left;
  }
  else if (t->r_thread == 1) {
    if (s == NULL)
      *q = NULL;
    else if (item > s->data) {
      s->r_thread = 1;
      *q = t->right;
    }
    else {
      s->l_thread = 1;
      *q = t->left;
    }
  }
  else {
    struct tbin_node *r = t->right;
    if (r->l_thread == 1) {
      r->left = t->left;
      r->l_thread = t->l_thread;
      if (r->l_thread == 0) {
	struct tbin_node *p = r->left;
	while (p->r_thread == 0)
	  p = p->right;
	p->right = r;
      }
      *q = r;
    }
    else {
      struct tbin_node *p = r->left;
      while (p->l_thread == 0) {
	r = p;
	p = r->left;
      }

      t->data = p->data;
      if (p->r_thread == 1) {
	r->l_thread = 1;
	r->left = t;
      }
      else {
	s = r->left = p->right;
	while (s->l_thread == 0)
	  s = s->left;
	s->left = t;
      }
      t = p;
    }
  }

  free(t);
  tree->count--;
  return 1;
}

/* Helper function for tbin_walk().  Recursively prints data from each
   node in tree rooted at NODE in in-order. */
static void walk(const struct tbin_node *node)
{
  if (node->l_thread == 0)
    walk(node->left);
  printf("%d ", node->data);
  if (node->r_thread == 0)
    walk(node->right);
}

/* Prints all the data items in TREE in in-order. */
void tbin_walk(const struct tbin_tree *tree)
{
  assert(tree != NULL);
  if (tree->root != NULL)
    walk(tree->root);
}

/* Returns the next node in in-order in the tree after X, or NULL if X
   is the greatest node in the tree. */
static struct tbin_node *successor(struct tbin_node *x)
{
  struct tbin_node *y;

  assert(x != NULL);

  y = x->right;
  if (x->r_thread == 0)
    while (y->l_thread == 0)
      y = y->left;
  return y;
}

/* Return the node with the least value in the tree rooted at X. */
static struct tbin_node *minimum(struct tbin_node *x)
{
  while (x->l_thread == 0)
    x = x->left;
  return x;
}

/* Returns the previous node in in-order in the tree after X, or NULL if X
   is the greatest node in the tree. */
static struct tbin_node *predecessor(struct tbin_node *x)
{
  struct tbin_node *y;

  assert(x != NULL);

  y = x->left;
  if (x->l_thread == 0)
    while (y->r_thread == 0)
      y = y->right;
  return y;
}

/* Return the node with the greatest value in the tree rooted at X. */
static struct tbin_node *maximum(struct tbin_node *x)
{
  while (x->r_thread == 0)
    x = x->right;
  return x;
}

/* Prints all the data items in TREE in in-order, using an iterative
   algorithm. */
void tbin_traverse(const struct tbin_tree *tree)
{
  struct tbin_node *node;

  assert(tree != NULL);
  if (tree->root != NULL)
    for (node = minimum(tree->root); node != NULL; node = successor(node))
      printf("%d ", node->data);
}

/* Destroys tree rooted at NODE. */
static void destroy(struct tbin_node *node)
{
  if (node->l_thread == 0)
    destroy(node->left);
  if (node->r_thread == 0)
    destroy(node->right);
  free(node);
}

/* Destroys TREE. */
void tbin_destroy(struct tbin_tree *tree)
{
  assert(tree != NULL);
  if (tree->root != NULL)
    destroy(tree->root);
  free(tree);
}

/* Returns the number of data items in TREE. */
int tbin_count(const struct tbin_tree *tree)
{
  assert(tree != NULL);
  return tree->count;
}

/* Print the structure of node NODE of a threaded binary tree, which
   is LEVEL levels from the top of the tree.  Uses different
   delimiters to visually distinguish levels.  */
static void
print_structure(struct tbin_tree *tree, struct tbin_node *node, int level)
{
  char lc[] = "([{<`";
  char rc[] = ")]}>'";

  if (level >= 10) {
    printf("Too deep, giving up.\n");
    return;
  }
  if (node == NULL) {
    printf(" nil");
    return;
  }
  printf(" %c%d", lc[level % 5], (int) node->data);
  fflush(stdout);

  if (node->l_thread == 0)
    print_structure(tree, node->left, level + 1);
  else if (node->left != NULL)
    printf(" :%d", (int) node->left->data);
  else
    printf(" :nil");
  fflush(stdout);

  if (node->r_thread == 0)
    print_structure(tree, node->right, level + 1);
  else if (node->right != NULL)
    printf(" :%d", (int) node->right->data);
  else
    printf(" :nil");
  fflush(stdout);

  printf("%c", rc[level % 5]);
  fflush(stdout);
}

/* Examine NODE in a binary tree.  *COUNT is increased by the number
   of nodes in the tree, including the current one.  If the node is
   the root of the tree, PARENT should be INT_MIN, otherwise it should
   be the parent node value.  If this node is a left child of its
   parent node, DIR should be -1.  Otherwise, if it is not the root,
   it is a right child and DIR must be +1.  Sets *OKAY to 0 if an
   error is found. */
static void
recurse_tree(struct tbin_node *node, int *count,
	     int parent, int dir, int *okay)
{
  assert(count != NULL);
  (*count)++;

  if (node->l_thread == 0)
    recurse_tree(node->left, count, node->data, -1, okay);
  if (node->r_thread == 0)
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

/* Verifies that ITEM exists in the array ARRAY having LENGTH
   elements. */
static int array_contains(int array[], int length, int item)
{
  while (length-- > 0)
    if (array[length] == item)
      return 1;

  return 0;
}

/* Checks that TREE's structure is kosher and verifies that the values
   in ARRAY are actually in the tree.  There must be as many elements
   in ARRAY as there are nodes in the tree.  Exits the program if an
   error was encountered. */
static void verify_tree(struct tbin_tree *tree, int array[])
{
  int count = 0;
  int okay = 1;

  if (tree->root != NULL)
    recurse_tree(tree->root, &count, INT_MIN, 0, &okay);
  if (count != tree->count) {
    printf(" Tree has %d nodes, but tree count is %d.\n", count, tree->count);
    okay = 0;
  }

  if (okay) {
    int i;

    for (i = 0; i < tree->count; i++)
      if (!tbin_search(tree, array[i])) {
	printf("Tree does not contain expected value %d.\n", array[i]);
	okay = 0;
      }
  }

  if (okay && tree->root != NULL) {
    struct tbin_node *t;
    int last = INT_MIN;

    count = 0;
    for (t = minimum(tree->root); t != NULL; t = successor(t)) {
      if (t->data <= last) {
	printf(" Misordered right threads: %d after %d.\n", t->data, last);
	okay = 0;
	break;
      }
      last = t->data;
      if (count++ >= tree->count)
	break;
      if (!array_contains(array, tree->count, t->data)) {
	printf(" Wrong data in tree by right threads.\n");
	okay = 0;
	break;
      }
    }

    if (okay && count != tree->count) {
      printf
	  (" Tree has %d nodes, but count by right threads is %d.\n",
	   count, tree->count);
      okay = 0;
    }
  }

  if (okay && tree->root != NULL) {
    struct tbin_node *t;
    int last = INT_MAX;

    count = 0;
    for (t = maximum(tree->root); t != NULL; t = predecessor(t)) {
      if (t->data >= last) {
	printf(" Misordered left threads: %d after %d.\n", t->data, last);
	okay = 0;
	break;
      }
      last = t->data;
      if (count++ >= tree->count)
	break;
      if (!array_contains(array, tree->count, t->data)) {
	printf(" Wrong data in tree by left threads.\n");
	okay = 0;
	break;
      }
    }

    if (okay && count != tree->count) {
      printf
	  (" Tree has %d nodes, but count by left threads is %d.\n",
	   count, tree->count);
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
  struct tbin_tree *tree;
  int i;

  randomize(argc, argv);

  for (i = 0; i < TREE_SIZE; i++)
    array[i] = i;
  shuffle(array, TREE_SIZE);

  tree = tbin_create();

  for (i = 0; i < TREE_SIZE; i++) {
    int result = tbin_insert(tree, array[i]);
    if (result != 1) {
      printf("Error %d inserting element %d, %d, into tree.\n",
	     result, i, array[i]);
      exit(EXIT_FAILURE);
    }

    printf("Inserted %d: ", array[i]);
    tbin_walk(tree);
    putchar('\n');

    verify_tree(tree, array);
  }

  shuffle(array, TREE_SIZE);
  for (i = 0; i < TREE_SIZE; i++) {
    if (tbin_delete(tree, array[i]) == 0) {
      printf("Error removing element %d, %d, from tree.\n", i, array[i]);
      exit(EXIT_FAILURE);
    }

    printf("Removed %d: ", array[i]);
    tbin_traverse(tree);
    putchar('\n');

    verify_tree(tree, array + i + 1);
  }

  tbin_destroy(tree);
  printf("Success!\n");

  return EXIT_SUCCESS;
}
