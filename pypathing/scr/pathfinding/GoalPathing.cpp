#include "GoalPathing.h"

#include <unordered_set>
#include <queue>
#include "distance.h"
#include "updateEvent.h"


GoalCluster::GoalCluster(Cluster* clus) {
	this->clus = clus;
	this->buildNodes();
}
GoalCluster::GoalCluster(bool liveBuild)
{
	this->liveBuild = liveBuild;
}

GoalCluster::~GoalCluster() {
	for (auto n = this->nodes.begin(); n != this->nodes.end(); n++) {
		GoalNode* node = ((*n).second);
		delete node;
	}
	this->nodes.clear();
}

void GoalCluster::buildNodes() {
	for (auto n = this->clus->nodes.begin(); n != this->clus->nodes.end(); n++) {
		GoalNode* newNode = new GoalNode();
		newNode->Nodeptr = n->second;
		this->nodes.insert({ n->first, newNode });
	}
}
void GoalCluster::buildGraph(int start, unsigned int speed) {
	this->build(this->clus->nodes.at(start), speed);
}
void GoalCluster::build(PathNode* start, unsigned int speed) {
	this->goal = start;
	std::unordered_map<float, std::unordered_set<PathNode*>>currentNodes = { {0, {start}} };
	std::priority_queue<float, std::vector<float>, std::greater<float>> priorities;
	priorities.push(0);
	this->nodes.at(start->id)->goalThisWay = NULL;
	std::unordered_set<PathNode*> visitedNodes;

	while (priorities.size() > 0) {
		auto val = priorities.top();
		for (auto Node = currentNodes.at(val).begin(); Node != currentNodes.at(val).end(); Node++) {
			visitedNodes.insert((*Node));
			for (auto currentEdge = (*Node)->edges.begin(); currentEdge != (*Node)->edges.end(); currentEdge++) {
				PathNode* newNode = currentEdge->first;

				if (newNode->walkable && currentEdge->first->walkable) {
					float edge_len;
					if (speed == 0) {
						edge_len = currentEdge->second->length;
					}
					else {
						edge_len = currentEdge->second->getLength(newNode, speed);
					}

					if ((visitedNodes.count(newNode) == 0 || newNode->distance > (*Node)->distance + edge_len) && edge_len != INFINITY) {
						visitedNodes.insert(newNode);
						auto newGoalnode = this->nodes.at(newNode->id);
						newGoalnode->goal = this->goal;
						newGoalnode->goalThisWay = this->nodes.at((*Node)->id);
						newNode->distance = (*Node)->distance + edge_len;

						if (currentNodes.count(newNode->distance) == 0) {
							currentNodes.insert({ newNode->distance, {} });
							priorities.push(newNode->distance);
						}
						currentNodes.at(newNode->distance).insert(newNode);
					}
				}
				else if (currentEdge->first->walkable) {
					auto newGoalnode = this->nodes.at(newNode->id);
					newGoalnode->goal = this->goal;
					newGoalnode->goalThisWay = NULL;
				}
			}
		}
		currentNodes.erase(val);
		priorities.pop();
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
void GoalCluster::setGoal(int a) {
	this->goal = this->clus->nodes.at(a);

	this->currentNodes = { {0, {this->goal}} };
	this->visitedNodes.clear();
	while (!this->nodePriorities.empty()) {
		this->nodePriorities.pop();
	}
	this->nodePriorities.push(0);
	this->nodes.at(this->goal->id)->goalThisWay = NULL;
	this->nodes.at(this->goal->id)->goal = goal;
}


void GoalCluster::liveBuildNextNode(PathNode* targetNode, int distanceKey, unsigned int speed) {

	while (this->nodePriorities.size() > 0) {
		auto val = this->nodePriorities.top();
		for (auto Node = this->currentNodes.at(val).begin(); Node != this->currentNodes.at(val).end(); Node++) {
			this->visitedNodes.insert((*Node));
			for (auto currentEdge = (*Node)->edges.begin(); currentEdge != (*Node)->edges.end(); currentEdge++) {
				PathNode* newNode = currentEdge->first;

				if (newNode->walkable && currentEdge->first->walkable){
					float edge_len;
					if (speed == 0) {
						edge_len = currentEdge->second->length;
					}
					else {
						edge_len = currentEdge->second->getLength(newNode, speed);
					}

					if ((this->visitedNodes.count(newNode) == 0 || newNode->distance > (*Node)->distance + edge_len) && edge_len != INFINITY) {
						this->visitedNodes.insert(newNode);
						auto newGoalnode = this->nodes.at(newNode->id);
						newGoalnode->goal = this->goal;
						newGoalnode->goalThisWay = this->nodes.at((*Node)->id);
						newNode->distance = (*Node)->distance + edge_len;

						float distance = newNode->distance + distance::distance(newNode, targetNode, distanceKey);

						if (currentNodes.count(newNode->distance) == 0) {
							currentNodes.insert({ newNode->distance, {} });
							this->nodePriorities.push(newNode->distance);
						}
						this->currentNodes.at(newNode->distance).insert(newNode);
						if (newNode == targetNode) {
							return;
						}
					}
				}
			}
		}
		this->currentNodes.erase(val);
		this->nodePriorities.pop();
	}
}

void GoalCluster::update(updateEvent* update, int distanceKey, int speed)
{
	if (!this->liveBuild) {
		this->build(this->goal, speed);
	}
	else {
		std::unordered_set<PathNode*> toUpdate(update->inserts.begin(), update->inserts.end());
		for (auto const& Node : update->deletions) {
			toUpdate.insert(Node);
		}
		this->updateNodes(update, this->getAjacents(toUpdate), distanceKey, speed);
	}
}

void GoalCluster::updateDels(updateEvent* update, int distanceKey, int speed)
{
	if (!this->liveBuild) {
		this->build(this->goal, speed);
	}
	else {
		std::unordered_set<PathNode*> toUpdate(update->deletions.begin(), update->deletions.end());
		toUpdate = this->getAjacents(toUpdate);
		for (auto const& mNode : update->inserts) {
			toUpdate.insert(mNode);
		}
		this->updateNodes(update, toUpdate, distanceKey, speed);
	}
}

void GoalCluster::updateNodes(updateEvent* update, std::unordered_set<PathNode*> nodes, int distanceKey, int speed)
{
	this->currentNodes.clear();
	while (nodePriorities.size() != 0) {
		nodePriorities.pop();
	}
	this->visitedNodes.clear();

	this->currentNodes.insert({ 0, {this->goal} });
	this->nodePriorities.push(0);

	for (auto const& toUpdate : nodes) {
		if (std::find(this->visitedNodes.begin(), this->visitedNodes.end(), toUpdate)!=this->visitedNodes.end()){
			this->liveBuildNextNode(toUpdate, distanceKey, speed);
		}
	}
	/*if (!this->liveBuild) {
		this->currentNodes.clear();
		while (nodePriorities.size()!=0) {
			nodePriorities.pop();
		}
		this->visitedNodes.clear();
	}*/
}

std::unordered_set<PathNode*> GoalCluster::getAjacents(std::unordered_set<PathNode*> nodes)
{
	auto res = std::unordered_set<PathNode*>(nodes);
	for (auto const& Node : nodes) {
		for (auto const& currentEdge : Node->edges) {
			res.insert(currentEdge.first);
		}
	}
	return res;
}

void GoalCluster::setLiveBuild(bool val)
{
	this->liveBuild = val;
}

PathNode* GoalCluster::liveGetNextNode(int a, int distanceKey, unsigned int speed) {
	GoalNode* start = this->nodes.at(a);
	if (start->Nodeptr != this->goal) {
		this->liveBuildNextNode(start->Nodeptr, distanceKey, speed);
	}
	if (start->goalThisWay == NULL) {
		return NULL;
	}
	return start->goalThisWay->Nodeptr;
}