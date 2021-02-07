#include "node.h"
#include "Edge.h"
#include "scr/jvector.h"


 PathNode::PathNode(std::list<PathNode*> connectedNodes, std::vector<int> postion, std::vector<int> ofset){
	// constructor
	pos = jce::vector::add(postion, ofset);
	for (auto node_iter = connectedNodes.begin(); node_iter != connectedNodes.end(); node_iter++) {
		// convert the pointers pointer to pointer to the other node connected to this one
		PathNode* otherNode = *node_iter;
		// create new edge and save the pointer
		edge* currentEdge = new edge(this, otherNode);

		// add the pointer to the edges to the node
		this->edges.insert(currentEdge);
		otherNode->edges.insert(currentEdge);
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

 PathNode::~PathNode() {
	 while (this->edges.size()) {
		 delete(*this->edges.begin());
	 }
 }


