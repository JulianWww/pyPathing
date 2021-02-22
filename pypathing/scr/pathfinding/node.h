#include <set>
#include <unordered_map>
#include <tuple>
#include <list>
#include <vector>
#include <string>

//#define buildPos(x,y,z, arr)(x + y * arr{0]{0].size() + z*arr{0]{0].size()*arr{0].size()+1)
class Cluster;
#ifndef node
#define node

class edge;
class Cluster;

// the node path
class PathNode {
	// the edges connected to this node
public: std::unordered_map<PathNode*, edge*> edges = {};
	  // the postion of the postion
public: std::vector<int> pos;

	  //internal distance for all algorythems
public: float distance = 0;
	  // the id of the node cumputed by pos
public: size_t id=-1;
	// internal pointer to the node that initated this one
public: PathNode* movedFrom;
public: Cluster* clus;
public: short movementMode;
public: PathNode* lowerEquvilant=nullptr;
	  // wether or not the node can be walked on
public: bool walkable = true;
	  // set wether or not this node is walkable
public: void setWalkable(bool, short);
public: void setWalkable(bool);


public:
	PathNode(std::list<std::pair<PathNode*, short>> connectedNodes, std::vector<int> postion, short&, std::vector<int>ofset = { 0,0,0 });
	PathNode();
	PathNode(PathNode*);
	~PathNode();

	std::vector<int>connectedNodes();
};

class GoalNode{
public: PathNode* Nodeptr;
public: GoalNode* goalThisWay;
public: PathNode* goal;
};

#endif