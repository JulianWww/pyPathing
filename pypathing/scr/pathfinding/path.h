#include <vector>
#include <list>
#include <unordered_map>
#include "scr/nodeQueue.h"
#include "Edge.h"
#include <memory>
#include <iostream>

class PathNode;
class updateEvent;
class VisGraph;
class Environment;


class Path {
	// the path current path
public: std::vector<PathNode*> path = {};
	  // how expensive the path is
public: float cost;
	  // the speed specified
public: float speed;
	  // the movement key thats passed
public: int key;
public: bool valid = false;
public: PathNode* start;
public: PathNode* end;
};

class AstarPath : public Path {
public: AstarPath(PathNode* start, PathNode* end, int posKey = 0, int speed = 0);
protected: void rebuild(int distanceKey = NULL);
};

class DPAstarPath : public AstarPath {
public: DPAstarPath(PathNode* start, PathNode* end, int posKey = 0, int speed = 0) : AstarPath(start, end, posKey, speed) {};
public: void update(updateEvent* update, PathNode* current = nullptr, int distanceKey=-1);
public: void cheapUpdate(updateEvent* update, PathNode* current = nullptr, int distanceKey = -1);
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

namespace LPAstar {
	struct Key {
		float k1, k2;

		Key() : k1(std::numeric_limits<float>::infinity()), k2(std::numeric_limits<float>::infinity()) {}
		Key(float k1_, float k2_) : k1(k1_), k2(k2_) {}
		Key(const Key& other) : k1(other.k1), k2(other.k2) {}

		inline bool operator==(const Key& p) const {
			return k1 == p.k1 && k2 == p.k2;
		}
		inline bool operator!=(const Key& p) const {
			return !(*this == p);
		}
		inline bool operator<(const Key& p) const {
			return k1 < p.k1 || (k1 == p.k1 && k2 <= p.k2);
		}
		inline bool operator>(const Key& p) const {
			return p < *this;
		}

		Key& operator=(const Key& other) {
			if (this == &other) {
				return *this;
			}
			k1 = other.k1;
			k2 = other.k2;
			return *this;
		}
	};

	struct LPAstarNode;
	struct NodeDummy {
		float rhs = INFINITY;
		LPAstarNode* parent= nullptr;;

		NodeDummy(float val, LPAstar::LPAstarNode* n) : rhs(val), parent(n) {}
		NodeDummy() {};
	};

	struct LPAstarNode : public NodeDummy {
		LPAstarNode(PathNode* node);
		// the pathnode this refers to
		PathNode* Node=nullptr;
		// im copining wiki
		float g = INFINITY;
		Key key;
		std::list<PathNode*> getPrevious();

		LPAstarNode() {}
		LPAstarNode(PathNode& p, LPAstarNode* c = nullptr) : g(std::numeric_limits<float>::infinity()), NodeDummy(std::numeric_limits<float>::infinity(), c), Node(&p) {}

		LPAstarNode& operator=(const LPAstarNode& other);

		bool IsConsistent() const;

		bool operator>(const LPAstarNode& other);
		bool operator==(const LPAstarNode& other);
	};

	Key CalculateKey(LPAstarNode* vertex, PathNode* goal, int distanceKey, float hweight = 1);
	
}

inline std::ostream& operator<< (std::ostream& out, const LPAstar::LPAstarNode& next) {
	out << " LPAstarNode <Node: " << next.Node << "g: " << next.g << "rhs: " << next.rhs << " ";
	return out;
}

class LPAstarPath : public Path {
protected: std::unordered_map<PathNode*, LPAstar::LPAstarNode*> nodes;
protected: jce::NodeQueue<LPAstar::LPAstarNode> open;
protected: LPAstar::LPAstarNode* goal;
protected: LPAstar::LPAstarNode* start;
protected: int key;
protected: bool goal_became_unreachable;

public: LPAstarPath(PathNode* start, PathNode* end, int posKey = 0, int speed = 0);
public:~LPAstarPath();

public: bool update(updateEvent* update, PathNode* current = nullptr, int distanceKey = -1);

private: LPAstar::Key CalculateKey(LPAstar::LPAstarNode* vertex, int distanceKey, float hweight = 1);
private: float GetCost(edge* e, PathNode* to, int speed) const;
private: void Initialize(PathNode* start, PathNode* end, int metrics);
private: bool findShortestPath(int speed, int distanceKey);
private: std::list<std::pair<LPAstar::LPAstarNode*, edge*>> getSucsessors(LPAstar::LPAstarNode* node);
private: std::list<std::pair<LPAstar::LPAstarNode*, edge*>> getAllSucsessors(LPAstar::LPAstarNode* node);
private: LPAstar::LPAstarNode* getNode(PathNode* node);
private: void updateNode(LPAstar::LPAstarNode* node, int& distanceKey);
private: LPAstar::NodeDummy GetMinPredecessor(LPAstar::LPAstarNode* current, int speed);
private: void buildPath();
private: void updateInserts(updateEvent* update, int key);
private: void updateDels(updateEvent* update, int key);
};