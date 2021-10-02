/* shpath.c */

#include <stdlib.h>
#include "shpath.h"
#include "pqueue.h"

#define TRUE	-1
#define FALSE	0

void Dijkstra_InitTable(struct Dijkstra_Table * Table)
	/* Initialises a table. It simply sets all the members to meaningful empty
	   values. */
{
	if (!Table) return;

	Table->G=NULL;
	Table->Source=-1;
	Table->Results=NULL;
}

void Dijkstra_FreeTable(struct Dijkstra_Table * Table)
	/* Frees memory occupied by the results (and not Table itself) */
{
	if (!Table) return;

	free(Table->Results);
	Table->Results=NULL;
}

int InitResults(struct Dijkstra_Row ** Results, int NumRows)
{
  int i;
  struct Dijkstra_Row * ptr;

  ptr=malloc(sizeof(struct Dijkstra_Row) * NumRows);
  if (!ptr) return GRAPH_OUTOFMEM;
	
  for (i=0;i<NumRows;i++)
  {
    ptr[i].Total=GRAPH_NOTCONNECTED;
    ptr[i].Previous=-1;
    ptr[i].Visited=FALSE;
  }

  *Results=ptr;
  return 0;
}

int Dijkstra_Simple(struct Graph * G, int Source,
                               struct Dijkstra_Table * Table)
{
  int V;	/* V is the index of the current vertex */
  int C;	/* C is used in stage 4, the lowest cost found */
  int i;
  struct Dijkstra_Row * Results;
  struct EdgeScan E;    /* holds information about an edge */

  i=InitResults(&Results, G->NumVertices);
  if (i) return i;

  Results[Source].Total=0;      /* Step 1 */
  Results[Source].Previous=-1;	/* no previous node */
  V=Source;
	
  do {
    Results[V].Visited=TRUE;    /* Step 2 */
	
    EdgeScanStart(G, V, &E);    /* Step 3 */
	while (EdgeScanNext(&E)==0)
    {
      if (Results[E.Dest].Visited) continue;
      if (E.Cost<0) return GRAPH_BADGRAPH;

      if (Results[V].Total + E.Cost < Results[E.Dest].Total )
      {
        Results[E.Dest].Total = Results[V].Total + E.Cost;
        Results[E.Dest].Previous=V;
      }
    }
	EdgeScanEnd(&E);
	
    V=-1;                       /* Step 4 */
    C=GRAPH_NOTCONNECTED;
    for (i=0;i<G->NumVertices;i++)
    {
      if (!Results[i].Visited && Results[i].Total<C)
      {
        V=i;
        C=Results[i].Total;
      }
    }
  } while (V!=-1);

  Table->G=G;
  Table->Source=Source;
  Table->Results=Results;

  return 0;
}

int Dijkstra_Sparse(struct Graph * G, int Source, struct Dijkstra_Table * Table)
{
  int V;	/* V is the index of the current vertex */
  int i;
  struct PQueue Q;
  struct Dijkstra_Row * Results;
  struct EdgeScan E;    /* holds information about an edge */

  PQ_Initialise(&Q);
  i=InitResults(&Results, G->NumVertices);
  if (i) return i;	/* report any errors */

  Results[Source].Total=0;       /* Step 1 */
  Results[Source].Previous=-1;	/* no previous node */
  V=Source;
	
  do {
    Results[V].Visited=TRUE;    /* Step 2 */
	
    EdgeScanStart(G, V, &E);    /* Step 3 */
	while (EdgeScanNext(&E)==0)
    {
      if (Results[E.Dest].Visited) continue;
      if (E.Cost<0) return GRAPH_BADGRAPH;

      if (Results[V].Total + E.Cost < Results[E.Dest].Total )
      {
        Results[E.Dest].Total = Results[V].Total + E.Cost;
        Results[E.Dest].Previous=V;
		PQ_Enqueue(&Q,Results[E.Dest].Total,E.Dest);
      }
    }
	EdgeScanEnd(&E);
	
    V=PQ_Dequeue(&Q);           /* Step 4 */
  } while (V!=-1);

  Table->G=G;
  Table->Source=Source;
  Table->Results=Results;

  return 0;
}

