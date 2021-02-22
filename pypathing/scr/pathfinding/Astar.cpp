#include "Astar.h"
#include <queue> 
#include <unordered_map>
#include <unordered_set>
#include <set>
#include "Edge.h"
#include "node.h"
#include "distance.h"



#ifndef ASTAR_SEARCH_ALGORYTHEM_LINK
#define ASTAR_SEARCH_ALGORYTHEM_LINK
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
					// get the estimated distance  bewean 2 nodes
					float NodeDistanceEstimate = distance::distance(newNode, end, distanceKey) + currentNode->distance + edge_iter->second->length;

					// priorityse node Distance
					distanceQueue.push(NodeDistanceEstimate);
					// upade lenth
					newNode->distance = currentNode->distance + edge_iter->second->length;
					// create initiater
					newNode->movedFrom = currentNode;
					// add to map
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
};

#endif