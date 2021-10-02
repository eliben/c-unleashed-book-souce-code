#if !defined( TRIE_H_ )
#define TRIE_H_

/*
 * This file defines the public interface for the trie management module
 */

#include <stddef.h>

/*
 * This is the type of the result of the search.  It is currently a void*
 * because this module has no idea what type the application actually
 * needs to use.
 */
typedef void *trie_result;

/*
 * A trie_pointer is used as a generic pointer to a node.  It can validly
 * point to any type of node that can appear in a trie.  This is valid
 * because all nodes that can appear within a trie are structures that
 * have a enum trie_node_type as its first element, and a pointer to a
 * structure can be portably converted into a point to the first element
 */
enum trie_node_type { TRIE_LEAF, TRIE_SUBTRIE };
typedef enum trie_node_type *trie_pointer;

/*
 * This is the structure that represents the entire trie.  Of course, most
 * of the actual work is done by the nodes that this points to, but there
 * is no reason the application needs to know the details of that
 */
struct trie {
  trie_pointer root;
};

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
struct trie *trie_create(void);

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
void trie_destroy(struct trie *trie);

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
                                                    trie_result result);

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
int trie_delete(struct trie *trie, const unsigned char *key, size_t len_key);

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
trie_result trie_search(const struct trie *trie,
                                  const unsigned char *key, size_t len_key);

#endif /* TRIE_H_ */
