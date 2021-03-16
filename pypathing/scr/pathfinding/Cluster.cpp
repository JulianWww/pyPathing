#include "Cluster.h"
#include "Astar.h"
#include "pathfinders.h"
#include "distance.h"
#include "hpA_builders.h"
#include "node_Graph.h"
#include <stdexcept>
#include "scr/jvector.h"
#include "updateEvent.h"
#include "obstacle.h"




Cluster::Cluster(std::vector<int> size)
{
	this->clusterShape = size;
	this->curentEvent = new updateEvent();
}

Cluster::Cluster(std::vector<std::vector<std::vector<int>>> const& arr, int movementKey, std::vector<int>ofset) {
	short val = (short)movementKey;
	this->init(arr, val, ofset);
	this->curentEvent = new updateEvent();
}
Cluster::Cluster(std::vector<std::vector<std::vector<int>>> const& arr, short& movementKey, std::vector<int>ofset) {
	this->init(arr, movementKey, ofset);
	this->curentEvent = new updateEvent();
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

std::vector<PathNode*> Cluster::Astar(PathNode* start, PathNode* end, int poskey, bool getVisited, int directional) {
	// perform A* for nodes by id
	std::list<PathNode*> nodes;
	if (directional == 0) {
		nodes = serchers::Astar_c_node(start, end, poskey, getVisited);
	}
	else {
		nodes = serchers::Astar_c_node_wspeed(start, end, poskey, getVisited, directional);
	}
	return std::vector<PathNode*>(nodes.begin(), nodes.end());
}

std::vector<PathNode*> Cluster::bfs(PathNode* start, PathNode* end, bool getVisited) {
	// perform bfs for nodes by id
	auto nodes = serchers::bfs_c_node(start, end, getVisited);
	return std::vector<PathNode*>(nodes.begin(), nodes.end());
}

std::vector<PathNode*> Cluster::dfs(PathNode* start, PathNode* end, bool getVisited) {
	// perform bfs for nodes by id
	auto nodes = serchers::dfs_c_node(start, end, getVisited);
	return std::vector<PathNode*>(nodes.begin(), nodes.end());
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

updateEvent* Cluster::updateConnections() {
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

	auto lastUpdateEvent = this->curentEvent;
	this->curentEvent = new updateEvent();
	return lastUpdateEvent;
}

edge* Cluster::c_getEdge(PathNode* a_node, PathNode* b_node)
{
	auto pos = a_node->edges.find(b_node);
	if (pos != a_node->edges.end()) {
		return pos->second;
	}
	throw std::out_of_range("could not find a path betwean the nodes");
}

void Cluster::addNode(std::vector<int>pos)
{
	size_t id = utils::buildNewNodePos(pos, this->clusterShape);
	if (this->nodes.count(id) == 1) {
		throw std::out_of_range("node exists");
	}
	std::vector<int> poses(pos.rbegin(), pos.rend());
	auto newNode = new PathNode(poses, id);
	this->nodes.insert({ id, newNode });

	this->curentEvent->inserts.insert(newNode);
}


VisGraph::VisGraph(Cluster* clus)
{
	for (auto const& n : clus->nodes) {
		VisNode* vis = new VisNode(n.second);
		this->blockNodes.push_back(vis);
	}
}

VisGraph::~VisGraph()
{
	for (auto const& val : this->blockNodes){
		delete val;
	}
}

bool VisGraph::line_of_sight(PathNode* start, PathNode* end)
{
	obstacle::ray r;
	r.set(start->pos, end->pos);
	for (auto const& val : this->obstacles) {
		bool intersects = val->intersects(r);
		if (intersects) {
			return false;
		}
	}
	return true;
}

void VisGraph::updateObstacle(obstacle::Baise* o)
{
	if (std::find(this->obstacles.begin(), this->obstacles.end(), o) == this->obstacles.end()) {
		this->obstacles.push_back(o);
	}
	this->subUpdateObstacle(o);
}

void VisGraph::subUpdateObstacle(obstacle::Baise* o)
{
	for (auto const& n : this->blockNodes) {
		n->update(o);
	}
}

Environment::Environment(): Cluster(), VisGraph(this)
{
}

Environment::Environment(std::vector<int> size) : Cluster(size), VisGraph(this)
{
}

Environment::Environment(std::vector<std::vector<std::vector<int>>> const& arr, int movementKey, std::vector<int> ofset): Cluster(arr, movementKey, ofset), VisGraph(this)
{
}

Environment::Environment(std::vector<std::vector<std::vector<int>>> const& arr, short& movementKey, std::vector<int> ofset): Cluster(arr, movementKey, ofset), VisGraph(this)
{
}

std::vector<PathNode*> Environment::ThetaStar(PathNode* start, PathNode* end, int posKey, bool visited, int speed)
{
	auto path = serchers::ThedaStar(start, end, this, posKey, visited, speed);
	return std::vector<PathNode*>(path.begin(), path.end());
}
