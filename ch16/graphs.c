/* graphs.c */

#include <stdlib.h>
#include "graphs.h"
#include "graphprv.h"
#include "adjlist.h"
#include "adjmatrix.h"

struct Graph_Spec * Specs[]={&AdjMatrix_Spec, &AdjList_Spec};

/* ---------------------------- */
/* Creating / Destroying Graphs */
/* ---------------------------- */

struct Graph * MakeGraph(enum GraphType T)
	/* Allocates memory and initialises a graph of the specified type.
	   NULL is returned on error (out of memory)
	   otherwise a pointer to the graph structure is returned.
	*/
{
	struct Graph * G;

	if (T!=Matrix && T!=List) return NULL;

	G=malloc(sizeof(struct Graph));
	if (!G) return NULL;			/* we're out of memory */

	G->Private=Specs[T];
	G->NumVertices=0;
	G->Vertices=NULL;

	if ((*G->Private->MakeGraph)(G))
	{
		free(G);
		return NULL;				/* error (failed to initialise - out of memory) */
	}

	return G;
}


int FreeGraph(struct Graph * G)
	/* Frees up the memory associated with a graph G, including all vertices.
	   0 is returned on success, <0 indicates an error (GRAPH_BADPARAM)
	*/
{
	if (!G) return GRAPH_BADPARAM;		/* check for freeing a NULL pointer */

	(*G->Private->FreeGraph)(G);
	free(G->Vertices);
	free(G);

	return 0;
}

/* ----------------- */
/* Managing Vertices */
/* ----------------- */

int AddVertex(struct Graph * G)
	/* Adds an empty, unconnected vertex to specified graph
	   index of new vertex is returned (i returned, G->Vertices[i] is the new vertex
	   index is always >=0. If returns <0, then an error has occured (GRAPH_BADPARAM, GRAPH_OUTOFMEM)
	*/
{
	int Index;
	void * ptr;

	if (!G) return GRAPH_BADPARAM;

	Index=G->NumVertices;
	G->NumVertices++;

	ptr=realloc(G->Vertices,sizeof(struct Graph_Vertex *) * G->NumVertices);
	if (!ptr) return GRAPH_OUTOFMEM;
	G->Vertices=ptr;

	G->Vertices[Index]=malloc(sizeof(struct Graph_Vertex));

	if (!G->Vertices || (*G->Private->AddVertex)(G,Index))
	{
		/* undo what we've done */
		G->NumVertices--;
		free(G->Vertices[Index]);

		ptr=realloc(G->Vertices,sizeof(struct Graph_Vertex *) * G->NumVertices);
		if (ptr) G->Vertices=ptr;		/* if ptr==NULL, then just ignore the error. There's not much else we can do */
	
		return GRAPH_OUTOFMEM;
	}

	G->Vertices[Index]->Tag.Ptr=NULL;

	return Index;
}

int RemoveVertex(struct Graph * G, int Index)
	/* Removes the indicated vertex from the graph. Indices are adjusted so that all indices (if any)
	   greater than Index are decremented by 1, so no 'holes' exist in G->Vertices. Any connections to
	   the removed vertex are discarded.
	   0 is returned on success, !0 indicates an error (GRAPH_BADPARAM)
	*/
{
	int i, retval;
	void * ptr;

	if (!G || Index<0 || Index>=G->NumVertices) return GRAPH_BADPARAM;

	for (i=0;i<G->NumVertices;i++)
	{
		retval=DisconnectVertex(G,i,Index);
		if (retval) return retval;				/* should never happen, but just in case */
	}

	(*G->Private->RemoveVertex)(G,Index);
	
	G->NumVertices--;
	for (i=Index;i<G->NumVertices;i++) G->Vertices[i]=G->Vertices[i+1];

	ptr=realloc(G->Vertices,sizeof(struct Graph_Vertex *) * G->NumVertices);
	if (ptr) G->Vertices=ptr;		/* ignore the error - it is not critical since we're shrinking, not growing */

	return 0;
}

