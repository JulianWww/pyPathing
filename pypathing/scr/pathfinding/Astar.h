// Astar search algorythem s


#include <vector>
#include <list>
#include <queue>
#include <list>


#ifndef ASTAR_SEARCH_ALGORYTHEM
#define ASTAR_SEARCH_ALGORYTHEM
class Cluster;
class VisGraph;
class PathNode;


namespace serchers {
	// astar algorythom performed on nodes
	std::list<PathNode*> Astar_c_node(PathNode* start, PathNode* end, int distanceKey = 0, bool getVisited = false);
	std::list<PathNode*> Astar_c_node_wspeed(PathNode* start, PathNode* end, int distanceKey = 0, bool getVisited = false, int speed = 0);

	std::list<PathNode*> ThedaStar(PathNode* start, PathNode* end, VisGraph* graph, int distanceKey=0, bool getVisited=false, int speed=0);
}

#endif