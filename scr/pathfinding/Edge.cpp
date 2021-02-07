

#include "edge.h"
#include "Edge.h"

void insert(PathNode* a, PathNode* b, edge* e) {
	a->edges.insert(e);
	b->edges.insert(e);
}

edge::edge(PathNode* a, PathNode* b) {
	// create an edge betwean 2 nodes that are side by side
	this->nodes = std::make_pair(a, b);
	this->length = distance::diagonal(a, b);
}
edge::edge(PathNode* a, PathNode* b, float& distance) {
	this->length = distance;
	this->nodes = std::make_pair(a, b);
	insert(a, b, this);
}
edge::edge(PathNode* a, PathNode* b, bool intra) {
	this->connectsClusters = intra;
	this->nodes = std::make_pair(a, b);
	this->length = 1;
	insert(a, b, this);
}

edge::~edge() {
	this->removeFromNode(this->nodes.first);
	this->removeFromNode(this->nodes.second);
}

void edge::removeFromNode(PathNode* a) {
	a->edges.erase(this);
}