int FindVertex(struct Graph * G, void * Ptr, int Num)
	/* Scans the vertices in a graph for a vertex with the relavent Tag.
	   If Ptr==NULL, then Num is used as the search key. Otherwise Ptr is used.
	   Care must be taken so that a graph does not contain vertices which have
	   a mixture of Num and Ptr tags. Simply... don't do it.
	   Returns index of vertex >=0 or an error <0 (GRAPH_BADPARAM, GRAPH_NOTFOUND)
	*/
{
	int i;

	if (!G) return GRAPH_BADPARAM;

	/* note: this is faster than a single for loop which checks Ptr, and then
       does the suitable comparison. Why? Simply because it halves the number
	   of comparisons performed per iteration. For small graphs, the improvement
	   is unnoticable. For a very large graph, with a good compiler, this could
	   take half the time
	*/
	if (Ptr)
	{
		for (i=0;i<G->NumVertices;i++)
		{
			if (G->Vertices[i]->Tag.Ptr==Ptr) return i;
		}
	} else
	{
		for (i=0;i<G->NumVertices;i++)
		{
			if (G->Vertices[i]->Tag.Num==Num) return i;
		}
	}
		
	return GRAPH_NOTFOUND;	
}

/* -------------- */
/* Managing Edges */
/* -------------- */

int ConnectVertex(struct Graph * G, int Source, int Destination, int Cost)
	/* Make an edge from Source to Destination of specified cost.
	   Cost must be != GRAPH_NOTCONNECTED
	   Returns 0 on success, or <0 on error (GRAPH_BADPARAM).
	   If Source is already connected to Destination, then the edges cost is updated to Cost
	*/
{
	if (Cost==GRAPH_NOTCONNECTED || !G || Source<0 || Destination<0 || Source>=G->NumVertices || Destination>=G->NumVertices) return GRAPH_BADPARAM;
	return (*G->Private->ConnectVertex)(G,Source,Destination,Cost);
}

int DisconnectVertex(struct Graph * G, int Source, int Destination) 
	/* Disconects an edge from Source to Destination. If Source is not connected to
	   Destination, then 0 (success) is returned.
	   Returns 0 on success, or <0 on error (GRAPH_BADPARAM)
	*/
{
	if (!G || Source<0 || Destination<0 || Source>=G->NumVertices || Destination>=G->NumVertices) return GRAPH_BADPARAM;
	return (*G->Private->DisconnectVertex)(G,Source,Destination);
}

int EdgeScanStart(struct Graph * G, int Index, struct EdgeScan * EScan)
	/* Initialises CScan in preparation to scan through the edges for vertex
	   denoted by Index. EdgeScanNext can now be used to retrieve edge info
	   Returns 0 on success, or <0 on error (GRAPH_BADPARAM)
	*/
{
	if (!G || !EScan || Index<0 || Index>=G->NumVertices) return GRAPH_BADPARAM;
	
	EScan->G=G;
	EScan->Source=Index;
	EScan->Dest=-1;
	EScan->Cost=GRAPH_NOTCONNECTED;

	return (*G->Private->EdgeScanStart)(G,Index,EScan);
}

int EdgeScanEnd(struct EdgeScan * EScan)
	/* Does the opposite of an initialisation. It frees up any memory that
	   may have been allocated in order to perform the scan.
	   Returns 0 on success, or <0 on error (GRAPH_BADPARAM)
	*/
{
	if (!EScan || !EScan->G) return GRAPH_BADPARAM;
	return (*EScan->G->Private->EdgeScanEnd)(EScan);
}

int EdgeScanNext(struct EdgeScan * EScan)
	/* Retrieves information about an edge, putting it in CScan.
	   0 is returned on success. <0 is returned on error (GRAPH_BADPARAM)
	   >0 is returned when there are no more edges.
	*/
{
	if (!EScan || !EScan->G) return GRAPH_BADPARAM;
	return (*EScan->G->Private->EdgeScanNext)(EScan);
}

