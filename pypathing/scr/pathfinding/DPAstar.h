#include <vector>

class PathNode;


class DPAstarPath {
	// the path current path
public: std::vector<PathNode*> path;
	  // how expensive the path is
public: float cost;

public: DPAstarPath(PathNode* start, PathNode* end, int posKey = 0, int speed = 0);
};