#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "trie_internal.h"

/*
 * trie_create -- create a new trie
 *
 * inputs: none
 *
 * outputs: a pointer to the new trie, or NULL on error
 *
 * This attempts to dynamically allocate a new trie, initializes it to
 * empty, and returns a pointer to it.
 */
struct trie *trie_create(void)
{
  struct trie *trie = malloc( sizeof(*trie ) );

  if (trie) {
    trie->root = 0;
  }
  return trie;
}

/*
 * destroy_leaf -- deallocate a leaf
 *
 * inputs: a pointer to the leaf to free
 *
 * outputs: none
 *
 * This frees up all memory associated with the leaf.  Calling it with
 * NULL is legitimite
 */
static void destroy_leaf(struct trie_leaf *leaf)
{
  if (leaf) {
    free(leaf->key);
    free(leaf);
  }
}

/*
 * destroy_node -- deallocate a node of any type
 *
 * inputs: a pointer to the node to free
 *
 * outputs: none
 *
 * This frees up all memory associated with the node.  Calling it with
 * NULL is legitimite
 */
static void destroy_node(trie_pointer node)
{
  if (node == 0)
    return;

  switch (*node) {
    case TRIE_LEAF: {
      /* If it's a leaf, we have a dedicated routine to free it */
      struct trie_leaf *p = (struct trie_leaf*)node;
      destroy_leaf(p);
      break;
    }
    case TRIE_SUBTRIE: {
      /*
       * If it's a subtrie, we need to free the exact_match (if any) and
       * its children before we free up the memory taken by the structure
       * itself
       */
      struct trie_subtrie *p = (struct trie_subtrie*)node;
      int i;
      destroy_leaf(p->exact_match);
      for (i=0; i<TRIE_BRANCH_FACTOR; i++)
        destroy_node(p->next_level[i]);
      free(p);
      break;
    }
    default: assert(0);
  }
}

/*
 * trie_destroy -- destroy a trie
 *
 * inputs: a pointer to the trie to free
 *
 * outputs: none
 *
 * This frees up all memory associated with the trie.  If any keys were
 * inserted into the trie, any memory associated with those are freed as
 * well.
 * Once this returns, it is no longer legitimite to refer to the trie
 */
void trie_destroy(struct trie *trie)
{
  if (trie) {
    destroy_node(trie->root);
    free(trie);
  }
}

/*
 * compare_leaf -- check a leaf to see if it matches the key we are
 *                 searching for
 *
 * inputs: a pointer to the leaf to compare
 *         the key and its length we are searching for
 *
 * outputs: the result of the search
 *
 * This does the final check for a search on this key.  It assumes that the
 * trie walk has ensured that this leaf is the only possible match.  This
 * checks this leaf to see if it is a match, and returns what the search
 * should return, either the result if this is a match, or NULL if it isn't
 */
static trie_result compare_leaf( const struct trie_leaf *leaf,
                                  const unsigned char *key, size_t len_key )
{
  if (!leaf || len_key != leaf->len_key ||
      0 != memcmp( key, leaf->key, len_key)) {
    return 0;
  } else
    return leaf->result;
}

/*
 * trie_search -- search the trie for an entry
 *
 * inputs: trie -- a pointer to the trie to search
 *         key -- a pointer to the key to look for
 *         len_key -- the length (in bytes) of the key to look for
 *
 * outputs: the result of the key, or NULL if the key was not found
 *
 * This searches for the given key through the trie.  If that key has been
 * successfully inserted, and has not been removed since, this will return
 * the trie_result that was specified during the insert.
 */
