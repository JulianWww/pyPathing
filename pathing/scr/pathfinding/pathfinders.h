#pragma once
#include <vector>

class PathNode;

namespace serchers {
	// astar algorythom performed on nodes
	std::vector<PathNode*> bfs_c_node(PathNode* start, PathNode* end, bool getVisited = false);
	std::vector<PathNode*> dfs_c_node(PathNode* start, PathNode* end, bool getVisited = false);
}