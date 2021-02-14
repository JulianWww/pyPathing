#include "GoalPathing.h"

#include <unordered_set>


GoalCluster::GoalCluster(Cluster* clus) {
	this->clus = clus;
	this->buildNodes();
}

void GoalCluster::buildNodes() {
	for (auto n = this->clus->nodes.begin(); n != this->clus->nodes.end(); n++){
		GoalNode* newNode = new GoalNode();
		newNode->Nodeptr = n->second;
		this->nodes.insert({ n->first, newNode });
	}
}
void GoalCluster::buildGraph(int start) {
	this->build(this->clus->nodes.at(start));
}
void GoalCluster::build(PathNode* start) {
	std::unordered_set<PathNode*> currentNodes = { start };
	this->nodes.at(start->id)->goalThisWay = NULL;
	std::unordered_set<PathNode*> visitedNodes;
	std::unordered_set<PathNode*> newNodes;
	while (currentNodes.size() > 0) {
		for (auto Node = currentNodes.begin(); Node != currentNodes.end(); Node++) {
			visitedNodes.insert(*Node);
			for (auto currentEdge = (*Node)->edges.begin(); currentEdge != (*Node)->edges.end(); currentEdge++) {
				PathNode* newNode;
				if ((*currentEdge)->nodes.first == *Node) {
					newNode = (*currentEdge)->nodes.second;
				}
				else {
					newNode = (*currentEdge)->nodes.first;
				}
				if (visitedNodes.count(newNode) == 0) {
					visitedNodes.insert(newNode);
					newNodes.insert(newNode);
					auto newGoalnode = this->nodes.at(newNode->id);
					newGoalnode->goalThisWay = this->nodes.at((*Node)->id);
					newGoalnode->Nodeptr = *Node;
					this->nodes.insert({ newNode->id, newGoalnode });
				}
			}
		}
		currentNodes.clear();
		currentNodes = newNodes;
		newNodes.clear();
	}
}

PathNode* GoalCluster::getNextPos(int nodeId) {
	auto Node = this->nodes.at(nodeId);
	if (Node->goalThisWay == NULL) {
		return NULL;
	}
	auto nextNode = Node->goalThisWay->Nodeptr;
	return nextNode;
}