trie_result trie_search(const struct trie *trie, const unsigned char *key, size_t len_key)
{
  trie_pointer node;
  key_walker walker;
  const struct trie_leaf *leaf;

  if (trie == 0)
    return 0;

  /*
   * The general strategy is to extact bits from the key, and use those
   * bits to guide us down the trie, and we continue the walk until either
   * we hit something other than a TRIE_SUBTRIE (which means we hit a NULL or
   * a leaf, and so there is at most one possible match), or we run out of key,
   * in which case we look at the current nodes exact_match
   */
  initialize_walker(walker, key, len_key);
  node = trie->root;
  for (;;) {
    const struct trie_subtrie *q;
    int n;

    /* If we hit a NULL, there obviously isn't a match */
    if (!node) {
      return 0;
    }
    /* If we hit something other than a TRIE_SUBTRIE, we have limited the */
    /* possible matches down to one */
    if (*node != TRIE_SUBTRIE)
      break;

    q = (const struct trie_subtrie*)node;
    n = extract_next(walker);
    if (n < 0) {
      /* We ran out of key -- see if the node has an exact_match value */
      /* that we match on */
      return compare_leaf( q->exact_match, key, len_key );
    } else {
      /* Index down the next level based on the next bits of the key */
      assert( n < TRIE_BRANCH_FACTOR );
      node = q->next_level[n];
    }
  }

  /* If hit a leaf -- check to see if that leaf is what we are looking for */
  assert( *node == TRIE_LEAF );
  leaf = (const struct trie_leaf*)node;
  return compare_leaf( leaf, key, len_key );
}

/*
 * insert_node -- insert an entry into a node
 *
 * inputs: old_node -- a pointer to the node we are inserting into
 *         walker -- a pointer to the key walker for the key we are inserting
 *         level -- the depth in the trie of the node we are inserting into
 *                  (the root node is depty 0, the nodes immediately beneath
 *                  that are depth 1, etc)
 *         leaf -- pointer to the leaf to insert
 *
 * outputs: pointer to the new node if leaf successfully inserted, NULL on
 *          error
 *
 * This attempts to insert the given key into a node.
 * There are two error conditions: malloc failure, and if the key already
 * exists within the trie.
 * On success, the returned pointer should be used to reference the revised
 * node, the old pointer and the pointer to the leaf should be forgotten.  On
 * error, the node is unchanged, and the old pointer should continue to be used
 *
 * Having the old pointer be NULL is supported, and is interpreted to mean
 * that you are inserting the leaf into a node that does not correspond to
 * any keys
 */
