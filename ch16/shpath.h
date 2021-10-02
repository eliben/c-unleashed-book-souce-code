/* shpath.h */

#ifndef SHPATH_H
#define SHPATH_H

#include "graphs.h"

/* ************************************ */
/* Dijkstra's algorithm implementations */
/* ************************************ */

struct Dijkstra_Table
{
	struct Graph * G;
	int Source;							/* index of source vertex */
	struct Dijkstra_Row * Results;		/* table of results */
};

struct Dijkstra_Row
{
	int Total;							/* Total cost from source of shortest path found */
	int Previous;						/* index of vertex preceeding this one in the path, or -1 if there is no previous vertex */
	int Visited;
};

void Dijkstra_InitTable(struct Dijkstra_Table * Table);
	/* Initialises a table. It simply sets all the members to meaningful empty
	   values. */

void Dijkstra_FreeTable(struct Dijkstra_Table * Table);
	/* Frees memory occupied by the results (and not Table itself) */

int Dijkstra_Simple(struct Graph * G, int Source, struct Dijkstra_Table * Table);
	/* Implementation of Dijkstra's shortest path algorithm. The table must be
	   initialised before it is used, and it's contents freed when it is finished
	   with, or before it is reused.
	   0 is returned on success, an error code (<0) is returned otherwise (GRAPH_OUTOFMEM, GRAPH_BADGRAPH)
	   GRAPH_BADGRAPH is returned when a graph with a negative cost edge is passed. In this
	   case, Bellman's algorithm should be used to ensure correct results.
	*/

int Dijkstra_Sparse(struct Graph * G, int Source, struct Dijkstra_Table * Table);
	/* Exactly the same as Dijkstra_Simple, but should be faster for sparse graphs */

int Bellman(struct Graph * G, int Source, struct Dijkstra_Table * Table);
	/* This is a drop in replacement for the above two implementations of Dijkstra's
	   algorithm. Bellman's algorithm allows for the existance of edges of negative
       cost, but at the expense of longer running time.
	   0 is returned on success, an error code (<0) is returned otherwise (GRAPH_OUTOFMEM, GRAPH_BADGRAPH)
	   GRAPH_BADGRAPH is returned when a graph with a negative cost cycle is passed, which
	   prevents the algorithm from infinitely looping.
	*/

/* ******************************** */
/* Floyd's algorithm implementation */
/* ******************************** */

struct Floyd_Table
{
	struct Graph * G;
	struct Floyd_Result ** Results;		/* 2 dimensional dynamic array holding results */
};

struct Floyd_Result
{
	int Total;
	int Previous;
};

void Floyd_InitTable(struct Floyd_Table * Table);
	/* Initialises the given table */

void Floyd_FreeTable(struct Floyd_Table * Table);
	/* Frees the memory used be results in the table. Does not free Table itself */

int Floyd(struct Graph * G,struct Floyd_Table * Table);
	/* Uses Floyds algorithm to calculated the shortest path for all pairs of vertices in G.
	   Returns 0 on succes, or <0 on error (GRAPH_OUTOFMEM, GRAPH_BADPARAM)
	   Passing a graph containing a negative cost cycle will produce spurious results
	   but will not result in segmentation fault, infinite loops or similar
	*/

#endif
