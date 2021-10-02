#if !defined( TRIE_INTERNAL_H_ )
#define TRIE_INTERNAL_H_

/*
 * This file defines the private internals for the trie management module
 * No file outside the trie code itself should include this
 */

#include <limits.h>
#include "trie.h"

/*
 * This is the number of bits to extract out of the key for each level,
 * along with the resulting branching factor.  This is a tunable parameter
 * that you can tweak according to the needs of the application
 * If you make this larger, the search runs faster (and, if it happens to
 * be CHAR_BIT, the code simplifies somewhat).  If you make this
 * smaller, the trie takes up less memory.
 * In practice, it should usually be either 4 or 8, with 8 being used only
 * if you really need high performance.  Anything less than 4 doesn't
 * decrease the memory usage enough to make up for the performance loss.
 * Anything more than 8 tends to chew up lots of memory without gaining
 * significantly more speed (actually, you gain no speed at all if you
 * increase TRIE_LOG_BRANCH_FACTOR > CHAR_BIT)
 */
#define TRIE_LOG_BRANCH_FACTOR 4
#define TRIE_BRANCH_FACTOR      (1<<TRIE_LOG_BRANCH_FACTOR)

/*
 * This is the internal representation of a leaf, which corresponds to
 * a single key.  Note that it can occur anywhere in the trie where we
 * have reduced the possibilities to one key
 */
struct trie_leaf {
  enum trie_node_type type; /* TRIE_LEAF.  This is the type field to */
                         /* identify the node type, and must be the first */
                         /* element of this structure */
  unsigned char *key;    /* Our copy of the key that this leaf stands for */
  size_t len_key;        /* The length of the key, in bytes */
  trie_result result;    /* The value that trie_search should return when */
                         /* we find this key */
};

/*
 * This is the internal representation of a subtrie, which corresponds to
 * a prefix that occurs in at least 2 keys.  Note that the prefix is not
 * explicitly stored here -- it is given by the position of this subtrie
 * within the trie
 *
 * In case you're wondering, a prefix that doesn't occur in any key is
 * represented by a NULL.
 */
struct trie_subtrie {
  enum trie_node_type type; /* TRIE_NODE.  This is the type field to */
                         /* identify the node type, and must be the first */
                         /* element of this structure */
  struct trie_leaf *exact_match; /* The leaf that corresponds to the key */
                         /* that ends here -- this is the leaf that matches */
                         /* if the key ends here.  NULL if it hasn't been */
                         /* inserted */
  int count;             /* The number of leaves that has the prefix */
  trie_pointer next_level[TRIE_BRANCH_FACTOR]; /* Entry N here is a pointer */
                         /* to the node that correspond to the prefix */
                         /* followed by N.  While searching, we extract the */
                         /* next set of bits from the key, and index into */
                         /* this array to get the next node */
};

/*
 * This defined the abstract object which is used to extract successive
 * sets of bits from the key.  The object is called a key_walker, and
 * two function-like macros are defined for it.  These are macros, rather
 * than functions, because they are used in the search routine, and the
 * whole point of the trie is to make the search go fast.  These macros are:
 *
 * initialize_walker, which initializes a key_walker object to start at
 * the beginning of the specified key
 * 
 * extract_next, which evaluates to the next set of bits from the key, or
 * -1 if we have reached the end of the key.
 *
 * In addition, we define the extract_at_offset macro, which takes a key and
 * an offset N, and evaluates to what the Nth extract_next would evaluate to
 * for that key
 *
 * Note that, if TRIE_BRANCH_FACTOR is big enough to include an entire unsigned
 * char, these macros simplify a bit because no bit shifting operations are
 * necessary.  Therefore, we give two different implementations, and select
 * the version based on the size of TRIE_BRANCH_FACTOR
 */
#if TRIE_BRANCH_FACTOR > UCHAR_MAX
/*
 * This is the version where we can evaluate the next output just by reading
 * the next unsigned char from the key
 */
typedef struct key_walker {
  const unsigned char *key;
  size_t len;
} key_walker;
#define initialize_walker(walker, key, len_key) \
  (void)(                                       \
    (walker).key = (key),                       \
    (walker).len = (len_key)                    \
  )
#define extract_next(walker)                    \
  ((walker).len == 0 ? -1 : ((walker).len -= 1, *(walker).key++))
#define extract_at_offset(key, len_key, offset) \
  ((len_key) <= (offset) ? -1 : ((key)[offset]))

#else

/*
 * This is the version where we must extract the next output from a
 * portion of an unsigned char from a key -- this does the shifts and
 * masks necessary.  Note that this does not attempt to combine outputs
 * from adjacent unsigned chars.
 */
#define LEVEL_PER_UCHAR  ((CHAR_BIT+TRIE_LOG_BRANCH_FACTOR-1) /        \
                                                  TRIE_LOG_BRANCH_FACTOR)

typedef struct key_walker {
  const unsigned char *key;
  size_t len;
  int bit_offset;
} key_walker;
#define initialize_walker(walker, key, len_key)               \
  (void) (                                                    \
    (walker).key = (key),                                     \
    (walker).len = (len_key),                                 \
    (walker).bit_offset = -TRIE_LOG_BRANCH_FACTOR             \
  )
#define extract_next(walker)                                  \
  ((walker).len == 0 ? -1 :                                   \
    ((walker).bit_offset >= CHAR_BIT-TRIE_LOG_BRANCH_FACTOR ? \
      (((walker).len -= 1) ? ((walker).bit_offset=0,          \
          *++(walker).key & (TRIE_BRANCH_FACTOR-1)) : -1) :   \
      ((walker).bit_offset += TRIE_LOG_BRANCH_FACTOR,         \
          (*(walker).key >> (walker).bit_offset)              \
                  & (TRIE_BRANCH_FACTOR-1))))
#define extract_at_offset(key, len_key, offset)               \
   ((len_key)*LEVEL_PER_UCHAR <= (offset) ? -1 :              \
     ((((key)[ (offset)/LEVEL_PER_UCHAR ]) >>                 \
          (TRIE_LOG_BRANCH_FACTOR*((offset)%LEVEL_PER_UCHAR))) \
                  & (TRIE_BRANCH_FACTOR-1)))
#endif

#endif /* TRIE_INTERNAL_H_ */
