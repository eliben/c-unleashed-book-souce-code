/* rb - manipulates red-black trees.
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

/* A node color. */
enum color {
  RB_RED,
  RB_BLACK
};

/* A red-black tree node. */
struct rb_node {
  struct rb_node *link[2];
  int data;
  enum color color;
};

/* A red-black tree. */
struct rb_tree {
  struct rb_node *root;
  int count;
};

/* Creates and returns a new red-black tree.  Returns a null pointer
   if a memory allocation error occurs. */
struct rb_tree *rb_create(void)
{
  struct rb_tree *tree = malloc(sizeof *tree);
  if (tree == NULL)
    return NULL;
  tree->root = NULL;
  tree->count = 0;
  return tree;
}

/* Searches TREE for matching ITEM.  Returns 1 if found, 0
   otherwise. */
int rb_search(const struct rb_tree *tree, int item)
{
  const struct rb_node *node;

  assert(tree != NULL);
  node = tree->root;
  for (;;) {
    if (node == NULL)
      return 0;
    else if (item == node->data)
      return 1;
    else if (item > node->data)
      node = node->link[1];
    else
      node = node->link[0];
  }
}

/* Allocates a new node in TREE at *NODE.  Sets the node's parent to
   PARENT and data to ITEM, and initializes the other fields
   appropriately. */
static struct rb_node *new_node(struct rb_tree *tree,
				int item, enum color color)
{
  struct rb_node *node = malloc(sizeof *node);
  if (node == NULL)
    return NULL;
  node->data = item;
  node->link[0] = node->link[1] = NULL;
  node->color = color;
  tree->count++;
  return node;
}

/* Inserts ITEM into TREE.  Returns 1 if the item was inserted, 2 if
   an identical item already existed in TREE, or 0 if a memory
   allocation error occurred. */
int rb_insert(struct rb_tree *tree, int item)
{
  struct rb_node *ap[48];
  int ad[48];
  int ak;

  struct rb_node *x, *y;

  assert(tree != NULL);
  if (tree->root == NULL) {
    tree->root = new_node(tree, item, RB_BLACK);
    return tree->root != NULL;
  }

  ad[0] = 0;
  ap[0] = (struct rb_node *) &tree->root;
  ak = 1;

  x = tree->root;
  for (;;) {
    int dir;

    if (item == x->data)
      return 2;
    dir = item > x->data;

    ap[ak] = x;
    ad[ak++] = dir;
    y = x->link[dir];
    if (y == NULL) {
      x = x->link[dir] = new_node(tree, item, RB_RED);
      if (x == NULL)
	return 0;
      break;
    }
    x = y;
  }

  while (ap[ak - 1]->color == RB_RED)
    if (ad[ak - 2] == 0) {
      y = ap[ak - 2]->link[1];
      if (y != NULL && y->color == RB_RED) {
	ap[--ak]->color = y->color = RB_BLACK;
	ap[--ak]->color = RB_RED;
      }
      else {
	if (ad[ak - 1] == 1) {
	  x = ap[ak - 1];
	  y = x->link[1];
	  x->link[1] = y->link[0];
	  y->link[0] = x;
	  ap[ak - 2]->link[0] = y;
	}
	else
	  y = ap[ak - 1];

	x = ap[ak - 2];
	x->color = RB_RED;
	y->color = RB_BLACK;

	x->link[0] = y->link[1];
	y->link[1] = x;
	ap[ak - 3]->link[ad[ak - 3]] = y;

	break;
      }
    }
    else {
      y = ap[ak - 2]->link[0];
      if (y != NULL && y->color == RB_RED) {
	ap[--ak]->color = y->color = RB_BLACK;
	ap[--ak]->color = RB_RED;
      }
      else {
	if (ad[ak - 1] == 0) {
	  x = ap[ak - 1];
	  y = x->link[0];
	  x->link[0] = y->link[1];
	  y->link[1] = x;
	  ap[ak - 2]->link[1] = y;
	}
	else
	  y = ap[ak - 1];

	x = ap[ak - 2];
	x->color = RB_RED;
	y->color = RB_BLACK;

	x->link[1] = y->link[0];
	y->link[0] = x;
	ap[ak - 3]->link[ad[ak - 3]] = y;
	break;
      }
    }

  tree->root->color = RB_BLACK;

  return 1;
}

/* Deletes any item matching ITEM from TREE.  Returns 1 if such an
   item was deleted, 0 if none was found. */
