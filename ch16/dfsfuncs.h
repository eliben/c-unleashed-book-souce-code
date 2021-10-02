/* dfsfuncs.h */

#ifndef DFSFUNCS_H
#define DFSFUNCS_H

#include "graphs.h"

int AreVerticesConnected(struct Graph * G,int Source, int Dest);
	/* determines whether there is a path from Source to Dest
	   It should work for directed and undirected graphs. Cost
	   is not determined - only that a path exists. Because a
	   depth first search is used, upto G->NumVertices iterations
	   are required to determine that there is a path
	   returns <0 on error (GRAPH_OUTOFMEM,GRAPH_BADPARAM)
	   0 denotes no path, >0 denotes a path
	*/
	    
#endif
