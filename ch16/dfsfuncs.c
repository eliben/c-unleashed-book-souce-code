/* dfsfuncs.c */

#include "dfsfuncs.h"
#include <stdlib.h>

/* An implementation of a depth-first search to determine if there is a
   path (of any length) from S to D. Visited is required to avert infinitely
   moving around cycles, must be sizeof(int)*NumberOfVertices), and be
   initialised with zeros before first executing.
*/
int AVC_Inner(struct Graph * G, int S, int D,int * Visited)
{
	int retval;
	struct EdgeScan EScan;

	retval=0;
	Visited[S]=1;
	EdgeScanStart(G,S,&EScan);
	while (EdgeScanNext(&EScan)==0 && retval==0)
	{
		if (EScan.Dest==D) retval=0;
		else if (!Visited[EScan.Dest]) retval=AVC_Inner(G,EScan.Dest,D,Visited);
	}
	EdgeScanEnd(&EScan);
	return retval;
}

int AreVerticesConnected(struct Graph * G,int Source, int Dest)
{
	int * Visited;
	int i, retval;

	if (!G || Source<0 || Dest<0 || Source>=G->NumVertices || Dest>=G->NumVertices) return GRAPH_BADPARAM;

	Visited=malloc(sizeof(int)*G->NumVertices);
	if (!Visited) return GRAPH_OUTOFMEM;
	for (i=0;i<G->NumVertices;i++) Visited[i]=0;	

	retval=AVC_Inner(G,Source,Dest,Visited);
	free(Visited);

	return retval;
}

