#include <set>
#include <unordered_map>
#include <tuple>
#include <list>
#include <vector>
#include <string>
#include <iostream>

//#define buildPos(x,y,z, arr)(x + y * arr{0]{0].size() + z*arr{0]{0].size()*arr{0].size()+1)
#ifndef NODE
#define NODE
class edge;
class Cluster;
namespace obstacle { class Baise; };

class Key {
public: float val=NULL;
public: int tieBreaker = 0;
public: Key() {};
public: Key(float v) : val(v) {};
public: Key(float v, int t) : val(v), tieBreaker(t) {};
public: bool operator>(Key& k);
public: bool operator<(Key& k);
public: bool operator==(Key& k);
};

// the node path
class PathNode {
	// the edges connected to this node
public: std::unordered_map<PathNode*, edge*> edges = {};
public: std::list<PathNode*> furtherNodes;
	  // the postion of the postion
public: std::vector<int> pos;
public: Key key;

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

public: bool operator>(PathNode& other);
public: bool operator<(PathNode& other);
public: bool operator==(PathNode& other);
public: void setKey(float val, int t = 0);
public: void setKey(float val, PathNode*);


public:
	PathNode(std::list<std::pair<PathNode*, short>> connectedNodes, std::vector<int> postion, short&, std::vector<int>ofset = { 0,0,0 });
	PathNode();
	PathNode(std::vector<int>, int);
	PathNode(PathNode*);
	~PathNode();

	std::vector<PathNode*>connectedNodes();
};

class GoalNode{
public: PathNode* Nodeptr;
public: GoalNode* goalThisWay;
public: PathNode* goal;
};

class VisNode {
	PathNode* node;
	std::list<obstacle::Baise*> obstacles;
	
public: VisNode(PathNode* n);
public: ~VisNode();
public: void update(obstacle::Baise* o);
};


inline std::ostream& operator<< (std::ostream& out, const PathNode& next) {
	out << " PathNode ";
	return out;
}

#endif