int rb_delete(struct rb_tree *tree, int item)
{
  struct rb_node *ap[48];
  int ad[48];
  int k;

  struct rb_node *w, *x, *y, *z;

  assert(tree != NULL);

  ad[0] = 0;
  ap[0] = (struct rb_node *) &tree->root;
  k = 1;

  z = tree->root;
  for (;;) {
    int dir;

    if (z == NULL)
      return 0;

    if (item == z->data)
      break;
    dir = item > z->data;

    ap[k] = z;
    ad[k++] = dir;
    z = z->link[dir];
  }
  tree->count--;

  if (z->link[0] == NULL || z->link[1] == NULL) {
    y = z;

    x = y->link[0];
    if (x == NULL)
      x = y->link[1];
  }
  else {
    ap[k] = z;
    ad[k++] = 1;
    y = z->link[1];

    while (y->link[0] != NULL) {
      ap[k] = y;
      ad[k++] = 0;
      y = y->link[0];
    }

    x = y->link[1];
    z->data = y->data;
  }
  ap[k - 1]->link[ad[k - 1]] = x;

  if (y->color == RB_RED) {
    free(y);
    return 1;
  }

  free(y);

  while (k > 1 && (x == NULL || x->color == RB_BLACK))
    if (ad[k - 1] == 0) {
      w = ap[k - 1]->link[1];

      if (w->color == RB_RED) {
	w->color = RB_BLACK;
	ap[k - 1]->color = RB_RED;

	ap[k - 1]->link[1] = w->link[0];
	w->link[0] = ap[k - 1];
	ap[k - 2]->link[ad[k - 2]] = w;

	ap[k] = ap[k - 1];
	ad[k] = 0;
	ap[k - 1] = w;
	k++;

	w = ap[k - 1]->link[1];
      }

      if ((w->link[0] == NULL || w->link[0]->color == RB_BLACK)
	  && (w->link[1] == NULL || w->link[1]->color == RB_BLACK)) {
	w->color = RB_RED;
	x = ap[k - 1];
	k--;
      }
      else {
	if (w->link[1] == NULL || w->link[1]->color == RB_BLACK) {
	  w->link[0]->color = RB_BLACK;
	  w->color = RB_RED;

	  y = w->link[0];
	  w->link[0] = y->link[1];
	  y->link[1] = w;

	  w = ap[k - 1]->link[1] = y;
	}

	w->color = ap[k - 1]->color;
	ap[k - 1]->color = RB_BLACK;
	w->link[1]->color = RB_BLACK;

	ap[k - 1]->link[1] = w->link[0];
	w->link[0] = ap[k - 1];
	ap[k - 2]->link[ad[k - 2]] = w;

	x = tree->root;
	break;
      }
    }
    else {
      w = ap[k - 1]->link[0];
      if (w->color == RB_RED) {
	w->color = RB_BLACK;
	ap[k - 1]->color = RB_RED;

	ap[k - 1]->link[0] = w->link[1];
	w->link[1] = ap[k - 1];
	ap[k - 2]->link[ad[k - 2]] = w;

	ap[k] = ap[k - 1];
	ad[k] = 1;
	ap[k - 1] = w;
	k++;

	w = ap[k - 1]->link[0];
      }

      if ((w->link[0] == NULL || w->link[0]->color == RB_BLACK)
	  && (w->link[1] == NULL || w->link[1]->color == RB_BLACK)) {
	w->color = RB_RED;
	x = ap[k - 1];
	k--;
      }
      else {
	if (w->link[0] == NULL || w->link[0]->color == RB_BLACK) {
	  w->link[1]->color = RB_BLACK;
	  w->color = RB_RED;

	  y = w->link[1];
	  w->link[1] = y->link[0];
	  y->link[0] = w;

	  w = ap[k - 1]->link[0] = y;
	}

	w->color = ap[k - 1]->color;
	ap[k - 1]->color = RB_BLACK;
	w->link[0]->color = RB_BLACK;

	ap[k - 1]->link[0] = w->link[1];
	w->link[1] = ap[k - 1];
	ap[k - 2]->link[ad[k - 2]] = w;

	x = tree->root;
	break;
      }
    }

  if (x != NULL)
    x->color = RB_BLACK;

  return 1;
}

/* Helper function for rb_walk().  Recursively prints data from each
   node in tree rooted at NODE in in-order. */
static void walk(const struct rb_node *node)
{
  if (node == NULL)
    return;
  walk(node->link[0]);
  printf("%d ", node->data);
  walk(node->link[1]);
}

/* Prints all the data items in TREE in in-order. */
void rb_walk(const struct rb_tree *tree)
{
  assert(tree != NULL);
  walk(tree->root);
}

/* Prints all the data items in TREE in in-order, using an iterative
   algorithm. */
void rb_traverse(const struct rb_tree *tree)
{
  struct rb_node *stack[32];
  int count;

  struct rb_node *node;

  assert(tree != NULL);
  count = 0;
  node = tree->root;
  for (;;) {
    while (node != NULL) {
      stack[count++] = node;
      node = node->link[0];
    }
    if (count == 0)
      return;
    node = stack[--count];
    printf("%d ", node->data);
    node = node->link[1];
  }
}

