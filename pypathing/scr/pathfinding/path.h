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
	  // the movement key thats passed
public: int key;
};

class DPAstarPath : public Path {
public: DPAstarPath(PathNode* start, PathNode* end, int posKey = 0, int speed = 0);

public: void update(updateEvent* update, PathNode* current = nullptr, int distanceKey=-1);
private: void updateDeletions(updateEvent* update, PathNode* current, int distanceKey); // worse path
private: void updateInserts(updateEvent* update, PathNode* current, int distanceKey); // better path
private: std::pair<std::list<PathNode*>, std::list<PathNode*>> getValidSubpathes(PathNode* A, PathNode* B, PathNode* current);
private: std::vector<PathNode*> buildPath(PathNode* end);
};

struct updateNode {
	updateNode(PathNode* Node);
	updateNode(PathNode* Node, bool visited, bool worse, updateNode* parent);
	PathNode* Node;
	bool Rvisited;
	bool Rworse;
	updateNode* parent;
};