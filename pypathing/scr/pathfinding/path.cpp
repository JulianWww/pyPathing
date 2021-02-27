#include "path.h"
#include "node.h"
#include "Astar.h"
#include "Edge.h"
#include "updateEvent.h"
#include <list>


DPAstarPath::DPAstarPath(PathNode* start, PathNode* end, int posKey, int speed) {
	std::list<PathNode*> nodes;
	if (speed == 0) {
		nodes = serchers::Astar_c_node(start, end, posKey);
	}
	else {
		nodes = serchers::Astar_c_node_wspeed(start, end, posKey, false, speed);
	}
	nodes.push_front(start);
	std::vector<PathNode*> path(nodes.begin(), nodes.end());
	this->path = path;
	auto last = *(path.end() - 1);
	this->cost = last->distance;
	this->speed = speed;
	this->key = posKey;
}

void DPAstarPath::update(updateEvent* update, PathNode* current, int distanceKey) {
	if (current == nullptr) {
		current = this->path.front();
	}
	this->updateDeletions(update, current, distanceKey);
	this->updateInserts  (update, current, distanceKey);
}

void DPAstarPath::updateDeletions(updateEvent* update, PathNode* current, int distanceKey) {
	auto turningNodes = update->getEntryAndExitsDels(this);
	if (std::get<0>(turningNodes) == nullptr) {
		return;
	}
	PathNode* A = std::get<0>(turningNodes);
	PathNode* B = std::get<1>(turningNodes);
	PathNode* goal = *(this->path.end() - 1);

	std::pair<std::list<PathNode*>, std::list<PathNode*>> validSubPathes = this->getValidSubpathes(A, B, current);

	std::priority_queue<float, std::vector<float>, std::greater<float>> distanceQueue;
	distanceQueue.push(0);
	current->distance = 0;
	current->movedFrom = nullptr;
	std::unordered_map<float, std::list<PathNode*>> nodeMap = { {0, {current} } };
	std::list<PathNode*> visited = { current };

	while (distanceQueue.size() > 0) {
		float currentDistance = distanceQueue.top();
		distanceQueue.pop();
		PathNode* currentNode = nodeMap.at(currentDistance).front();
		nodeMap.at(currentDistance).pop_front();

		if (currentNode == goal) {
			this->path = this->buildPath(currentNode);
			return;
		}

		for (auto const& currentEdge : currentNode->edges) {
			PathNode* newNode = currentEdge.first;

			if (newNode->walkable && currentEdge.second->walkable) {
				auto pos = std::find(validSubPathes.second.begin(),
					validSubPathes.second.end(), currentNode);

				if (pos != validSubPathes.second.end() && newNode != *std::next(pos)) {
					continue;
				}

				pos = std::find(validSubPathes.first.begin(),
					validSubPathes.first.end(), newNode);

				if (pos != validSubPathes.first.end() && pos != validSubPathes.first.begin() && newNode != *std::next(pos, -1)) {
					continue;
				}

				float distance = currentNode->distance + currentEdge.second->getLength(currentNode, this->speed);
				bool notcontains = (std::find(visited.begin(), visited.end(), newNode) == visited.end());
				if (distance < newNode->distance || notcontains) {
					if (notcontains) {
						visited.push_back(newNode);
					}
					newNode->distance = distance;
					newNode->movedFrom = currentNode;

					float distEstimate = distance + distance::distance(newNode, goal, distanceKey);

					distanceQueue.push(distEstimate);

					if (nodeMap.count(distEstimate) == 0) {
						nodeMap.insert({ distEstimate, std::list<PathNode*>() });
					}
					nodeMap.at(distEstimate).push_back(newNode);
				}
			}
		}
	}
}

