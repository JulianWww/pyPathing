// Astar search algorythem s


#include <vector>
#include "distance.h"

#ifndef ASTAR_SEARCH_ALGORYTHEM
#define ASTAR_SEARCH_ALGORYTHEM
class Cluster;
class PathNode;

namespace serchers {
	// astar algorythom performed on nodes
	std::list<PathNode*> Astar_c_node(PathNode* start, PathNode* end, int distanceKey = 0, bool getVisited = false);
}
#endif