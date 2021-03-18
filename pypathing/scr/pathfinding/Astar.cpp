#include "Astar.h"
#include <queue> 
#include <unordered_map>
#include <unordered_set>
#include <set>
#include "Edge.h"
#include "node.h"
#include "distance.h"
#include "scr/nodeQueue.h"
#include "Cluster.h"



#ifndef ASTAR_SEARCH_ALGORYTHEM_LINK
#define ASTAR_SEARCH_ALGORYTHEM_LINK

std::list<PathNode*>getPath(PathNode* node) {
	std::list<PathNode*> res;
	while (node != NULL) {
		res.push_front(node);
		node = node->movedFrom;
	}
	return res;
}

std::list<PathNode*> serchers::Astar_c_node(PathNode* start, PathNode* end, int distanceKey, bool getVisited) {
		// a queue used to define next move
		std::priority_queue<float, std::vector<float>, std::greater<float>> distanceQueue;
		distanceQueue.push(0);

		// map of prioryty to queue containing the nodes of that area
		std::unordered_map<float, std::queue<PathNode*>> nodeMap = { {(float)0, std::queue<PathNode*>({start}) } };
		start->distance = 0;

		// set of visited postions
		std::unordered_set<PathNode*> visited = { start };

		while (distanceQueue.size() > 0) {
			float current_location = distanceQueue.top();
			distanceQueue.pop();
			// get the node we currently operate on an pop it from queue
			PathNode* currentNode = nodeMap.find(current_location)->second.front();
			nodeMap.find(current_location)->second.pop();
			for (auto edge_iter = currentNode->edges.begin(); edge_iter != currentNode->edges.end(); edge_iter++) {
				// the new node to be operated on
				PathNode* newNode = edge_iter->first;

				if (visited.count(newNode) == 0 && newNode->walkable && edge_iter->second->walkable) {

					// upade lenth
					newNode->distance = currentNode->distance + edge_iter->second->length;
					// create initiater
					newNode->movedFrom = currentNode;
					// add to map

					// get the estimated distance  bewean 2 nodes
					float NodeDistanceEstimate = distance::distance(newNode, end, distanceKey) + newNode->distance;
					// priorityse node Distance
					distanceQueue.push(NodeDistanceEstimate);

					if (nodeMap.count(NodeDistanceEstimate) == 0) {
						nodeMap.insert({ NodeDistanceEstimate, std::queue<PathNode*>() });
					}
					nodeMap.find(NodeDistanceEstimate)->second.push(newNode);
					// insert to visited set
					visited.insert(newNode);

					// backtrack path
					if (newNode == end) {
						std::list<PathNode*> path;
						if (getVisited) {
							for (auto currentNode = visited.begin(); currentNode != visited.end(); currentNode++) {
								path.push_front(*currentNode);
							}
							return path;
						}

						PathNode* Node = end;
						while (Node->distance > 0) {
							path.push_front(Node);
							Node = Node->movedFrom;
						}
						//path.push_back(start);
						return path;
					}
				}
			}

		}

		return {};
	};

std::list<PathNode*> serchers::Astar_c_node_wspeed(PathNode* start, PathNode* end, int distanceKey, bool getVisited, int speed) {
	// a queue used to define next move
	std::priority_queue<float, std::vector<float>, std::greater<float>> distanceQueue;
	distanceQueue.push(0);
	
	// map of prioryty to queue containing the nodes of that area
	std::unordered_map<float, std::queue<PathNode*>> nodeMap = { {(float)0, std::queue<PathNode*>({start}) } };
	start->distance = 0;

	// set of visited postions
	std::unordered_set<PathNode*> visited = { start };
	
	while (distanceQueue.size() > 0) {
		float current_location = distanceQueue.top();
		distanceQueue.pop();
		// get the node we currently operate on an pop it from queue
		PathNode* currentNode = nodeMap.find(current_location)->second.front();
		nodeMap.find(current_location)->second.pop();
		
		// backtrack path
		if (currentNode == end) {
			std::list<PathNode*> path;
			if (getVisited) {
				for (auto currentNode = visited.begin(); currentNode != visited.end(); currentNode++) {
					path.push_front(*currentNode);
				}
				return path;
			}
			
			PathNode* Node = end;
			while (Node->distance > 0) {
				path.push_front(Node);
				Node = Node->movedFrom;
			}
			//path.push_back(start);
			return path;
		}


		for (auto edge_iter = currentNode->edges.begin(); edge_iter != currentNode->edges.end(); edge_iter++) {
			// the new node to be operated on
			PathNode* newNode = edge_iter->first;
			
			float edge_distance = edge_iter->second->getLength(newNode, speed);

			float NodeDistanceEstimate =   distance::distance(newNode, end, distanceKey) 
											 + currentNode->distance 
											 + edge_distance;

			if ((visited.count(newNode) == 0 || NodeDistanceEstimate < newNode->distance) && newNode->walkable && edge_iter->second->walkable && edge_distance != INFINITY) {
					// get the estimated distance  bewean 2 nodes

				// priorityse node Distance
				distanceQueue.push(NodeDistanceEstimate);
				// upade lenth
				newNode->distance = currentNode->distance + edge_distance;
				// create initiater
				newNode->movedFrom = currentNode;
				// add to map
				if (nodeMap.count(NodeDistanceEstimate) == 0) {
					nodeMap.insert({ NodeDistanceEstimate, std::queue<PathNode*>() });
				}
				nodeMap.find(NodeDistanceEstimate)->second.push(newNode);
				// insert to visited set
				visited.insert(newNode);
			}
		}
	}
	return {};
}


void ThetaStarUpdateNode(PathNode* node, PathNode* nextNode, edge* e, VisGraph* graph, int speed, jce::NodeQueue<PathNode>& open, int distanceKey, PathNode* goal) {
	PathNode* parent;
	if (node->movedFrom!=NULL && graph->line_of_sight(node->movedFrom, nextNode)) {
		parent = node->movedFrom;
	}
	else {
		parent = node;
	}
	if (nextNode->distance > node->distance + e->getLength(nextNode, speed)) {
		nextNode->distance = node->distance + e->getLength(nextNode, speed);

		nextNode->setKey(nextNode->distance + distance::distance(nextNode, goal, distanceKey), node);

		open.remove(nextNode);
		nextNode->movedFrom = parent;
		open.push(nextNode);
	}
}

std::list<PathNode*> serchers::ThedaStar(PathNode* start, PathNode* end, VisGraph* graph, int distanceKey, bool getVisited, int speed)
{
	start->distance = 0;

	jce::NodeQueue<PathNode> open;
	start->setKey(0);
	open.push(start);
	start->movedFrom = NULL;
	std::list<PathNode*> closed = { start };

	while (open.size() > 0) {
		PathNode* current = open.topAndPop();
		if (current == end) {
			return getPath(current);
		}
		closed.push_back(current);
		for (auto const& edgeIter : current->edges) {
			if (edgeIter.first->walkable && edgeIter.second->walkable){
				if (std::find(closed.begin(), closed.end(), edgeIter.first) == closed.end()) {
					if (!open.contains(edgeIter.first)) {
						edgeIter.first->distance = std::numeric_limits<float>::infinity();
						edgeIter.first->movedFrom = NULL;
					}
					ThetaStarUpdateNode(current, edgeIter.first, edgeIter.second, graph, speed, open, distanceKey, end);
				}
			}
		}
	}
	return {};
}
;

#endif