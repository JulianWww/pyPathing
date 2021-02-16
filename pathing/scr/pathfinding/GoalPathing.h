#pragma once
#include "Cluster.h"
#include "node.h"
#include<unordered_map>
#include <memory>
#include <queue>

class GoalCluster{
	// the cluster to operate on
public: Cluster* clus;
private: PathNode* goal;
	   // the currently notfinisched nodes
private: std::unordered_map<float, std::unordered_set<PathNode*>> currentNodes;
private: std::priority_queue<float, std::vector<float>, std::greater<float>> nodePriorities;
private: std::unordered_set<PathNode*> visitedNodes;


public: std::unordered_map<size_t, GoalNode*> nodes;
	  // build a new Goal cluster
public: GoalCluster(Cluster* clus);
public: GoalCluster() {};
public: ~GoalCluster();
	  //once given a cluster this will all the neaded Goal nodes
public: void buildNodes();
	  // build node graph pointing towards a goal given by id
public: void buildGraph(int);
private: void build(PathNode*);
	   //get the next node
public: PathNode* getNextPos(int);
	  //set the goal by id
public: void setGoal(int);
	  //get the next node using live sys
public: PathNode* liveGetNextNode(int, int);
private: void liveBuildNextNode(PathNode*, int=0);
};