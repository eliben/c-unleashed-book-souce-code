#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "trie_internal.h"
#include <stdio.h>

int
print_node( trie_pointer node, char *prefix ) {
  if (!node) return 0;
  switch (*node) {
    case TRIE_LEAF: {
      struct trie_leaf *p = (struct trie_leaf*)node;
      printf( "%s: %*.*s -> %s\n", prefix, p->len_key, p->len_key, p->key, p->result );
      return 0;
    }
    case TRIE_SUBTRIE: {
      struct trie_subtrie *p = (struct trie_subtrie*)node;
      int i;
      print_node( (trie_pointer)p->exact_match, prefix );
      for (i=0; i<TRIE_BRANCH_FACTOR; i++) {
         char new_prefix[ 100 ];
         sprintf( new_prefix, "%s %x", prefix, i );
         print_node( p->next_level[i], new_prefix );
      }
      return 0;
    }
    default:
      assert(0); /* if NDEBUG is not defined, the program will stop here */
  }
  return 1; /* something went wrong */
}

void print_trie( struct trie *trie )
{
   print_node( trie->root, "" );
}

int validate_node( trie_pointer node )
{
  switch (*node) {
    case TRIE_LEAF: return 1;
    case TRIE_SUBTRIE: {
      struct trie_subtrie *p = (struct trie_subtrie*)node;
      int i, n;
      int count = p->count;
      int count2 = 0;
      if (count < 2) return 0;
      if (p->exact_match) {
        n = validate_node( (trie_pointer)p->exact_match );
        if (!n) return 0;
        count2 += n;
      }
      for (i=0; i<TRIE_BRANCH_FACTOR; i++)
        if (p->next_level[i]) {
          n = validate_node( (trie_pointer)p->next_level[i] );
          if (!n) return 0;
          count2 += n;
        }
      if (count == count2) return count; else return 0;
    }
    default: assert( 0 ); return 0;
  }
}

int validate_trie( struct trie *trie )
{
   if (trie->root)
      return 0 != validate_node( trie->root );
   else
      return 1;
}
