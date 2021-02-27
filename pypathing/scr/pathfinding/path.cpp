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

	std::vector<PathNode*> path(nodes.begin(), nodes.end());
	this->path = path;
	auto last = *(path.end() - 1);
	this->cost = last->distance;
	this->speed = speed;
}

void DPAstarPath::update(updateEvent* update, PathNode* current) {
	if (current == nullptr) {
		current = this->path.front();
	}
	return;
}

void DPAstarPath::updateDeletions(updateEvent* update, PathNode* InNode, PathNode* OutNode, PathNode* current) {
	auto turningNodes = update->getEntryAndExitsInserts(this);
	if (std::get<0>(turningNodes) == nullptr) {
		return;
	}
	PathNode* A = std::get<0>(turningNodes);
	PathNode* B = std::get<1>(turningNodes);
	PathNode* goal = *(this->path.end() - 1);

	std::pair<std::list<PathNode*>, std::list<PathNode*>> validSubPathes = this->getValidSubpathes(A, B, current);

	std::priority_queue<float> distanceQueue;
	distanceQueue.push(0);
	current->distance = 0;
	current->movedFrom = nullptr;
	std::unordered_map<float, std::list<PathNode*>> nodeMap = { {0, {current} } };
	std::list<PathNode*> visited;

	while (distanceQueue.size() > 0) {
		float currentDistance = distanceQueue.top();
		distanceQueue.pop();
		PathNode* currentNode = nodeMap.at(currentDistance).front();

		if (currentNode == goal) {
			std::list<PathNode*> path;
			while (currentNode->movedFrom != nullptr) {
				path.push_front(currentNode);
				currentNode = currentNode->movedFrom;
			}
			std::vector<PathNode*> path_vec(path.begin(), path.end());
			this->path = path_vec;
			return;
		}

		for (auto const& currentEdge : currentNode->edges) {
			PathNode* newNode = currentEdge.first;


			auto pos = std::find(validSubPathes.second.begin(),
				validSubPathes.second.end(), currentNode);

			if (pos != validSubPathes.second.end() && newNode != *std::next(pos)) {
				continue;
			}

			pos = std::find(validSubPathes.first.begin(),
				validSubPathes.first.end(), newNode);

			if (pos != validSubPathes.second.end() && newNode != *std::next(pos, -1)) {
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

				distanceQueue.push(distance);

				if (nodeMap.count(distance) == 0) {
					nodeMap.insert({ distance, std::list<PathNode*>() });
				}
				nodeMap.at(distance).push_back(newNode);
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
