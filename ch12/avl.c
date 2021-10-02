/* avl - manipulates AVL trees.
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

/* An AVL tree node. */
struct avl_node {
  struct avl_node *link[2];
  int data;
  short bal;
};

/* An AVL tree. */
struct avl_tree {
  struct avl_node *root;
  int count;
};

/* Creates and returns a new AVL tree.  Returns a null pointer if a
   memory allocation error occurs. */
struct avl_tree *avl_create(void)
{
  struct avl_tree *tree = malloc(sizeof *tree);
  if (tree == NULL)
    return NULL;
  tree->root = NULL;
  tree->count = 0;
  return tree;
}

/* Searches TREE for matching ITEM.  Returns 1 if found, 0
   otherwise. */
int avl_search(const struct avl_tree *tree, int item)
{
  const struct avl_node *node;
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
static struct avl_node *new_node(struct avl_tree *tree, int item)
{
  struct avl_node *node = malloc(sizeof *node);
  if (node == NULL)
    return NULL;
  node->data = item;
  node->link[0] = node->link[1] = NULL;
  node->bal = 0;
  tree->count++;
  return node;
}

/* Inserts ITEM into TREE.  Returns 1 if the item was inserted, 2 if
   an identical item already existed in TREE, or 0 if a memory
   allocation error occurred. */
int avl_insert(struct avl_tree *tree, int item)
{
  struct avl_node **v, *w, *x, *y, *z;

  assert(tree != NULL);
  v = &tree->root;
  x = z = tree->root;
  if (x == NULL) {
    tree->root = new_node(tree, item);
    return tree->root != NULL;
  }

  for (;;) {
    int dir;
    if (item == z->data)
      return 2;

    dir = item > z->data;
    y = z->link[dir];
    if (y == NULL) {
      y = z->link[dir] = new_node(tree, item);
      if (y == NULL)
	return 0;
      break;
    }

    if (y->bal != 0) {
      v = &z->link[dir];
      x = y;
    }
    z = y;
  }

  w = z = x->link[item > x->data];
  while (z != y)
    if (item < z->data) {
      z->bal = -1;
      z = z->link[0];
    }
    else {
      z->bal = +1;
      z = z->link[1];
    }

  if (item < x->data) {
    if (x->bal != -1)
      x->bal--;
    else if (w->bal == -1) {
      *v = w;
      x->link[0] = w->link[1];
      w->link[1] = x;
      x->bal = w->bal = 0;
    }
    else {
      assert(w->bal == +1);
      *v = z = w->link[1];
      w->link[1] = z->link[0];
      z->link[0] = w;
      x->link[0] = z->link[1];
      z->link[1] = x;
      if (z->bal == -1) {
	x->bal = 1;
	w->bal = 0;
      }
      else if (z->bal == 0)
	x->bal = w->bal = 0;
      else {
	assert(z->bal == +1);
	x->bal = 0;
	w->bal = -1;
      }
      z->bal = 0;
    }
  }
  else {
    if (x->bal != +1)
      x->bal++;
    else if (w->bal == +1) {
      *v = w;
      x->link[1] = w->link[0];
      w->link[0] = x;
      x->bal = w->bal = 0;
    }
    else {
      assert(w->bal == -1);
      *v = z = w->link[0];
      w->link[0] = z->link[1];
      z->link[1] = w;
      x->link[1] = z->link[0];
      z->link[0] = x;
      if (z->bal == +1) {
	x->bal = -1;
	w->bal = 0;
      }
      else if (z->bal == 0)
	x->bal = w->bal = 0;
      else {
	assert(z->bal == -1);
	x->bal = 0;
	w->bal = 1;
      }
      z->bal = 0;
    }
  }
  return 1;
}

/* Deletes any item matching ITEM from TREE.  Returns 1 if such an
   item was deleted, 0 if none was found. */
int avl_delete(struct avl_tree *tree, int item)
{
  struct avl_node *ap[32];
  int ad[32];
  int k = 1;

  struct avl_node **y, *z;

  assert(tree != NULL);

  ad[0] = 0;
  ap[0] = (struct avl_node *) &tree->root;

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
  y = &ap[k - 1]->link[ad[k - 1]];
  if (z->link[1] == NULL)
    *y = z->link[0];
  else {
    struct avl_node *x = z->link[1];
    if (x->link[0] == NULL) {
      x->link[0] = z->link[0];
      *y = x;
      x->bal = z->bal;
      ad[k] = 1;
      ap[k++] = x;
    }
    else {
      struct avl_node *w = x->link[0];
      int j = k++;

      ad[k] = 0;
      ap[k++] = x;
      while (w->link[0] != NULL) {
	x = w;
	w = x->link[0];
	ad[k] = 0;
	ap[k++] = x;
      }

      ad[j] = 1;
      ap[j] = w;
      w->link[0] = z->link[0];
      x->link[0] = w->link[1];
      w->link[1] = z->link[1];
      w->bal = z->bal;
      *y = w;
    }
  }

  free(z);
  assert(k > 0);
  while (--k) {
    struct avl_node *w, *x;

    w = ap[k];
    if (ad[k] == 0) {
      if (w->bal == -1) {
	w->bal = 0;
	continue;
      }
      else if (w->bal == 0) {
	w->bal = 1;
	break;
      }

      assert(w->bal == +1);

      x = w->link[1];
      assert(x != NULL);

      if (x->bal > -1) {
	w->link[1] = x->link[0];
	x->link[0] = w;
	ap[k - 1]->link[ad[k - 1]] = x;
	if (x->bal == 0) {
	  x->bal = -1;
	  break;
	}
	else
	  w->bal = x->bal = 0;
      }
      else {
	assert(x->bal == -1);
	z = x->link[0];
	x->link[0] = z->link[1];
	z->link[1] = x;
	w->link[1] = z->link[0];
	z->link[0] = w;
	if (z->bal == +1) {
	  w->bal = -1;
	  x->bal = 0;
	}
	else if (z->bal == 0)
	  w->bal = x->bal = 0;
	else {
	  assert(z->bal == -1);
	  w->bal = 0;
	  x->bal = +1;
	}
	z->bal = 0;
	ap[k - 1]->link[ad[k - 1]] = z;
      }
    }
    else {
      assert(ad[k] == 1);
      if (w->bal == +1) {
	w->bal = 0;
	continue;
      }
      else if (w->bal == 0) {
	w->bal = -1;
	break;
      }

      assert(w->bal == -1);

      x = w->link[0];
      assert(x != NULL);

      if (x->bal < +1) {
	w->link[0] = x->link[1];
	x->link[1] = w;
	ap[k - 1]->link[ad[k - 1]] = x;
	if (x->bal == 0) {
	  x->bal = +1;
	  break;
	}
	else
	  w->bal = x->bal = 0;
      }
      else if (x->bal == +1) {
	z = x->link[1];
	x->link[1] = z->link[0];
	z->link[0] = x;
	w->link[0] = z->link[1];
	z->link[1] = w;
	if (z->bal == -1) {
	  w->bal = 1;
	  x->bal = 0;
	}
	else if (z->bal == 0)
	  w->bal = x->bal = 0;
	else {
	  assert(z->bal == 1);
	  w->bal = 0;
	  x->bal = -1;
	}
	z->bal = 0;
	ap[k - 1]->link[ad[k - 1]] = z;
      }
    }
  }

  return 1;
}

/* Helper function for avl_walk().  Recursively prints data from each
   node in tree rooted at NODE in in-order. */
static void walk(const struct avl_node *node)
{
  if (node == NULL)
    return;
  walk(node->link[0]);
  printf("%d ", node->data);
  walk(node->link[1]);
}

/* Prints all the data items in TREE in in-order. */
void avl_walk(const struct avl_tree *tree)
{
  assert(tree != NULL);
  walk(tree->root);
}

/* Prints all the data items in TREE in in-order, using an iterative
   algorithm. */
void avl_traverse(const struct avl_tree *tree)
{
  struct avl_node *stack[32];
  int count;
  struct avl_node *node;

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
static void destroy(struct avl_node *node)
{
  if (node == NULL)
    return;
  destroy(node->link[0]);
  destroy(node->link[1]);
  free(node);
}

/* Destroys TREE. */
void avl_destroy(struct avl_tree *tree)
{
  assert(tree != NULL);
  destroy(tree->root);
  free(tree);
}

/* Returns the number of data items in TREE. */
int avl_count(const struct avl_tree *tree)
{
  assert(tree != NULL);
  return tree->count;
}

/* Print the structure of node NODE of an AVL tree, which is LEVEL
   levels from the top of the tree.  Uses different delimiters to
   visually distinguish levels. */
void print_structure(struct avl_node *node, int level)
{
  assert(level <= 100);
  if (node == NULL) {
    printf(" nil");
    return;
  }

  printf(" %c%d", "([{`/"[level % 5], node->data);
  if (node->link[0] || node->link[1]) {
    print_structure(node->link[0], level + 1);
    if (node->link[1])
      print_structure(node->link[1], level + 1);
  }
  printf("%c", ")]}'\\"[level % 5]);
}

/* Examine NODE in an AVL tree.  *COUNT is increased by the number of
   nodes in the tree, including the current one.  If the node is the
   root of the tree, PARENT should be INT_MIN, otherwise it should be
   the parent node value.  If this node is a left child of its parent
   node, DIR should be -1.  Otherwise, if it is not the root, it is a
   right child and DIR must be +1.  Sets *OKAY to 0 if an error is
   found.  Returns the height of the tree rooted at NODE. */
int
recurse_tree(struct avl_node *node, int *count, int parent, int dir, int *okay)
{
  int lh, rh;
  if (node == NULL)
    return 0;

  assert(count != NULL);
  (*count)++;
  lh = recurse_tree(node->link[0], count, node->data, -1, okay);
  rh = recurse_tree(node->link[1], count, node->data, +1, okay);
  if (rh - lh != node->bal) {
    printf(" Node %d is unbalanced: right height=%d, left height=%d, "
	   "difference=%d, but balance factor=%d.\n", node->data,
	   rh, lh, rh - lh, node->bal);
    *okay = 0;
  }
  else if (*okay && (node->bal < -1 || node->bal > +1)) {
    printf(" Node %d has balance factor %d not between -1 and +1.\n",
	   node->data, node->bal);
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

  return 1 + (lh > rh ? lh : rh);
}

/* Checks that TREE's structure is kosher and verifies that the values
   in ARRAY are actually in the tree.  There must be as many elements
   in ARRAY as there are nodes in the tree.  Exits the program if an
   error was encountered. */
void verify_tree(struct avl_tree *tree, int array[])
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
      if (!avl_search(tree, array[i])) {
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

/* Simple stress test procedure for the AVL tree routines.  Does
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

   This is pretty good test code if you write some of your own AVL
   tree routines.  If you do so you will probably want to modify the
   code below so that it increments the random seed and goes on to new
   test cases automatically. */
int main(int argc, char **argv)
{
  int array[TREE_SIZE];
  struct avl_tree *tree;
  int i;

  randomize(argc, argv);

  for (i = 0; i < TREE_SIZE; i++)
    array[i] = i;
  shuffle(array, TREE_SIZE);
  tree = avl_create();

  for (i = 0; i < TREE_SIZE; i++) {
    int result = avl_insert(tree, array[i]);
    if (result != 1) {
      printf("Error %d inserting element %d, %d, into tree.\n",
	     result, i, array[i]);
      exit(EXIT_FAILURE);
    }
    printf("Inserted %d: ", array[i]);

    /*print_structure(tree->root, 0); */
    avl_walk(tree);
    putchar('\n');
    verify_tree(tree, array);
  }

  shuffle(array, TREE_SIZE);
  for (i = 0; i < TREE_SIZE; i++) {
    if (avl_delete(tree, array[i]) == 0) {
      printf("Error removing element %d, %d, from tree.\n", i, array[i]);
      exit(EXIT_FAILURE);
    }
    printf("Removed %d: ", array[i]);

    /* print_structure(tree->root, 0); */
    avl_traverse(tree);
    putchar('\n');
    verify_tree(tree, array + i + 1);
  }
  avl_destroy(tree);
  printf("Success!\n");
  return EXIT_SUCCESS;
}
