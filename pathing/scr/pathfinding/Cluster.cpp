#include "Cluster.h"
#include "Astar.h"
#include "pathfinders.h"
#include "distance.h"
#include "hpA_builders.h"
#include "node_Graph.h"



Cluster::Cluster(std::vector<std::vector<std::vector<int>>> const& arr, int movementKey, std::vector<int>ofset) {
	short val = (short)movementKey;
	this->init(arr, val, ofset);
}
Cluster::Cluster(std::vector<std::vector<std::vector<int>>> const& arr, short& movementKey, std::vector<int>ofset) {
	this->init(arr, movementKey, ofset);
}

void Cluster::init(std::vector<std::vector<std::vector<int>>> const& arr, short& movementKey, std::vector<int>ofset){
	this->movementMode = movementKey;
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
				if (x_val == 1 && utils::isWalkable(x, y, z_posi, arr)) {
					// go over all the naibours and add them as nabors
					std::list<std::pair<PathNode*, short>> connecteds;
					for (auto direction_pair = directions.begin(); direction_pair != directions.end(); direction_pair++) {
						std::tuple<int, int, int> pos = *direction_pair;
						size_t newPos = utils::buildNewPos(x, y, z_posi, pos, arr);
						// if a node at that matrix pos exist add it to linking queue
						auto obsticleData = movements::furtherMovement(movementKey, arr, { z_posi,y,x }, pos);
						if (createdNodes.count(newPos) != 0 && newPos != 0 && obsticleData.first) {
							connecteds.push_back({ createdNodes[newPos], obsticleData.second});
						}
					}
					// build node and its connections of length 1
					PathNode* n = new PathNode(connecteds, { x,y,z_posi }, movementKey, ofset);
					n->clus = this;
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
	this->clusterShape = { (int)arr[0][0].size(), (int)arr[0].size(), (int)arr.size() };
}

std::vector<int> Cluster::Astar(int start, int end, int poskey, bool getVisited) {
	// perform A* for nodes by id
	auto nodes = serchers::Astar_c_node(this->nodes.at(start), this->nodes.at(end), poskey, getVisited);
	std::vector<int> out;
	for (auto node_iter = nodes.begin(); node_iter != nodes.end(); node_iter++) {
		out.push_back((int)(*node_iter)->id);
	}
	return out;
}

std::vector<int> Cluster::bfs(int start, int end, bool getVisited) {
	// perform bfs for nodes by id
	auto nodes = serchers::bfs_c_node(this->nodes.at(start), this->nodes.at(end), getVisited);
	std::vector<int> out;
	for (auto node_iter = nodes.begin(); node_iter != nodes.end(); node_iter++) {
		out.push_back((int)(*node_iter)->id);
	}
	return out;
}

std::vector<int> Cluster::dfs(int start, int end, bool getVisited) {
	// perform bfs for nodes by id
	auto nodes = serchers::dfs_c_node(this->nodes.at(start), this->nodes.at(end), getVisited);
	std::vector<int> out;
	for (auto node_iter = nodes.begin(); node_iter != nodes.end(); node_iter++) {
		out.push_back((int)(*node_iter)->id);
	}
	return out;
}

std::vector<PathNode*> Cluster::getNodes() {
	std::vector < PathNode* > nodes(this->nodes.size());
	auto inp = nodes.begin();
	for (auto Node = this->nodes.begin(); Node != this->nodes.end(); Node++) {
		*inp = (*Node).second;
		inp++;
	}
	return nodes;
}
std::vector<int> Cluster::getNodeKeys() {
	std::vector<int> nodes(this->nodes.size());
	auto inp = nodes.begin();
	for (auto Node = this->nodes.begin(); Node != this->nodes.end(); Node++) {
		*inp = (*Node).first;
		inp++;
	}
	return nodes;
}

void Cluster::updateConnections() {
	bool updateHigher = false;
	for (auto a_node = this->intraClusterNodes.begin(); a_node != this->intraClusterNodes.end(); a_node++) {
		for (auto b_node = this->intraClusterNodes.begin(); b_node != this->intraClusterNodes.end() && *b_node != *a_node; b_node++) {
			if ((*a_node) != *b_node) {
				auto path = serchers::Astar_c_node((*a_node)->lowerEquvilant, (*b_node)->lowerEquvilant);
				if (path.size() > 0) {
					if (areNotConnected(*a_node, *b_node)) {
						auto e = new edge(*a_node, *b_node, path.back()->distance);
						#if HIGHMEMORY
						std::vector<PathNode*> vec(path.begin(), path.end());
						e->path = vec;
						#endif		
						updateHigher = true;
					}
					else {
						auto e = (*a_node)->edges.at(*b_node);
						e->walkable = true;
						e->length = path.back()->distance;
						#if HIGHMEMORY
						std::vector<PathNode*> vec(path.begin(), path.end());
						e->path = vec;
						#endif
						if (e->length != path.back()->distance) {
							updateHigher = true;
						}
					}
				}
				else {
					auto e = (*a_node)->edges.at(*b_node);
					e->walkable = false;
				}
			}
		}
	}
	if (updateHigher && this->superCluster != NULL) {
		this->superCluster->updateConnections();
	}
}