static trie_pointer insert_node( trie_pointer old_node, key_walker *walker, int level,
                                               struct trie_leaf *leaf )
{
  /*
   * If the old pointer is NULL, then the leaf is the appropriate value for
   * the new node
   */
  if (!old_node) {
    return (trie_pointer)leaf;
  }
  switch (*old_node) {
    /*
     * Inserting a new leaf onto an existing leaf.  The strategy here is to
     * expand the old leaf into a full node, and then recursively call
     * insert_node to insert the new leaf into it
     */
    case TRIE_LEAF: {
      struct trie_subtrie *new_node;
      struct trie_leaf *previous_leaf = (struct trie_leaf*)old_node;
      trie_pointer result;
      int i, n;

      /*
       * Create a new subtrie, and attach the old leaf into it.  Note that,
       * since we aren't certain that the insert won't fail later, we need
       * to be sure we don't do any irrecoverable changes quite yet
       */
      new_node = malloc( sizeof( *new_node ));
      if (!new_node) return 0;

      new_node->type = TRIE_SUBTRIE;
      new_node->exact_match = 0;
      for (i=0; i<TRIE_BRANCH_FACTOR; i++)
        new_node->next_level[i] = 0;

      /*
       * Figure out exactly where in the node the old leaf would go
       */
      n = extract_at_offset( previous_leaf->key, previous_leaf->len_key,
                                                                   level );
      if (n < 0) {
        /* The previous key doesn't extend any farther, put it in the
        /* exact_match spot */
        new_node->exact_match = previous_leaf;
      } else {
        /* Place the previous leaf in the appropriate spot in the subtrie */
        assert( n < TRIE_BRANCH_FACTOR );
        new_node->next_level[n] = (trie_pointer)previous_leaf;
      }
      new_node->count = 1;

      /*
       * Now, attempt to recursively insert the new leaf into the subtrie we
       * just created
       */
      result = insert_node( (trie_pointer)new_node, walker, level, leaf );

      /*
       * If we failed, then destroy the subtrie we just spent so much time
       * creating, but not the old leaf underneath it.  Since we haven't
       * modified the old leaf, there is nothing to undo there
       */
      if (!result) {
        free(new_node);
        return 0;
      }

      /*
       * We are successful, the resulting node is exactly what the caller
       * wants
       */
      return result;
    }

    /*
     * Inserting a new leaf onto an existing subtrie.  Here, we ask the walker
     * for where the search would look, and attempt to insert the leaf there
     */
    case TRIE_SUBTRIE: {
      struct trie_subtrie *node = (struct trie_subtrie*)old_node;
      int n;

      n = extract_next( *walker );

      if (n < 0) {
        /*
         * If we ran out of key, then install this as an exact match, unless
         * there's already one there
         */
        if (node->exact_match) {
          return 0;
        }
        node->exact_match = leaf;
      } else {
        /*
         * The next set of bits is n -- recursively call insert_node to insert
         * the leaf into the n-th next_level node
         */
        assert( n < TRIE_BRANCH_FACTOR );
        if (node->next_level[n]) {
          trie_pointer next_level = insert_node( node->next_level[n],
                                   walker, level+1, leaf );
          if (!next_level) {
            return 0;
          }
          node->next_level[n] = next_level;
        } else
          node->next_level[n] = (trie_pointer)leaf;
      }
      /*
       * In either case, if we reach here, we succesfully added the leaf.
       */
      node->count += 1;
      return (trie_pointer)node;
    }
    default:
      assert(0);
      return 0;
  }
}

/*
 * trie_insert -- insert an entry into a trie
 *
 * inputs: trie -- a pointer to the trie to insert into
 *         key -- a pointer to the key to insert
 *         len_key -- the length (in bytes) of the key to insert
 *         result -- the value that trie_search should return when it finds
 *                   this entry
 *
 * outputs: nonzero on success, zero on error
 *
 * This attempts to insert the given key into the trie, so that if you
 * search for that specific key, you will find it.
 * There are two error conditions: malloc failure, and if the key already
 * exists within the trie.  On error, the trie is unchanged.
 * The trie makes a copy of the key.  When trie_insert returns, you are
 * free to reuse the memory that you used to specify the key
 */
int trie_insert(struct trie *trie, const unsigned char *key, size_t len_key,
            trie_result result) {
  if (trie) {
    trie_pointer p;
    key_walker walker;
    struct trie_leaf *leaf;

    /*
     * Attempt to allocate the leaf.  We do it here, so that the insert_node
     * routine doesn't have to worry about it
     */
    leaf = malloc( sizeof( *leaf ) );
    if (!leaf)
      return 0;
    leaf->key = malloc( len_key > 0 ? len_key : 1 );
    if (!leaf->key) {
      free(leaf);
      return 0;
    }
    memcpy( leaf->key, key, len_key );
    leaf->type = TRIE_LEAF;
    leaf->len_key = len_key;
    leaf->result = result;

    /* Initialize the walker for the key */
    initialize_walker( walker, key, len_key );

    /* Attempt to insert the leaf into root node */
    p = insert_node( trie->root, &walker, 0, leaf );

    /* If we were successful, store the updated root, and return success */
    if (p) {
      trie->root = p;
      return 1;
    }

    /* We failed for some reason, free up the now worthless leaf */
    destroy_leaf( leaf );
  }
  return 0;
}

