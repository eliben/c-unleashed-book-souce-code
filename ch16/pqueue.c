#include "pqueue.h"
#include "graphs.h"
#include <stdlib.h>

void PQ_Initialise(struct PQueue * Q)
{
	Q->Front=NULL;
}

void PQ_Free(struct PQueue * Q)
{
	struct PQueue_Node * node;

	if (!Q->Front) return;

	while (Q->Front)
	{
		node=Q->Front->Next;
		free(Q->Front);
		Q->Front=node;
	}
}

int PQ_Enqueue(struct PQueue * Q, int Weight, int Index)
{
	struct PQueue_Node ** nodeptr;
	struct PQueue_Node *scannode, *newnode;

	/* check to see if Index is already present */
	nodeptr=&Q->Front;
	newnode=Q->Front;
	while (newnode)
	{
		if (newnode->Index==Index)
		{
			*nodeptr=newnode->Next;
			break;
		}
		nodeptr=&newnode->Next;
		newnode=newnode->Next;
	}
	if (!newnode)
	{
		newnode=malloc(sizeof(struct PQueue_Node));
		if (!newnode) return GRAPH_OUTOFMEM;
	}
	
	/* here, newnode is valid, uninitialised and not present in the queue regardless */
	newnode->Weight=Weight;
	newnode->Index=Index;

	/* now insert newnode into the queue */
	scannode=Q->Front;
	nodeptr=&Q->Front;
	while (scannode) {
		if (scannode->Weight>Weight)
		{
			/* insert newnode before scannode */
			newnode->Next=scannode;
			*nodeptr=newnode;
			return 0;
		}
		nodeptr=&scannode->Next;
		scannode=scannode->Next;
	}
	
	/* if we reached the end of the list, then insert newnode after scannode */
	newnode->Next=NULL;
	*nodeptr=newnode;

	return 0;
}

int PQ_Dequeue(struct PQueue * Q)
{
	/* simply, return >0 if Q->Front==NULL, otherwise the index of the first node (and remove it) */
	int retindex;
	struct PQueue_Node * node;

	if (!Q->Front) return -1;

	node=Q->Front;
	Q->Front=node->Next;

	retindex=node->Index;
	free(node);

	return retindex;	
}	

void PEQ_Initialise(struct PEdgeQueue * Q)
{
	Q->Front=NULL;
}

void PEQ_Free(struct PEdgeQueue * Q)
{
	struct PEdgeQueue_Node * node;

	if (!Q->Front) return;

	while (Q->Front)
	{
		node=Q->Front->Next;
		free(Q->Front);
		Q->Front=node;
	}
}

int PEQ_Enqueue(struct PEdgeQueue * Q, struct EdgeScan * EScan)
{
	struct PEdgeQueue_Node ** nodeptr;
	struct PEdgeQueue_Node *scannode, *newnode;

	/* check to see if Index is already present */
	nodeptr=&Q->Front;
	newnode=Q->Front;
	while (newnode)
	{
		if (newnode->Source==EScan->Source && newnode->Dest==EScan->Dest)
		{
			*nodeptr=newnode->Next;
			break;
		}
		nodeptr=&newnode->Next;
		newnode=newnode->Next;
	}
	if (!newnode)
	{
		newnode=malloc(sizeof(struct PEdgeQueue_Node));
		if (!newnode) return GRAPH_OUTOFMEM;
	}
	
	/* here, newnode is valid, uninitialised and not present in the queue regardless */
	newnode->Weight=EScan->Cost;
	newnode->Source=EScan->Source;
	newnode->Dest=EScan->Dest;

	/* now insert newnode into the queue */
	scannode=Q->Front;
	nodeptr=&Q->Front;
	while (scannode) {
		if (scannode->Weight>EScan->Cost)
		{
			/* insert newnode before scannode */
			newnode->Next=scannode;
			*nodeptr=newnode;
			return 0;
		}
		nodeptr=&scannode->Next;
		scannode=scannode->Next;
	}
	
	/* if we reached the end of the list, then insert newnode after scannode */
	newnode->Next=NULL;
	*nodeptr=newnode;

	return 0;
}

int PEQ_Dequeue(struct PEdgeQueue * Q, struct EdgeScan * EScan)
{
	struct PEdgeQueue_Node * node;

	if (!Q->Front) return -1;

	node=Q->Front;
	Q->Front=node->Next;

	EScan->Source=node->Source;
	EScan->Dest=node->Dest;
	EScan->Cost=node->Weight;

	free(node);
	return 0;
}