/* Destroys tree rooted at NODE. */
static void destroy(struct rb_node *node)
{
  if (node == NULL)
    return;
  destroy(node->link[0]);
  destroy(node->link[1]);
  free(node);
}

/* Destroys TREE. */
void rb_destroy(struct rb_tree *tree)
{
  assert(tree != NULL);
  destroy(tree->root);
  free(tree);
}

/* Returns the number of data items in TREE. */
int rb_count(const struct rb_tree *tree)
{
  assert(tree != NULL);
  return tree->count;
}

/* Print the structure of node NODE of a red-black tree, which is
   LEVEL levels from the top of the tree.  Uses different delimiters
   to visually distinguish levels. */
void print_structure(struct rb_node *node, int level)
{
  assert(level <= 100);

  if (node == NULL) {
    printf(" nil");
    return;
  }
  printf(" %c%d%c", "([{`/"[level % 5], node->data,
	 node->color == RB_BLACK ? 'r' : 'b');
  if (node->link[0] || node->link[1]) {
    print_structure(node->link[0], level + 1);
    if (node->link[1])
      print_structure(node->link[1], level + 1);
  }
  printf("%c", ")]}'\\"[level % 5]);
}

/* Examine NODE in a red-black tree.  *COUNT is increased by the
   number of nodes in the tree, including the current one.  If the
   node is the root of the tree, PARENT should be INT_MIN, otherwise
   it should be the parent node value.  If this node is a left child
   of its parent node, DIR should be -1.  Otherwise, if it is not the
   root, it is a right child and DIR must be +1.  Sets *OKAY to 0 if
   an error is found.  Returns the black-height of the tree rooted at
   NODE. */
int
recurse_tree(struct rb_node *node, int *count, int parent, int dir, int *okay)
{
  int lh, rh;

  if (node == NULL)
    return 1;

  assert(count != NULL);
  (*count)++;

  lh = recurse_tree(node->link[0], count, node->data, -1, okay);
  rh = recurse_tree(node->link[1], count, node->data, +1, okay);
  if (node->color != RB_RED && node->color != RB_BLACK) {
    printf(" Node %d is neither red nor black (%d).\n",
	   node->data, (int) node->color);
    *okay = 0;
  }

  if (node->color == RB_RED && node->link[0]
      && node->link[0]->color == RB_RED) {
    printf(" Red node %d has red left child %d\n", node->data,
	   node->link[0]->data);
    *okay = 0;
  }

  if (node->color == RB_RED && node->link[1]
      && node->link[1]->color == RB_RED) {
    printf(" Red node %d has red right child %d\n", node->data,
	   node->link[1]->data);
    *okay = 0;
  }

  if (lh != rh) {
    printf
	(" Node %d has two different black-heights: left bh=%d, "
	 "right bh=%d\n", node->data, lh, rh);
    *okay = 0;
  }

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

  return (node->color == RB_BLACK) + lh;
}

/* Checks that TREE's structure is kosher and verifies that the values
   in ARRAY are actually in the tree.  There must be as many elements
   in ARRAY as there are nodes in the tree.  Exits the program if an
   error was encountered. */
void verify_tree(struct rb_tree *tree, int array[])
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
      if (!rb_search(tree, array[i])) {
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

/* Simple stress test procedure for the red-black tree routines.  Does
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

   This is pretty good test code if you write some of your own red-black
   tree routines.  If you do so you will probably want to modify the
   code below so that it increments the random seed and goes on to new
   test cases automatically. */
int main(int argc, char **argv)
{
  int array[TREE_SIZE];
  struct rb_tree *tree;
  int i;

  randomize(argc, argv);

  for (i = 0; i < TREE_SIZE; i++)
    array[i] = i;
  shuffle(array, TREE_SIZE);

  tree = rb_create();

  for (i = 0; i < TREE_SIZE; i++) {
    int result = rb_insert(tree, array[i]);
    if (result != 1) {
      printf("Error %d inserting element %d, %d, into tree.\n",
	     result, i, array[i]);
      exit(EXIT_FAILURE);
    }

    printf("Inserted %d: ", array[i]);
    /*print_structure(tree->root, 0);*/
    rb_walk(tree);
    putchar('\n');

    verify_tree(tree, array);
  }

  shuffle(array, TREE_SIZE);
  for (i = 0; i < TREE_SIZE; i++) {
    if (rb_delete(tree, array[i]) == 0) {
      printf("Error removing element %d, %d, from tree.\n", i, array[i]);
      exit(EXIT_FAILURE);
    }

    printf("Removed %d: ", array[i]);
    /*print_structure(tree->root, 0);*/
    rb_traverse(tree);
    putchar('\n');

    verify_tree(tree, array + i + 1);
  }

  rb_destroy(tree);
  printf("Success!\n");

  return EXIT_SUCCESS;
}
