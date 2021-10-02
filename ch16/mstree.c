/* mstree.c */

#include "graphs.h"
#include "mstree.h"
#include "dfsfuncs.h"
#include "pqueue.h"
#include <stdlib.h>

#define TRUE -1
#define FALSE 0

#define SWAP(a,b,type) {type temp; temp=a; a=b; b=temp;}

int Kruskal_Undirected(struct Graph * G,struct Graph ** TreePtr)
{
  /* The graph G is examined to produce a new graph, which is
     a minimum spanning tree and the struct Graph * denoted
     by TreePtr is assigned the new value.
     return 0 on success, <0 on error (GRAPH_BADPARAM, GRAPH_OUTOFMEM)
  */

  int i, j, numadded;
  struct Graph * Tree;
  struct PEdgeQueue * Q;
  struct EdgeScan EScan;

  if (!G || !TreePtr) return GRAPH_BADPARAM;

  PEQ_Initialise(Q);
  Tree=MakeGraph(List);
  if (!Tree) return GRAPH_OUTOFMEM;

  /* step 1 and 2: order the edges in the graph by cost
     also, duplicate the vertices in G into Tree
     (only because it's generally faster to have one
     loop doing 2 things, than 2 loops doing 1)
  */
	
  for (i=0;i<G->NumVertices;i++)
  {
    j=AddVertex(Tree);
    if (j<0)
    {
      /* error, clean up and return */
      PEQ_Free(Q);
      FreeGraph(Tree);
      return j;
    }
    Tree->Vertices[j]->Tag=G->Vertices[i]->Tag;

    EdgeScanStart(G,i,&EScan);
    while (!EdgeScanNext(&EScan))
    {
      /* adjust EScan such that we do not repeat the reverse
         edge since we're dealing with undirected graphs only
         This means that we only use half the required memory,
         and we only ever traverse any edge once. The cost
         is the processing we need to do here.
      */
      if (EScan.Source>EScan.Dest) SWAP(EScan.Source,EScan.Dest,int);

      if (PEQ_Enqueue(Q,&EScan))
      {
        /* error - out of memory. Clean up and return */
        PEQ_Free(Q);
        FreeGraph(Tree);
        return GRAPH_OUTOFMEM;
      }
    }
    EdgeScanEnd(&EScan);
  }

  numadded=0;

  /* step 3, select the next edge in the list */
  while (numadded<G->NumVertices-1 && !PEQ_Dequeue(Q,&EScan))
  {

	/* step 4, if the current edge does not form a cycle
	   add the current edge to the tree
    */

    if (!AreVerticesConnected(Tree,EScan.Source,EScan.Dest))
    {
      /* if there is no path from source to dest, then the current edge will not make a cycle when added */
      j=ConnectVertex(Tree,EScan.Source,EScan.Dest,EScan.Cost);
      if (!j) j=ConnectVertex(Tree,EScan.Dest,EScan.Source,EScan.Cost);
      if (j)
      {
        /* an error occured, clean up and return */
        PEQ_Free(Q);
        FreeGraph(Tree);
        return j;
      }
      numadded++;
    }
  }

  *TreePtr=Tree;
  PEQ_Free(Q);

  return 0;
}

struct Prim_Working {
	int Visited;			/* flag denoting if a vertex has been accepted into the solution       */
	int LowestCost;			/* the value of the lowest cost edge found so far                      */
	int Prev;				/* the index of the vertex which the lowest cost edge found comes from */
};


int Prim_Undirected(struct Graph * G,struct Graph ** TreePtr)
{
  /* The graph G is examined to produce a new graph, which is
     a minimum spanning tree and the struct Graph * denoted
     by TreePtr is assigned the new value.
     return 0 on success, <0 on error (GRAPH_BADPARAM, GRAPH_OUTOFMEM)
  */

  struct Graph * Tree;
  struct Prim_Working * W;
  struct EdgeScan EScan;
  int NumVisited, V, i, j, lowest;

  if (!G || !TreePtr) return GRAPH_BADPARAM;

  Tree=MakeGraph(List);
  if (!Tree) return GRAPH_OUTOFMEM;

  W=malloc(sizeof(struct Prim_Working)*G->NumVertices);
  if (!W)
  {
    FreeGraph(Tree);
    return GRAPH_OUTOFMEM;
  };

  for (i=0;i<G->NumVertices;i++)
  {
    W[i].Visited=FALSE;
    W[i].LowestCost=GRAPH_NOTCONNECTED;
    W[i].Prev=-1;

    j=AddVertex(Tree);
    if (j<0)
    {
      /* error, clean up and return */
      FreeGraph(Tree);
      free(W);
      return j;
    }
    Tree->Vertices[j]->Tag=G->Vertices[i]->Tag;
  }

  NumVisited=0;

  /* Tree is an empty graph,
	 Visited is an array of integers G->NumVertices long initially zeroed */

  /* step 1, select a root vertex V*/
  V=0;

  for (;;)
  {
    /* step 2, mark V as visited */
    W[V].Visited=TRUE;
    NumVisited++;

    if (NumVisited==G->NumVertices) break;		/* we're finished */

    /* step 3. for every vertex adjacent to V, adjust
       the lowest edge cost
    */

    EdgeScanStart(G,V,&EScan);
    while (!EdgeScanNext(&EScan))
    {
      if (EScan.Cost<W[EScan.Dest].LowestCost)
      {
        W[EScan.Dest].LowestCost=EScan.Cost;
        W[EScan.Dest].Prev=V;
      }
    }
    EdgeScanEnd(&EScan);

    /* step 4, select the vertex with the lowest cost
       edge which has not been visited and add the edge
       to the minimum spanning tree
    */

	lowest=GRAPH_NOTCONNECTED;V=-1;
    for (i=0;i<G->NumVertices;i++)
    {
      if (W[i].Visited) continue;

      if (V==-1) V=i;                          
      if (W[i].LowestCost<lowest)
      {
        lowest=W[i].LowestCost;
        V=i;
      }
    }

	if (W[V].Prev!=-1)
	{
		i=ConnectVertex(Tree,V,W[V].Prev,W[V].LowestCost);
		if (!i) i=ConnectVertex(Tree,W[V].Prev,V,W[V].LowestCost);
		if (i)
		{
			/* error occurred, tidy up and return */
            free(W);
            FreeGraph(Tree);
            return i;
        }
    }

    /* The first 'V=i' in the for loop above is used to preserve
       the index of a vertex which has not been visited. This is
       to neatly handle the situation where all the visited 
       vertices do not have any edges to any vertices which have
       not been visited (that is, the graph is unconnected). If
       the graph is unconnected, then V is set to an unvisited
       vertex and a minimum spanning forest is made instead of a
       minimum spanning tree. If it is connected, then V would be
       the index of the vertex with the lowest cost edge known.
    */
  }

  free(W);
  *TreePtr=Tree;

  return 0;
}

#undef TRUE
#undef FALSE
