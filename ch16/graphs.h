/* graphs.h */

/* This is the top level header file for the graph library.
   It is designed to be a simple way in which to create and work with
   any kind of graph. The functions, constants and structures defined
   here are primitive operations - creation, destruction and management
   of graphs, vertices and edges.

   The main aim is to have the same primitive functions for both
   adjacency list and matrix representations. This is done with a clever
   but peculiar trick. Graph.Graph_Spec holds information about the
   type of graph (adj. matrix or list) which is used in some of the
   primitive functions. This way, both representations have the same
   functionality through the same functions.

   Any code written which works with one, will work with the other
   without any alterations. Indeed, if you knew a lot about the kind
   of graph you will be working with, you could create a specialist
   representation which was more efficient than the two already
   provided for that particular kind of graph.

   For all but the most extreme circumstances, the two provided
   representations will suffice.
*/

#ifndef GRAPHS_H
#define GRAPHS_H

#include <limits.h>

enum GraphType {Matrix, List};

#define GRAPH_NOTCONNECTED			INT_MAX
/* Since we can't store infinity, we use an arbitry large number instead.
   In an adjacency matrix, an unconnected 'edge' is representing as being
   an edge of infinite cost. We'll use INT_MAX instead.
*/

#define GRAPH_OUTOFMEM				-1			/* we're out of memory */
#define GRAPH_BADPARAM				-2			/* bad paramater (-ive index, NULL pointer) passed */
#define GRAPH_NOTFOUND				-3			/* failed to find... whatever it was */
#define GRAPH_BADGRAPH				-4
	/* the graph is 'bad'. For instance, Dijstras algorithm implementations will return this when the graph
	   it's working with has negative weighted edges */

struct Graph
{
	int NumVertices;
	struct Graph_Vertex ** Vertices;
	struct Graph_Spec * Private;				/* internal value */
};

struct Graph_Vertex
{
	union
	{
		void * Ptr;
		int Num;
	} Tag;										/* user specified value */
	union
	{
		int * Matrix;
		struct Graph_AdjList * List;
	} Edges;									/* internal values */
};

struct EdgeScan
{
	struct Graph * G;
	int Cost;
	int Source;
	int Dest;
	union
	{
		void * Ptr;
		int Index;
	} Internal;									/* used internally to record position */
};

/* ---------------------------- */
/* Creating / Destroying Graphs */
/* ---------------------------- */

struct Graph * MakeGraph(enum GraphType T);
	/* Allocates memory and initialises a graph of the specified type.
	   NULL is returned on error (out of memory)
	   otherwise a pointer to the graph structure is returned.
	*/

int FreeGraph(struct Graph * G);
	/* Frees up the memory associated with a graph G, including all vertices.
	   0 is returned on success, <0 indicates an error (GRAPH_BADPARAM)
	*/

/* ----------------- */
/* Managing Vertices */
/* ----------------- */

int AddVertex(struct Graph * G);
	/* Adds an empty, unconnected vertex to specified graph
	   index of new vertex is returned (i returned, G->Vertices[i] is the new vertex
	   index is always >=0. If returns <0, then an error has occured (GRAPH_BADPARAM, GRAPH_OUTOFMEM)
	*/

int RemoveVertex(struct Graph * G, int Index);
	/* Removes the indicated vertex from the graph. Indices are adjusted so that all indices (if any)
	   greater than Index are decremented by 1, so no 'holes' exist in G->Vertices. Any connections to
	   the removed vertex are discarded.
	   0 is returned on success, !0 indicates an error (GRAPH_BADPARAM)
	*/

int FindVertex(struct Graph * G, void * Ptr, int Num);
	/* Scans the vertices in a graph for a vertex with the relavent Tag.
	   If Ptr==NULL, then Num is used as the search key. Otherwise Ptr is used.
	   Care must be taken so that a graph does not contain vertices which have
	   a mixture of Num and Ptr tags. Simply... don't do it.
	   Returns index of vertex >=0 or an error <0 (GRAPH_BADPARAM, GRAPH_NOTFOUND)
	*/

/* -------------- */
/* Managing Edges */
/* -------------- */

int ConnectVertex(struct Graph * G, int Source, int Destination, int Cost);
	/* Make an edge from Source to Destination of specified cost.
	   Cost must be < GRAPH_NOTCONNECTED
	   Returns 0 on success, or <0 on error (GRAPH_BADPARAM).
	   If Source is already connected to Destination, then the cost is updated to Cost
	*/

int DisconnectVertex(struct Graph * G, int Source, int Destination);
	/* Disconects an edge from Source to Destination. If Source is not connected to
	   Destination, then 0 (success) is returned.
	   Returns 0 on success, or <0 on error (GRAPH_BADPARAM)
	*/

int EdgeScanStart(struct Graph * G, int Index, struct EdgeScan * EScan);
	/* Initialises Escan in preparation to scan through the edges for vertex
	   denoted by Index. EdgeScanNext can now be used to retrieve edge info
	   Returns 0 on success, or <0 on error (GRAPH_BADPARAM)
	*/

int EdgeScanEnd(struct EdgeScan * EScan);
	/* Does the opposite of an initialisation. It frees up any memory that
	   may have been allocated in order to perform the scan.
	   Returns 0 on success, or <0 on error (GRAPH_BADPARAM)
	*/

int EdgeScanNext(struct EdgeScan * EScan);
	/* Retrieves information about an edge, putting it in EScan.
	   0 is returned on success. <0 is returned on error (GRAPH_BADPARAM)
	   >0 is returned when there are no more edges.
	*/

#endif
