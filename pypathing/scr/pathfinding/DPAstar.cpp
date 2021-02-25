#include "DPAstar.h"
#include "Astar.h"
#include "node.h"
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
}