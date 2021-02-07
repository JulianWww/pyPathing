#pragma once
#include "node.h"
#include "Edge.h"
#include <vector>
#include <unordered_map>
#include <tuple>
#include <unordered_set>
#include "Astar.h"
#include "pathfinders.h"
#include "distance.h"
#include "hpA_builders.h"


/*
a collection of intaconected nodes
*/

class Cluster {

public: std::unordered_map<size_t, PathNode*> nodes;
	  // the postion of the cluster used by hpA*
public: std::vector<int> postion;
	  // the nodes that are used by hpA during abstract pathing
public: std::unordered_set<PathNode*> intraClusterNodes;
public: std::unordered_map<PathNode*, PathNode*> tempBuild;
	  // the shape of the cluster
public: std::vector<int> clusterShape;


public: Cluster() { return; };
public: Cluster(std::vector<std::vector<std::vector<int>>> const &arr, int movementKey = 0, std::vector<int>ofset = { 0,0,0 }) {
		int x, y;
		int z_posi;	
		this->clusterShape = { (int)arr[0][0].size(), (int)arr[0].size(), (int)arr.size() };
		x = 0;  y = 0;  z_posi = 0;
		  // gen tuple
		  std::vector<std::tuple<int, int, int>> directions = movements::movements(movementKey);
		  std::unordered_map<size_t, PathNode*> createdNodes = {};
		  // the position in the graph
		  

		  for (auto z_iter = arr.begin(); z_iter != arr.end(); z_iter++) {
			  // z axis value
			  std::vector<std::vector<int>> z_val = *z_iter;
			  for (auto y_iter = z_val.begin(); y_iter != z_val.end(); y_iter++) {
				  // y axis value
				  std::vector<int> y_val = *y_iter;
				  for (auto x_iter = y_val.begin(); x_iter != y_val.end(); x_iter++) {
					  // the value of the current node
					  int x_val = *x_iter;
					  if (x_val == 1 && utils::isWalkable(x,y, z_posi, arr)) {
						  // go over all the naibours and add them as nabors
						  std::list<PathNode*> connecteds;
						  for (auto direction_pair = directions.begin(); direction_pair != directions.end(); direction_pair++) {
							  std::tuple<int, int, int> pos = *direction_pair;
							  size_t newPos = utils::buildNewPos(x, y, z_posi, pos, arr);
							  // if a node at that matrix pos exist add it to linking queue
							  if (createdNodes.count(newPos) != 0 && newPos != 0 && movements::furtherMovement(movementKey, arr, { z_posi,y,x }, pos)) {
								  connecteds.push_back(createdNodes[newPos]);
							  }
						  }
						  // build node and its connections of length 1
						  PathNode* n = new PathNode(connecteds, { x,y,z_posi }, ofset);
						  size_t posId = buildPos(x, y, z_posi, arr);
						  createdNodes.insert({ {posId, n} });
						  n->id = posId;
						  this->nodes.insert({ {posId, n} });
					  }
					  x++;
				  }
				  y++; x = 0;
			  }
			  z_posi++; y = 0;
		  }
	  }
	  
	  // perform A* for nodes by id
public:	std::vector<int> Astar(int start, int end, int poskey=0, bool getVisited=false) {
	// perform A* for nodes by id
	auto nodes = serchers::Astar_c_node(this->nodes.at(start), this->nodes.at(end), poskey, getVisited);
	std::vector<int> out;
	for (auto node_iter = nodes.begin(); node_iter != nodes.end(); node_iter++) {
		out.push_back((int)(*node_iter)->id);
	}
	return out;
}
public:	std::vector<int> bfs(int start, int end, bool getVisited = false) {
	// perform bfs for nodes by id
	auto nodes = serchers::bfs_c_node(this->nodes.at(start), this->nodes.at(end), getVisited);
	std::vector<int> out;
	for (auto node_iter = nodes.begin(); node_iter != nodes.end(); node_iter++) {
		out.push_back((int)(*node_iter)->id);
	}
	return out;
}
public:	std::vector<int> dfs(int start, int end, bool getVisited = false) {
	// perform bfs for nodes by id
	auto nodes = serchers::dfs_c_node(this->nodes.at(start), this->nodes.at(end), getVisited);
	std::vector<int> out;
	for (auto node_iter = nodes.begin(); node_iter != nodes.end(); node_iter++) {
		out.push_back((int)(*node_iter)->id);
	}
	return out;
}
}; 