int Bellman(struct Graph * G, int Source, struct Dijkstra_Table * Table)
{
  int V;	/* V is the index of the current vertex */
  int i;
  struct PQueue Q;
  struct Dijkstra_Row * Results;
  struct EdgeScan E;    /* holds information about an edge */

  PQ_Initialise(&Q);
  i=InitResults(&Results, G->NumVertices);
  if (i) return i;	/* report any errors */

  Results[Source].Total=0;       /* Step 1 */
  Results[Source].Previous=-1;	/* no previous node */
  V=Source;
	
  do {
    Results[V].Visited++;       /* Step 2 */
	if (Results[V].Visited==G->NumVertices) return GRAPH_BADGRAPH;
    /* if we have visited a vertex once for each vertex, 
       then we must be in a negatively weighted cycle. Return
       an error to prevent infinite loop
    */
	
    EdgeScanStart(G, V, &E);    /* Step 3 */
	while (EdgeScanNext(&E)==0)
    {
      if (Results[V].Total + E.Cost < Results[E.Dest].Total )
      {
        Results[E.Dest].Total = Results[V].Total + E.Cost;
        Results[E.Dest].Previous=V;
		PQ_Enqueue(&Q,Results[E.Dest].Total,E.Dest);
      }
    }
	EdgeScanEnd(&E);
	
    V=PQ_Dequeue(&Q);           /* Step 4 */

  } while (V!=-1);

  Table->G=G;
  Table->Source=Source;
  Table->Results=Results;

  return 0;
}

void Floyd_InitTable(struct Floyd_Table * Table)
	/* Initialises the given table */
{
	if (!Table) return;

	Table->G=NULL;
	Table->Results=NULL;
}

void Floyd_FreeTable(struct Floyd_Table * Table) 
	/* Frees the memory used be results in the table. Does not free Table itself */
{
	int i;
	if (!Table) return;
	
	for (i=0;i<Table->G->NumVertices;i++) free(Table->Results[i]);
	free(Table->Results);
}

struct Floyd_Result ** MakeResults(int N)
{
	int i,j;
	struct Floyd_Result ** R;

	if (!N) return NULL;

	R=malloc(sizeof(struct Floyd_Result *)*N);
	if (!R) return NULL;

	for (i=0;i<N;i++)
    {
		R[i]=malloc(sizeof(struct Floyd_Result)*N);
		if (!R) {
			/* Clean up the mess */

			for (j=0;j<i;j++) free(R[j]);
			free(R);
			return NULL;
		}

		for (j=0;j<N;j++)
        {
			R[i][j].Total= (i==j? 0 : GRAPH_NOTCONNECTED);
			R[i][j].Previous=-1;
		}
	}			

	return R;	
}

int Floyd(struct Graph * G,struct Floyd_Table * Table)
	/* Uses Floyds algorithm to calculated the shortest path for all pairs of vertices in G.
	   Returns 0 on succes, or <0 on error (GRAPH_OUTOFMEM, GRAPH_BADPARAM)
	   Passing a graph containing a negative cost cycle will produce spurious results
	   but will not result in segmentation fault, infinite loops or similar
	*/
{
  int i,j,k;

  /* for every pair of vertices i and j find the shortest path
     D[i,j]. D[i,j] is determined as the minimum of D[i,j] and
     the path through every intermediate vertex k. That is
     min( D[i,j], D[i,k]+D[k,j] )
  */

  struct Floyd_Result **  R;
  struct EdgeScan EScan;

  if (!G || !Table || !G->NumVertices) return GRAPH_BADPARAM;

  /* Create the 2 dimensional array of results */
  R=MakeResults(G->NumVertices);
  if (!R) return GRAPH_OUTOFMEM;

  /* Initialise the results table */
  for (i=0;i<G->NumVertices;i++)
  {
    EdgeScanStart(G,i,&EScan);
    while (EdgeScanNext(&EScan)==0)
    {
      R[i][EScan.Dest].Total=EScan.Cost;
    }
    EdgeScanEnd(&EScan);
  }

  for (k=0;k<G->NumVertices;k++)
    for (i=0;i<G->NumVertices;i++)
      for (j=0;j<G->NumVertices;j++)
      {
        /* Because, in practice, GRAPH_NOTCONNECTED is INT_MAX we must ensure we do not overflow */
        if (R[i][k].Total==GRAPH_NOTCONNECTED || R[k][j].Total==GRAPH_NOTCONNECTED) continue;
        if (R[i][j].Total > R[i][k].Total + R[k][j].Total)
        {
          R[i][j].Total=R[i][k].Total + R[k][j].Total;
          R[i][j].Previous=k;
        }
      }
          
  Table->G=G;
  Table->Results=R;

  return 0;
}

#undef TRUE
#undef FALSE


