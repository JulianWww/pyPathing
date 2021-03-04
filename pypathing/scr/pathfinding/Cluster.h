#pragma once
#include "node.h"
#include "Edge.h"
#include <vector>
#include <unordered_map>
#include <tuple>
#include <unordered_set>
#include <cstdint>


class updateEvent;


/*
a collection of intaconected nodes
*/

class Cluster {

public: std::unordered_map<int, PathNode*> nodes;
	  // the postion of the cluster used by hpA*
public: std::vector<int> postion;
	  // the nodes that are used by hpA during abstract pathing
public: std::unordered_set<PathNode*> intraClusterNodes;
public: std::unordered_map<PathNode*, PathNode*> tempBuild;

public: Cluster* superCluster = NULL;
	  // the shape of the cluster
public: std::vector<int> clusterShape;
public: short movementMode;

public: updateEvent* curentEvent;


public: Cluster() { return; };
public: Cluster(std::vector<int> size);
public: Cluster(std::vector<std::vector<std::vector<int>>> const& arr, int movementKey, std::vector<int>ofset = { 0,0,0 });
public: Cluster(std::vector<std::vector<std::vector<int>>> const& arr, short& movementKey, std::vector<int>ofset = { 0,0,0 });
private: void init(std::vector<std::vector<std::vector<int>>> const& arr, short& movementKey, std::vector<int>ofset);
	  
	  // perform A* for nodes by id
public:	std::vector<PathNode*> Astar(PathNode* start, PathNode* end, int poskey = 0, bool getVisited = false, int speed=0);
public:	std::vector<PathNode*> bfs(PathNode* start, PathNode* end, bool getVisited = false);
public:	std::vector<PathNode*> dfs(PathNode* start, PathNode* end, bool getVisited = false);
public: std::vector<PathNode*> getNodes();
public: std::vector<int>getNodeKeys();

	  //update abstract edges
public: updateEvent* updateConnections();
public: edge* c_getEdge(PathNode*, PathNode*);
public: void addNode(std::vector<int>);
}; 