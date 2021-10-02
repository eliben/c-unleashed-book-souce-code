/* topsort.c

Implementation of a topological sort */

#include "topsort.h"
#include <stdlib.h>

#define Enqueue(zze) {queue[last]=zze;last++;}
#define Dequeue(zze) {zze=queue[first];first++;}

void InitIndegreeTable(struct Graph * G, int * itable)
{
  /*
    build an indegree table from graph G
    itable must be at least G->NumVertices in length
  */
	
  int i;
  struct EdgeScan E;

  for (i=0;i<G->NumVertices;i++) {
    EdgeScanStart(G,i,&E);
    while (EdgeScanNext(&E)==0)
    {
      itable[E.Dest]++;
    }
    EdgeScanEnd(&E);
  }
}

int TopologicalSort(struct Graph * G, int ** sorted)
{
  /*
    Perform a topological sort on graph G, creating
    an array of G->NumVertices integers and returning
    it in *sorted.

    Returns 0 on success,
    <0 on error (GRAPH_BADPARAM, GRAPH_OUTOFMEM, GRAPH_BADGRAPH)
  */
	
  int first, last, i;
  int * queue;
  int * itable;
  struct EdgeScan E;

  if (!G || !sorted) return GRAPH_BADPARAM;

  queue=malloc(sizeof(int)*G->NumVertices);
  itable=malloc(sizeof(int)*G->NumVertices);
  if (!queue || !itable)
  {
    free(queue);free(itable);
    return GRAPH_OUTOFMEM;
  }

  InitIndegreeTable(G,itable);
  last=0;first=0;

  /* search for vertices with indegree 0 */
  for (i=0;i<G->NumVertices;i++) 
  {
    if (itable[i]==0) Enqueue(i);
  }

  /* while there are still vertices with indegree 0... */
  while (last!=first)						
  {
    Dequeue(i);
    EdgeScanStart(G,i,&E);
    while (EdgeScanNext(&E)==0)			
    {
      /* decrement the indegree of vertices adjacent to them */
      
      itable[E.Dest]--;
      if (itable[E.Dest]==0) Enqueue(E.Dest);
    }
    EdgeScanEnd(&E);
  }

  free(itable);

  /* if we haven't dequeued G->NumVertices elements, we have a cyclic graph */
  if (first!=G->NumVertices)				
  {
    free(queue);
    return GRAPH_BADGRAPH;
  }

  *sorted=queue;
  return 0;
}

