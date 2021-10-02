/* adjlist.c */

#include "adjlist.h"
#include <stdlib.h>

int  AdjList_MakeGraph(struct Graph * G)
{
	/* nothing special needs to be done */
	return 0;
}

void AdjList_FreeGraph(struct Graph * G)
{
	/* free everything except G and G->Vertices */
	
	int i;

	for (i=0;i<G->NumVertices;i++)
	{
		struct Graph_AdjList * node;
		void * freenode;

		node=G->Vertices[i]->Edges.List;
		while (node)
		{
			freenode=node;
			node=node->Next;
			free(freenode);
		}

		free(G->Vertices[i]);
	}
}

int AdjList_AddVertex(struct Graph * G, int Index)
{
	G->Vertices[Index]->Edges.List=NULL;
	return 0;
}

void AdjList_RemoveVertex(struct Graph * G, int Index)
{
	/* we must adjust the index values of all connections > Index. This is slow */
	int i;
	for (i=0;i<G->NumVertices;i++)
	{
		struct Graph_AdjList * node;

		node=G->Vertices[i]->Edges.List;
		while (node)
		{
			if (node->Destination>Index) node->Destination--;
			node=node->Next;
		}
	}
}

int  AdjList_ConnectVertex(struct Graph * G, int Source, int Destination, int Cost)
{
	/* check if Source is already connected to Destination */
	struct Graph_AdjList * node;

	node=G->Vertices[Source]->Edges.List;
	while (node)
	{
		if (node->Destination==Destination) {
			node->Cost=Cost;
			return 0;
		}
		node=node->Next;
	}

	/* Source is not connected to Destination, so we must add a new node */
	node=malloc(sizeof(struct Graph_AdjList));
	if (!node) return GRAPH_OUTOFMEM;

	node->Cost=Cost;
	node->Destination=Destination;
	node->Next=G->Vertices[Source]->Edges.List;
	G->Vertices[Source]->Edges.List=node;

	return 0;
}

int  AdjList_DisconnectVertex(struct Graph * G, int Source, int Destination)
{
	struct Graph_AdjList * node;
	struct Graph_AdjList ** nodeptr;

	node=G->Vertices[Source]->Edges.List;
	nodeptr=&G->Vertices[Source]->Edges.List;

	while (node)
	{
		if (node->Destination==Destination)
		{
			*nodeptr=node->Next;
			free(node);
			return 0;
		}
		nodeptr=&node->Next;
		node=node->Next;
	}
	return 0;
}

int  AdjList_EdgeScanStart(struct Graph * G, int Index, struct EdgeScan * EScan)
{
	EScan->Internal.Ptr=G->Vertices[Index]->Edges.List;
	return 0;
}

int  AdjList_EdgeScanEnd(struct EdgeScan * EScan)
{
	EScan->Internal.Ptr=NULL;
	return 0;
}

int  AdjList_EdgeScanNext(struct EdgeScan * EScan)
{
	struct Graph_AdjList * node;

	node=EScan->Internal.Ptr;
	if (!node) return INT_MAX;		/* >0.... denotes finished */
	
	EScan->Cost=node->Cost;
	EScan->Dest=node->Destination;
	EScan->Internal.Ptr=node->Next;

	return 0;
}

struct Graph_Spec AdjList_Spec=
{
	AdjList_MakeGraph,
	AdjList_FreeGraph,
	AdjList_AddVertex,
	AdjList_RemoveVertex,
	AdjList_ConnectVertex,
	AdjList_DisconnectVertex,
	AdjList_EdgeScanStart,
	AdjList_EdgeScanEnd,
	AdjList_EdgeScanNext
};