void DPAstarPath::updateInserts(updateEvent* update, PathNode* start, int distanceKey) {
	auto turningNodes = update->getEntryAndExitsInserts(this);
	auto validSubPaths =  this->getValidSubpathes(turningNodes.first, turningNodes.second, start);
	if (turningNodes.first == nullptr) {
		validSubPaths.second = validSubPaths.first;
	}

	std::priority_queue<float, std::vector<float>, std::greater<float>> distanceQueue;
	distanceQueue.push(0);
	start->distance = 0;
	start->movedFrom = nullptr;

	updateNode* current = new updateNode(start);
	std::unordered_map<float, std::list<updateNode*>> nodeMap = { {0, {current} } };
	std::list<PathNode*> visited = { start };

	std::unordered_map<PathNode*, updateNode*> correspondingUpdate = { {start, current } };

	PathNode* goal = this->path.back();

	while (distanceQueue.size() > 0) {
		float distance = distanceQueue.top();
		distanceQueue.pop();

		current = nodeMap.at(distance).front();
		nodeMap.at(distance).pop_front();

		if (current->Node == this->path.back()) {
			this->path = this->buildPath(current->Node);;
			return;
		}

		current->Rvisited = current->parent->Rvisited || update->isInDels(current->Node);
		current->Rworse = current->parent->Rworse;
		bool checkAll;

		auto Iter = std::find(validSubPaths.second.begin(), validSubPaths.second.end(), current->Node);
		if (Iter == validSubPaths.second.end()) {
			checkAll = true;
		}
		else {
			current->Rworse = current->parent->Rworse || (current->Node->distance
														  + update->distance(current->Node, distanceKey)
														  + update->distance(goal, distanceKey)) > this->cost;
			
			if (!current->Rworse) {
				checkAll = true;
			}
			else if (Iter != validSubPaths.second.end()) {
				checkAll = false;
			}
			else {
				continue;
			}

		}
	
		for (auto const& currentEdge : current->Node->edges) {
			if (current->Node->walkable && currentEdge.second->walkable) {
				if (!checkAll && Iter != validSubPaths.second.begin() && currentEdge.first != *std::next(Iter, -1)) {
					continue;
				}
				float edgeLength = currentEdge.second->getLength(currentEdge.first, this->speed);
				auto testPos = std::find(visited.begin(), visited.end(), currentEdge.first);

				if (current->Node->distance + edgeLength < currentEdge.first->distance
					|| testPos == visited.end()) {
					if (testPos == visited.end()) {
						visited.push_back(currentEdge.first);
					}

					currentEdge.first->movedFrom = current->Node;
					currentEdge.first->distance = current->Node->distance + edgeLength;

					updateNode* nextUpdate;
					if (correspondingUpdate.count(currentEdge.first) == 0) {
						nextUpdate = new updateNode(currentEdge.first);
						correspondingUpdate.insert({ currentEdge.first, nextUpdate });
					}
					else {
						nextUpdate = correspondingUpdate.at(currentEdge.first);
					}
					nextUpdate->parent = current;


					float distEstimate = distance + distance::distance(nextUpdate->Node, goal, distanceKey);

					distanceQueue.push(distEstimate);

					if (nodeMap.count(distEstimate) == 0) {
						nodeMap.insert({ distEstimate, std::list<updateNode*>() });
					}
					nodeMap.at(distEstimate).push_back(nextUpdate);
				}
			}
		}
	}
}

std::pair<std::list<PathNode*>, std::list<PathNode*>> DPAstarPath::getValidSubpathes(PathNode* A, PathNode* B, PathNode* Tnode)
{
	std::list<PathNode*> front;
	std::list<PathNode*> back;

	bool onFront = false;
	bool onBack = false;

	for (auto const& current : this->path) {
		if (current == Tnode) {
			onFront = true;
		}
		if (current == A) {
			onFront = false;
		}
		if (current == B) {
			onBack = true;
		}
		if (onFront) {
			front.push_back(current);
		}
		else if (onBack) {
			back.push_back(current);
		}
	}
	return { front, back };
}

std::vector<PathNode*> DPAstarPath::buildPath(PathNode* currentNode)
{
	std::list<PathNode*> path;
	while (currentNode->movedFrom != nullptr) {
		path.push_front(currentNode);
		currentNode = currentNode->movedFrom;
	}
	path.push_front(currentNode);
	std::vector<PathNode*> path_vec(path.begin(), path.end());
	this->cost = path.back()->distance;
	return path_vec;
}



updateNode::updateNode(PathNode* Node)
{
	this->Node = Node;
	this->Rvisited = false;
	this->Rworse = false;
	this->parent = this;
}

updateNode::updateNode(PathNode* Node, bool visited, bool worse, updateNode* parent)
{
	this->Node = Node;
	this->Rvisited = visited;
	this->Rworse = worse;
	this->parent = parent;
}
