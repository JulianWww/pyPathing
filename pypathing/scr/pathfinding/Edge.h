/*
the edge of path betwean 2 nodes
*/
#ifndef Edge
#define Edge

//#include "node.h"
#include <set>
#include "distance.h"
#include "config.h"

// the edge betwean 2 nodes std lenth is 1
class edge {
	// the length of the path betwean the connected nodes
public: float length = 1;
public: short blocks = 0;
public: bool oneDirectional = false;
public: float dirCoefficient = 0;
	  //weather of not it connect clusters
#if HIGHMEMORY
public: std::vector<PathNode*> path = {};
#endif

public: bool connectsClusters = false;
	// the nodes connected by this edge
	// it schould be a pair not set but I cant get it to work
public:  std::pair<PathNode*, PathNode*> nodes = {};
	  // optionaly conains the path of the node

private: void removeFromNode(PathNode*, PathNode*);
public: bool walkable = true;

public:
	edge(PathNode* a, PathNode* b);
	edge(PathNode* a, PathNode* b, bool intra);
	edge(PathNode* a, PathNode* b, float& distance);
	edge(PathNode* a, PathNode* b, float distance, bool oneDirectional); // used by cythons make edge function
	edge() {};
	~edge();
	void updateWalkability(int);
	float getLength(PathNode*, int);
	PathNode* getNode(bool);

	bool isMoveable(PathNode*);
};

#endif