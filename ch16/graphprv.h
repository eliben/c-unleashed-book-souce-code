/* graphprivate.h */

/* This header file defines internal (i.e. private) structures. This
   is only the Graph_Spec structure - it defines the functions to call
   from the functions defined in graph.h which are representation
   specific. Each representation should have a Graph_Spec structure
   defined extern in it's header file, and fully defined in the
   accompanying c file.
*/

#ifndef GRAPHPRIVATE_H
#define GRAPHPRIVATE_H

#include "graphs.h"

struct Graph_Spec
{
	int  (*MakeGraph)       (struct Graph *);
	void (*FreeGraph)       (struct Graph *);
    int  (*AddVertex)       (struct Graph *, int Index);
	void (*RemoveVertex)    (struct Graph *, int Index);
	int  (*ConnectVertex)   (struct Graph *, int Source, int Destination, int Cost);
	int  (*DisconnectVertex)(struct Graph *, int Source, int Destination);
	int  (*EdgeScanStart)   (struct Graph *, int Index, struct EdgeScan *);
	int  (*EdgeScanEnd)     (struct EdgeScan *);
	int  (*EdgeScanNext)    (struct EdgeScan *);
};

#endif
