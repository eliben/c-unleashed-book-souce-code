/* pqueue.h */

#ifndef PQUEUE_H
#define PQUEUE_H

#include "graphs.h"

/* a prioritesed queue for vertices */

struct PQueue {
	struct PQueue_Node * Front;
};

struct PQueue_Node {
	int Weight;
	int Index;
	struct PQueue_Node * Next;
};

void PQ_Initialise(struct PQueue * Q);
void PQ_Free(struct PQueue * Q);
int PQ_Enqueue(struct PQueue * Q, int Weight, int Index);
int PQ_Dequeue(struct PQueue * Q);

/* a prioritesed queue for edges */

struct PEdgeQueue {
	struct PEdgeQueue_Node * Front;
};

struct PEdgeQueue_Node {
	int Weight;
	int Source;
	int Dest;
	struct PEdgeQueue_Node * Next;
};

void PEQ_Initialise(struct PEdgeQueue * Q);
void PEQ_Free(struct PEdgeQueue * Q);
int PEQ_Enqueue(struct PEdgeQueue * Q, struct EdgeScan * EScan);
int PEQ_Dequeue(struct PEdgeQueue * Q, struct EdgeScan * EScan);

#endif
