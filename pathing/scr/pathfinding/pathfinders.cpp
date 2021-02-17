#include "pathfinders.h"
#include "Edge.h"
#include <queue>
#include <stack>
#include "node.h"

// perform the bfs algorythem from start to end
std::vector<PathNode*> serchers::bfs_c_node(PathNode* start, PathNode* end, bool getVisited) {
	// the postions we already visited
	std::set<PathNode*> visited = { start };
	// postion queue to evaluate next
	std::queue<PathNode*> EvaluatePostionsQueue = std::queue<PathNode*>({ start });
	// while we have postions to evaluate do so
	while (EvaluatePostionsQueue.size() > 0) {
		// the current node to operate on
		PathNode* currentNode = EvaluatePostionsQueue.front(); 

		for (auto edge_iter = currentNode->edges.begin(); edge_iter != currentNode->edges.end(); edge_iter++) {
			//the new node to put at the end of the queue
			PathNode* newNode = edge_iter->first;

			if (visited.count(newNode) == 0 && newNode->walkable && edge_iter->second->walkable) {
				visited.insert(newNode);
				EvaluatePostionsQueue.push(newNode);
				newNode->distance = currentNode->distance + edge_iter->second->length;
				newNode->movedFrom = currentNode;

				// backtrack path
				if (newNode == end) {
					std::vector<PathNode*> path;
					if (getVisited) {
						for (auto currentNode = visited.begin(); currentNode != visited.end(); currentNode++) {
							path.push_back(*currentNode);
						}
						return path;
					}

					PathNode* Node = end;
					while (Node->distance > 0) {
						path.push_back(Node);
						Node = Node->movedFrom;
					}
					return path;
				}
			}
		}
		
		
		// at the very end remove the postition
		EvaluatePostionsQueue.pop();
	}


	return { start };
}

// perform the bfs algorythem from start to end
std::vector<PathNode*> serchers::dfs_c_node(PathNode* start, PathNode* end, bool getVisited) {
	// the postions we already visited
	std::set<PathNode*> visited = { start };
	// postion queue to evaluate next
	std::stack<PathNode*> EvaluatePostionsQueue = std::stack<PathNode*>({ start });
	// while we have postions to evaluate do so
	while (EvaluatePostionsQueue.size() > 0) {
		// the current node to operate on
		PathNode* currentNode = EvaluatePostionsQueue.top();

		for (auto edge_iter = currentNode->edges.begin(); edge_iter != currentNode->edges.end(); edge_iter++) {
			//the new node to put at the end of the queue
			PathNode* newNode = edge_iter->first;

			if (visited.count(newNode) == 0 && newNode->walkable && edge_iter->second->walkable) {
				visited.insert(newNode);
				EvaluatePostionsQueue.push(newNode);
				newNode->distance = currentNode->distance + edge_iter->second->length;
				newNode->movedFrom = currentNode;

				// backtrack path
				if (newNode == end) {
					std::vector<PathNode*> path;
					if (getVisited) {
						for (auto currentNode = visited.begin(); currentNode != visited.end(); currentNode++) {
							path.push_back(*currentNode);
						}
						return path;
					}

					PathNode* Node = end;
					while (Node->distance > 0) {
						path.push_back(Node);
						Node = Node->movedFrom;
					}
					return path;
				}
			}
		}


		// at the very end remove the postition
		EvaluatePostionsQueue.pop();
	}


	return {};
}