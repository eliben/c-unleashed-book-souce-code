#include <stdlib.h>

  /* ... */

  T *tmp;
  tmp = realloc(p, NewNumElems * sizeof *p);
  if(tmp != NULL)
  {
    /* the resizing worked */
    p = tmp;
  }
  else
  {
    /* The resizing didn't work, p still points to the
     * old data.
     */
  }
