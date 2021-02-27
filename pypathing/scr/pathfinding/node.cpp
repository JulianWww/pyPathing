#include "node.h"
#include "Edge.h"
#include "scr/jvector.h"
#include "Cluster.h"
#include "updateEvent.h"

 PathNode::PathNode(std::list<std::pair<PathNode*, short>> connectedNodes, std::vector<int> postion, short& movementMode, std::vector<int> ofset){
	 this->movementMode = movementMode;
	// constructor
	pos = jce::vector::add(postion, ofset);
	for (auto node_iter = connectedNodes.begin(); node_iter != connectedNodes.end(); node_iter++) {
		// convert the pointers pointer to pointer to the other node connected to this one
		PathNode* otherNode = node_iter->first;
		// create new edge and save the pointer
		edge* currentEdge = new edge(this, otherNode);
		currentEdge->length = distance::diagonal(this, node_iter->first);
		// add the pointer to the edges to the node
		this->edges.insert({ otherNode, currentEdge });
		otherNode->edges.insert({ this, currentEdge });
	}
}
/*
old linker to > operator
inline bool PathNode::operator<(const PathNode& rhs) const noexcept{
	// logic here
	return true; // for example
}*/
PathNode::PathNode(PathNode* n) {
	this->pos = n->pos;
	this->lowerEquvilant = n;
}
PathNode::PathNode() {
}
PathNode::PathNode(std::vector<int>pos, int id)
{
	this->pos = pos;
	this->id = id;
}
PathNode::~PathNode() {
	while (this->edges.size()) {
		 delete((*this->edges.begin()).second);
	}
}
std::vector<int>PathNode::connectedNodes() {
	 std::vector<int> res = {};
	 for (auto e = this->edges.begin(); e != this->edges.end(); e++) {
		 if (e->second->walkable) {
			 auto a = e->first->id;
			 res.push_back((int)a);
		 }
	 }
	 return res;
}

void PathNode::setWalkable(bool newWalkable, short mode) {
	if (this->walkable != newWalkable) {
		if (newWalkable) {
			this->clus->curentEvent->inserts.push_back(this);
		}
		else {
			this->clus->curentEvent->deletions.push_back(this);
		}
		this->walkable = newWalkable;
		// iterate over every edge and update there walkability
		for (auto edge_iter = this->edges.begin(); edge_iter != this->edges.end(); edge_iter++) {
			edge_iter->second->updateWalkability(0);
		}
		if (this->lowerEquvilant==NULL){
			for (auto a_node = this->edges.begin(); a_node != this->edges.end(); a_node++) {
				if (distance::manhattan(this, a_node->first) == 1) {
					for (auto b_node = this->edges.begin(); b_node != a_node; b_node++) {
						if (distance::manhattan(this, b_node->first) == 1) {
							if (a_node->first->edges.count(b_node->first) == 1) {
								if (newWalkable) {
									a_node->first->edges.at(b_node->first)->blocks--;
								}
								else {
									a_node->first->edges.at(b_node->first)->blocks++;
								}
								a_node->first->edges.at(b_node->first)->updateWalkability(mode);
							}
						}
					}
				}
			}
		}
		//this->clus->updateConnections();
	}
}
void PathNode::setWalkable(bool newWalkable) {
	this->setWalkable(newWalkable, this->movementMode);
}