/*
 * delete_node -- remove an entry from a node
 *
 * inputs: node -- a pointer to the pointer to the node we are modifying
 *         walker -- key walker for the key we are removing
 *         key -- a pointer to the key to delete
 *         len_key -- the length (in bytes) of the key to delete
 *
 * outputs: nonzero on success, zero on error
 *          updates *node on success, if appropriate
 *
 * This attempts to remove the given key from the node, returning the node
 * to the state as if the key had never been inserted
 * There is only one error condition: if the key was not found within the
 * node.  On error, the node is unchanged.
 */
static int delete_node( trie_pointer *node, key_walker *walker,
             const unsigned char *key, size_t len_key )
{
  /* If the pointer is NULL, then obviously the key was not a member of */
  /* this node */
  if (!*node)
    return 0;

  switch (**node) {
    /*
     * We're removing the leaf from a leaf -- this is succesful only
     * if they correspond to the same key
     */
    case TRIE_LEAF: {
      struct trie_leaf *p = (struct trie_leaf*)*node;
      if (len_key != p->len_key || 0 != memcmp( key, p->key, len_key ))
        return 0;

      /*
       * They do -- free up the leaf, and reset the pointer to NULL (now, the
       * node corresponds to no keys
       */
      destroy_leaf(p);
      *node = 0;
      return 1;
    }

    /*
     * We're removing the leaf from a subtrie -- try to remove it from the
     * approriate subnode, and then check if we need to collapse
     */
    case TRIE_SUBTRIE: {
      struct trie_subtrie *p = (struct trie_subtrie*)*node;
      int n;
      n = extract_next( *walker );
      if (n < 0) {
        /* If we run out of key, then we need to delete the exact match */
        if (p->exact_match == 0)
          return 0;

        /*
         * If there is an exact match in this node, it must correspond
         * to the key we're removing
         */
        assert( len_key == p->exact_match->len_key &&
                         0 == memcmp( key, p->exact_match->key, len_key ) );
        destroy_leaf( p->exact_match );
        p->exact_match = 0;
      } else {
        /* If the next set of output is N, delete the leaf from that subnode */
        if (!delete_node( &p->next_level[n], walker, key, len_key ))
          return 0;
      }

      /*
       * If we reach here, we have successfully removed the leaf from the node
       * Now, we need to check if we need to collapse the node into a leaf.
       * We do this if the number of keys that this node keeps track of falls
       * to one.
       */
      p->count -= 1;
      assert( p->count > 0 );
      if (p->count == 1) {
        trie_pointer leaf;
        int i;

        /*
         * We need to collapse the node.  Fortunately, we know that the
         * sublevels must also be collapsed, and so we can just scan through
         * all our children for a non-NULL subnode
         */
        leaf = (trie_pointer)p->exact_match;
        for (i=0; i<TRIE_BRANCH_FACTOR; i++)
          if (p->next_level[i]) {
            assert( leaf == 0 );
            leaf = p->next_level[i];
          }

        /* We must have found a child which must be a leaf */
        assert( leaf != 0 );
        assert( *leaf == TRIE_LEAF );

        /* We can use that leaf as the new node, free up the old subtrie */
        free(p);
        *node = leaf;
      }
      return 1;
    }

    default:
      assert(0);
      return 0;
  }
}

/*
 * trie_delete -- remove an entry from a trie
 *
 * inputs: trie -- a pointer to the trie to remove from
 *         key -- a pointer to the key to delete
 *         len_key -- the length (in bytes) of the key to delete
 *
 * outputs: nonzero on success, zero on error
 *
 * This attempts to remove the given key from the trie, so that if you
 * search for that specific key, it will not be found.
 * There is only one error condition: if the key was not found within the
 * trie.  On error, the trie is unchanged.
 */
int trie_delete( struct trie *trie, const unsigned char *key, size_t len_key )
{
  if (trie) {
    key_walker walker;

    initialize_walker( walker, key, len_key );
    return delete_node( &trie->root, &walker, key, len_key );
  }  
  return 0;
}
