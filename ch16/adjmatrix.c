/* adjlist.c */

#include "adjmatrix.h"
#include <stdlib.h>

int  AdjMatrix_MakeGraph(struct Graph * G)
{
	/* nothing special needs to be done */
	return 0;
}

void AdjMatrix_FreeGraph(struct Graph * G)
{
	/* free everything except G and G->Vertices */
	int i;

	for (i=0;i<G->NumVertices;i++)
	{
		free(G->Vertices[i]->Edges.Matrix);
		free(G->Vertices[i]);
	}
}

int AdjMatrix_AddVertex(struct Graph * G, int Index)
{
	int i, *matrix;

	/* for each vertex (except the last one), add the new vertexes cost */
	for (i=0;i<G->NumVertices-1;i++)
	{
		matrix=realloc(G->Vertices[i]->Edges.Matrix,sizeof(int)*G->NumVertices);
		if (!matrix) return GRAPH_OUTOFMEM;
		matrix[G->NumVertices-1]=GRAPH_NOTCONNECTED;
		G->Vertices[i]->Edges.Matrix=matrix;
	}
	
	/* for the last vertex, create and initialise it's matrix row */
	matrix=malloc(sizeof(int)*G->NumVertices);
	if (!matrix) return GRAPH_OUTOFMEM;
	
	for (i=0;i<G->NumVertices;i++) matrix[i]=GRAPH_NOTCONNECTED;
	G->Vertices[G->NumVertices-1]->Edges.Matrix=matrix;
	
	return 0;
}

void AdjMatrix_RemoveVertex(struct Graph * G, int Index)
{
	int i,j,*matrix;

	/* for each vertex, except Index, adjust all matrix rows */
	for (i=0;i<G->NumVertices;i++)
	{
		if (i==Index) continue;
		for (j=Index;j<G->NumVertices-1;j++) G->Vertices[i]->Edges.Matrix[j]=G->Vertices[i]->Edges.Matrix[j+1];

		matrix=realloc(G->Vertices[i]->Edges.Matrix,sizeof(int)*G->NumVertices-1);
		if (matrix) G->Vertices[i]->Edges.Matrix=matrix;
			/* if realloc errored, then we haven't resized, but it should all still function */
	}

	free(G->Vertices[Index]->Edges.Matrix);
}

int  AdjMatrix_ConnectVertex(struct Graph * G, int Source, int Destination, int Cost)
{
	G->Vertices[Source]->Edges.Matrix[Destination]=Cost;
	return 0;
}

int  AdjMatrix_DisconnectVertex(struct Graph * G, int Source, int Destination)
{
	G->Vertices[Source]->Edges.Matrix[Destination]=GRAPH_NOTCONNECTED;
	return 0;
}

int  AdjMatrix_EdgeScanStart(struct Graph * G, int Index, struct EdgeScan * EScan)
{
	EScan->Internal.Index=0;
	return 0;
}

int  AdjMatrix_EdgeScanEnd(struct EdgeScan * EScan)
{
	EScan->Internal.Index=-1;
	return 0;
}

int  AdjMatrix_EdgeScanNext(struct EdgeScan * EScan)
{

	while (EScan->Internal.Index<EScan->G->NumVertices && EScan->G->Vertices[EScan->Source]->Edges.Matrix[EScan->Internal.Index]==GRAPH_NOTCONNECTED) EScan->Internal.Index++;
	if (EScan->Internal.Index==EScan->G->NumVertices) return 1;

	EScan->Dest=EScan->Internal.Index;
	EScan->Cost=EScan->G->Vertices[EScan->Source]->Edges.Matrix[EScan->Dest];
	EScan->Internal.Index++;

	return 0;
}

struct Graph_Spec AdjMatrix_Spec=
{
	AdjMatrix_MakeGraph,
	AdjMatrix_FreeGraph,
	AdjMatrix_AddVertex,
	AdjMatrix_RemoveVertex,
	AdjMatrix_ConnectVertex,
	AdjMatrix_DisconnectVertex,
	AdjMatrix_EdgeScanStart,
	AdjMatrix_EdgeScanEnd,
	AdjMatrix_EdgeScanNext
};

