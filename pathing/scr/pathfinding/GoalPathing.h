#pragma once
#include "Cluster.h"
#include "node.h"
#include<unordered_map>

class GoalCluster{
	// the cluster to operate on
public: Cluster* clus;
public: std::unordered_map<size_t, GoalNode*> nodes;
public: GoalCluster(Cluster* clus);
public: GoalCluster() {};
public: void buildNodes();
public: void buildGraph(int);
private: void build(PathNode*);
public: PathNode* getNextPos(int);
};