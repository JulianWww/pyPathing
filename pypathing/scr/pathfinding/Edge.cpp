

#include "edge.h"
#include "Edge.h"
#include "node.h"
#include "Cluster.h"
#include "updateEvent.h"

void insert(PathNode* a, PathNode* b, edge* e) {
	a->edges.insert({ b, e });
	b->edges.insert({ a, e });
}

edge::edge(PathNode* a, PathNode* b) {
	// create an edge betwean 2 nodes that are side by side
	this->nodes = std::make_pair(a, b);
	this->length = distance::diagonal(a, b);
	insert(a, b, this);
}
edge::edge(PathNode* a, PathNode* b, float& distance) {
	this->length = distance;
	this->nodes = std::make_pair(a, b);
	insert(a, b, this);
}
edge::edge(PathNode* a, PathNode* b, float distance, bool oneDirectional)
{
	this->length = distance;
	this->nodes = std::make_pair(a, b);
	if (oneDirectional) {
		a->edges.insert({ b, this });
		this->oneDirectional = true;
		b->furtherNodes.push_back(a);
	}
	else {
		insert(a, b, this);
	}
}
edge::edge(PathNode* a, PathNode* b, bool intra) {
	this->connectsClusters = intra;
	this->nodes = std::make_pair(a, b);
	this->length = 1;
	insert(a, b, this);
}

edge::~edge() {
	this->removeFromNode(this->nodes.first, this->nodes.second);
	this->removeFromNode(this->nodes.second, this->nodes.first);
}

void edge::removeFromNode(PathNode* a, PathNode* b) {
	a->edges.erase(b);
}

void edge::updateWalkability(int mode) {
	if (mode == 0) {
		this->walkable = (this->nodes.first->walkable && this->nodes.second->walkable);
	}
	else if(mode>0) {
		this->walkable = this->blocks < mode;
	}
}

float edge::getLength(PathNode* to, int speed) {
	if (speed == 0) {
		return this->length;
	}
	float tureSpeed;
	if (this->nodes.first == to) {
		tureSpeed = speed + this->dirCoefficient;
	}
	else {
		tureSpeed = speed - this->dirCoefficient;
	}
	if (tureSpeed <= 0) {
		return INFINITY;
	}
	return this->length / tureSpeed;
}

PathNode* edge::getNode(bool b)
{
	if (b) {
		return this->nodes.first;
	}
	return this->nodes.second;
}

void edge::updateLength(float newLen) {
	if (this->length < newLen) {
		this->nodes.first->clus->curentEvent->deletions.insert(this->nodes.first);
		this->nodes.second->clus->curentEvent->deletions.insert(this->nodes.second);
	}
	else {
		this->nodes.first ->clus->curentEvent->inserts.insert(this->nodes.first);
		this->nodes.second->clus->curentEvent->inserts.insert(this->nodes.second);
	}
	this->length = newLen;
}


bool edge::isMoveable(PathNode* a)
{
	if (this->oneDirectional){
		return a == this->nodes.second;
	}
	return true;
}
