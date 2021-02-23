#include "funcs.h"

edge* makeEdge(PathNode* a, PathNode* b, float distance, bool oneDirectional)
{
	auto newEdge = new edge(a, b, distance, oneDirectional);
	return newEdge;
}
