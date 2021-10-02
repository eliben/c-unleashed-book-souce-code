/* mstree.h */

#ifndef MSTREE_H
#define MSTREE_H

#include "graphs.h"

int Kruskal_Undirected(struct Graph * G,struct Graph ** TreePtr);
	/* The graph G is examined to produce a new graph, which is
	   a minimum spanning tree and the struct Graph * denoted
	   by TreePtr is assigned the new value.
	   return 0 on success, <0 on error (GRAPH_BADPARAM, GRAPH_OUTOFMEM)
	*/


int Prim_Undirected(struct Graph * G,struct Graph ** TreePtr);
  /* As above, but using Prim's algorithm instead.
     return 0 on success, <0 on error (GRAPH_BADPARAM, GRAPH_OUTOFMEM)
  */

#endif
