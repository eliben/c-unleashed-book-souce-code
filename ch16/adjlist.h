/* adjlist.h */

#ifndef ADJLIST_H
#define ADJLIST_H

#include "graphprv.h"

struct Graph_AdjList {
	int Cost;
	int Destination;
	struct Graph_AdjList * Next;
};

extern struct Graph_Spec AdjList_Spec;

#endif
