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
		currentEdge->length = distance::diagonal(this, *node_iter);
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
 PathNode::~PathNode() {
	 while (this->edges.size()) {
		 delete((*this->edges.begin()).second);
	 }
 }
 std::vector<int>PathNode::connectedNodes() {
	 std::vector<int> res = {};
	 for (auto e = this->edges.begin(); e != this->edges.end(); e++) {
		 auto a = e->first->id;
		 res.push_back((int)a);
	 }
	 return res;
 }





