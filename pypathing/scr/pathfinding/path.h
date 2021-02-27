#include <vector>
#include <list>

class PathNode;
class updateEvent;


class Path {
	// the path current path
public: std::vector<PathNode*> path = {};
	  // how expensive the path is
public: float cost;
	  // the speed specified
public: float speed;
};

class DPAstarPath : public Path {
public: DPAstarPath(PathNode* start, PathNode* end, int posKey = 0, int speed = 0);

public: void update(updateEvent* update, PathNode* current = nullptr);
private: void updateDeletions(updateEvent* update, PathNode* InNode, PathNode* OutNode, PathNode* current);
private: std::pair<std::list<PathNode*>, std::list<PathNode*>> getValidSubpathes(PathNode* A, PathNode* B, PathNode* current);
};