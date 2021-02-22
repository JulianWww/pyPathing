#include "node_Graph.h"
#include "Cluster.h"
#include "scr/jvector.h"
#include <algorithm>
#include "distance.h"
#include <vector>
#include <queue>
#include "Astar.h"
#include "hpA_builders.h"



#define buildClusterPos(x,y,z, arr, size)(x/size + y/size * (arr[0][0].size()/size+1) + z/size*(arr[0][0].size()/size+1)*(arr[0].size()/size+1)+1)
//
node_Graph::node_Graph(std::vector<std::vector<std::vector<int>>> const& arr, int sizes, short movementKey, int bridgeKey) {
	this->size = sizes;
	this->std_init(arr, sizes, movementKey, { 0,0,0 }, bridgeKey);
}

void node_Graph::std_init(std::vector<std::vector<std::vector<int>>> const& arr, int sizes, short& movementKey, std::vector<int> ofset, int singler, int bridgeKey) {
	this->superCluster = new Cluster();
	
	// a matrix used to figure what clusterse are connected
	std::vector<std::tuple<int, int, int>> directionalMatrix = { {0,0,sizes},{0,0,-sizes},{0,sizes,0},{0,-sizes,0},{sizes,0,0},{-sizes,0,0} };
	size_t size = sizes;
	this->size = (int)size;
	// iterate over arr and split it into cub
	for (size_t x_pos = 0; x_pos < arr.size(); x_pos = x_pos + size) {
		for (size_t y_pos = 0; y_pos < arr[0].size(); y_pos = y_pos + size) {
			for (size_t z_pos = 0; z_pos < arr[0][0].size(); z_pos = z_pos + size) {

				std::vector<std::vector<std::vector<int>>> subGraph = jce::vector::slice(arr,
					std::make_pair(x_pos, x_pos + size),
					std::make_pair(y_pos, y_pos + size),
					std::make_pair(z_pos, z_pos + size));

				Cluster* newCluster = new Cluster(subGraph, movementKey, {(int)(z_pos+ofset[2]), (int)(y_pos+ofset[1]), (int)(x_pos+ofset[0]) });
				newCluster->postion = { (int)(x_pos + ofset[0]), (int)(y_pos+ofset[1]), (int)(z_pos+ofset[2]) };
				this->clusters.insert({ buildClusterPos(x_pos, y_pos, z_pos, arr, size), newCluster });

				int idx = 0;
				for (auto iter = directionalMatrix.begin(); iter != directionalMatrix.end(); iter++) {
					long long x_nextPos = std::get<0>(*iter) + (long long)x_pos;
					long long y_nextPos = std::get<1>(*iter) + (long long)y_pos;
					long long z_nextPos = std::get<2>(*iter) + (long long)z_pos;
					if (x_nextPos >= 0 && y_nextPos >= 0 && z_nextPos >= 0) {

						long long newPos = buildClusterPos(x_nextPos, y_nextPos, z_nextPos, arr, size);

						if (this->clusters.count((int)newPos) == (size_t)1) {
							if (distance::manhattan<int>(this->clusters.at((int)newPos)->postion, newCluster->postion) == (float)this->size) {
								buildBridges(newCluster, this->clusters.at((int)newPos), singler, bridgeKey);// , idx, size - 1);
							}
						}

					}
					idx++;
				}
			}
		}
	}
	this->buildSuperNodes();
	this->buildClusterConnections();
	this->superCluster->clusterShape = { (int)(arr[0][0].size() / size), (int)(arr[0].size() / size), (int)(arr.size() / size) };
}

void node_Graph::buildSuperNodes() {
	this->superCluster->nodes.clear();
	for (auto clus = this->clusters.begin(); clus != this->clusters.end(); clus++) {
		int a = 1;
		for (auto conNode = clus->second->intraClusterNodes.begin(); conNode != clus->second->intraClusterNodes.end(); conNode++) {
			(*conNode)->id = (int)this->superCluster->nodes.size();
			this->superCluster->nodes.insert({ this->superCluster->nodes.size(), *conNode });
		}
	}
}

node_Graph::node_Graph(std::vector<std::vector<std::vector<int>>> const& vec, std::vector<int> sizes, int movementKey, int s, int bridgeKey) {
	short key  = (short)movementKey;
	this->buildMulit(vec, sizes, key, { 0,0,0 }, s, bridgeKey);
	//this->buildClusterConnections(this->superCluster);
}
void node_Graph::buildMulit(std::vector<std::vector<std::vector<int>>> const& vec, std::vector<int> sizes, short& movementKey, std::vector<int> ofset, int singler, int bridgeKey) {

	if (sizes.size()==1) {
		this->std_init(vec, sizes[0], movementKey, ofset, singler, bridgeKey);
		return;
	}

	this->superCluster = new Cluster();
	
	// a matrix used to figure what clusterse are connected
	std::vector<std::tuple<int, int, int>> directionalMatrix = { {0,0,sizes[0]},{0,0,-sizes[0]},{0,sizes[0],0},{0,-sizes[0],0},{sizes[0],0,0},{-sizes[0],0,0} };
	size_t size = sizes[0];
	this->size = (int)size;
	auto poses = jce::vector::slice(sizes, std::make_pair(1, sizes.size()));
	// iterate over arr and split it into cub
	size_t x_pos=0, y_pos=0, z_pos = 0;
	for (x_pos = 0; x_pos < vec.size(); x_pos = x_pos + size) {
		for  (y_pos = 0; y_pos < vec[0].size(); y_pos = y_pos + size) {
			for (z_pos = 0; z_pos < vec[0][0].size(); z_pos = z_pos + size) {
				std::vector<std::vector<std::vector<int>>> subGraph = jce::vector::slice(
					vec,
					std::make_pair(x_pos, x_pos + size),
					std::make_pair(y_pos, y_pos + size),
					std::make_pair(z_pos, z_pos + size));

				node_Graph* lower = new node_Graph();
				lower->buildMulit(subGraph, poses, movementKey, { (int)(x_pos + ofset[0]),
																  (int)(y_pos + ofset[1]),
																  (int)(z_pos + ofset[2]) },singler, bridgeKey);
				lower->buildClusterConnections(lower->superCluster);

				//lower->superCluster = this->superCluster;

				this->clusters.insert({ buildClusterPos(x_pos, y_pos, z_pos, vec, size), lower->superCluster });
				lower->superCluster->postion = { (int)x_pos, (int)y_pos, (int)z_pos };
				this->lowerNodeGraphs.insert({ buildClusterPos(x_pos, y_pos, z_pos, vec, size), lower });

				/*int idx = 0;
				for (auto iter = directionalMatrix.begin(); iter != directionalMatrix.end(); iter++) {
					long long x_nextPos = std::get<0>(*iter) + (long long)x_pos;
					long long y_nextPos = std::get<1>(*iter) + (long long)y_pos;
					long long z_nextPos = std::get<2>(*iter) + (long long)z_pos;
					if (x_nextPos >= 0 && y_nextPos >= 0 && z_nextPos >= 0) {

						long long newPos = buildClusterPos(x_nextPos, y_nextPos, z_nextPos, vec, size);

						if (this->clusters.count(newPos) == 1) {
							int d = distance::manhattan<int>(this->clusters.at(newPos)->postion, lower->superCluster->postion);
							if (distance::manhattan<int>(this->clusters.at(newPos)->postion, lower->superCluster->postion) == this->size) {
								buildBridges(lower->superCluster, this->clusters.at(newPos));// , idx, size - 1);
							}
						}

					}
					idx++;*/
				//}
			}
		}
	}
	this->superCluster->clusterShape = { (int)(vec[0][0].size() /size), (int)(vec[0].size()/size), (int)(vec.size()/size) };

	this->buildClusterBridges(singler, bridgeKey);
	this->buildSuperNodes();
	this->buildClusterConnections();
	
	//this->superCluster->clusterShape = { (int)vec.size(), (int)vec[0].size(), (int)vec[0][0].size() };
}

bool isConnected(Cluster* a, Cluster* b, int dis) {
	int p = abs(a->postion[0] - b->postion[0]);
	if (p > dis) {
		return false;
	}
	auto a_iter = a->postion.begin()+1;
	auto b_iter = b->postion.begin()+1;
	float distance = 0.0;
	for (; a_iter != a->postion.end(); a_iter++) {
		distance = distance + abs((*a_iter) - (*b_iter));
		b_iter++;
	}
	return distance == dis;
}

void node_Graph::buildClusterBridges(int singler, int key) {
	for (auto a_cluster = this->clusters.begin(); a_cluster != this->clusters.end(); a_cluster++) {
		for (auto b_cluster = this->clusters.begin(); b_cluster != a_cluster; b_cluster++) {
			if (isConnected(a_cluster->second, b_cluster->second, this->size)) {
				this->buildClusterBridges(a_cluster->second, b_cluster->second, singler, a_cluster->first, b_cluster->first, this, this, key);
				this->buildBridges(a_cluster->second, b_cluster->second, singler, key);
			}
		}
	}
	this->buildSuperNodes();
	for (auto graph = this->lowerNodeGraphs.begin(); graph != this->lowerNodeGraphs.end(); graph++) {
		graph->second->buildClusterConnections();
	}
	this->buildClusterConnections();
}



// build bridges bewean lower level clusters
void node_Graph::buildClusterBridges(Cluster* a, Cluster* b, int singler, int aId, int bId, node_Graph* aGraph, node_Graph* bGraph, int key) {
	if (this->lowerNodeGraphs.size() == 0) {
		subbuildBridges(a, b, singler, aGraph, bGraph, false);
		aGraph->buildSuperNodes();
		bGraph->buildSuperNodes();
	}
	else {
		auto a_graph = aGraph->lowerNodeGraphs.at(aId);
		auto b_graph = bGraph->lowerNodeGraphs.at(bId);
		for (auto a_clus = a_graph->clusters.begin(); a_clus != a_graph->clusters.end(); a_clus++) {
			for (auto b_clus = b_graph->clusters.begin(); b_clus != b_graph->clusters.end(); b_clus++) {
				if (isConnected(a_clus->second, b_clus->second, b_graph->size)) {
					b_graph->buildClusterBridges(a_clus->second, b_clus->second, singler, a_clus->first, b_clus->first, a_graph, b_graph, key);
				}
			}
		}
	}
}

bool isConnected(PathNode* a, PathNode* b) {
	int p = abs(a->pos[2] - b->pos[2]);
	if (p > 1) {
		return false;
	}
	auto a_iter = a->pos.begin();
	auto b_iter = b->pos.begin();
	float distance = 0.0;
	for (; a_iter != a->pos.end()-1; a_iter++) {
		distance = distance + abs((*a_iter) - (*b_iter));
		b_iter++;
	}
	return distance ==1;
}

void subbuildBridges(Cluster* a, Cluster* b, int singler, node_Graph* aGraph, node_Graph* bGraph, bool build_edges, int buildKey) {
	// maby add nodes and deque later
	std::vector<PathNode*> ainterClusterConnections;
	std::vector<PathNode*> binterClusterConnections;
	for (auto a_node = a->nodes.begin(); a_node != a->nodes.end(); a_node++) {
		for (auto b_node = b->nodes.begin(); b_node != b->nodes.end(); b_node++) {
			if (isConnected((*a_node).second, (*b_node).second)) { // redo this to account for diagonal movement should be dooable pass pointers by ref ?
				ainterClusterConnections.push_back((*a_node).second);
				binterClusterConnections.push_back((*b_node).second);
			}
		}
	}
	std::set<PathNode*> aVisited;
	auto a_connectionsIter = ainterClusterConnections.begin();
	auto b_connectionsIter = binterClusterConnections.begin();

	std::unordered_map<PathNode*, PathNode*> a_usedConnectors;
	std::unordered_map<PathNode*, PathNode*> b_usedConnectors;


	for (; a_connectionsIter != ainterClusterConnections.end();) {

		PathNode* currentNode = *a_connectionsIter;
		if (aVisited.find(currentNode) == aVisited.end()) {
			std::queue<PathNode*> poses = std::queue<PathNode*>({ *a_connectionsIter });
			std::unordered_set<PathNode*> atempRes;
			if (buildKey == 0) {
				while (poses.size() > 0) {

					PathNode* currentNode = poses.front(); poses.pop();
					int counter = 0;
					for (auto edge_iter = currentNode->edges.begin(); edge_iter != currentNode->edges.end(); edge_iter++) {
						PathNode* newNode = edge_iter->first;

						if (aVisited.find(newNode) == aVisited.end()) {
							aVisited.insert(newNode);
							if (std::find(ainterClusterConnections.begin(), ainterClusterConnections.end(), newNode) != ainterClusterConnections.end()) {
								poses.push(newNode);
								counter++;
							}
						}
					}
					if (counter == 0) {
						atempRes.insert(currentNode);
					}
				}
			}
			else if (buildKey == 1) {
				atempRes.clear();
				atempRes.insert(*a_connectionsIter);
			}
			if (atempRes.size() < singler) {
				PathNode* na;
				if (aGraph->superCluster->nodes.count((*a_connectionsIter)->id) == 0) {
					na = new PathNode(*a_connectionsIter);
					na->clus = a;

					a->tempBuild.insert({ *a_connectionsIter, na });


					//this->superCluster->nodes.insert({ this->superCluster->nodes.size(), na });
				}
				else {
					na = a->tempBuild.at((*a_connectionsIter));
				}
				PathNode* nb;
				if (b->tempBuild.count(binterClusterConnections[std::find(ainterClusterConnections.begin(), ainterClusterConnections.end(), *a_connectionsIter) - ainterClusterConnections.begin()])) {
					nb = new PathNode(binterClusterConnections[std::find(ainterClusterConnections.begin(), ainterClusterConnections.end(), *a_connectionsIter) - ainterClusterConnections.begin()]);
					nb->clus = b;
					
					b->tempBuild.insert({ binterClusterConnections[std::find(ainterClusterConnections.begin(), ainterClusterConnections.end(), *a_connectionsIter) - ainterClusterConnections.begin()], nb });



					//this->superCluster->nodes.insert({ this->superCluster->nodes.size(), nb });
				}
				else {
					nb = b->tempBuild.at(binterClusterConnections[std::find(ainterClusterConnections.begin(), ainterClusterConnections.end(), *a_connectionsIter) - ainterClusterConnections.begin()]);
				}
				a->intraClusterNodes.insert(*a_connectionsIter);
				b->intraClusterNodes.insert(nb);

				if (build_edges) { new edge(na, nb, true); }
			}
			else {
				for (auto v = atempRes.begin(); v != atempRes.end(); v++) {
					PathNode* n;
					if (b->tempBuild.count(binterClusterConnections[std::find(ainterClusterConnections.begin(), ainterClusterConnections.end(), *v) - ainterClusterConnections.begin()]) == 0) {
						n = new PathNode(binterClusterConnections[std::find(ainterClusterConnections.begin(), ainterClusterConnections.end(), *v) - ainterClusterConnections.begin()]);
						n->clus = b;
						b->tempBuild.insert({ binterClusterConnections[std::find(ainterClusterConnections.begin(), ainterClusterConnections.end(), *v) - ainterClusterConnections.begin()], n });

						//this->superCluster->nodes.insert({ this->superCluster->nodes.size(), n });
						
					}
					else {
						n = b->tempBuild.at(binterClusterConnections[std::find(ainterClusterConnections.begin(), ainterClusterConnections.end(), *v) - ainterClusterConnections.begin()]);
					}
					PathNode* nv;
					if (a->tempBuild.count((*v)) == 0) {
						nv = new PathNode(*v);
						nv->clus = a;
						a->tempBuild.insert({ *v, nv });
						//this->superCluster->nodes.insert({ this->superCluster->nodes.size(), nv });
					}
					else {
						nv = a->tempBuild.at((*v));
					}
					a->intraClusterNodes.insert(nv);
					b->intraClusterNodes.insert(n);

					if (build_edges) { new edge(n, nv, true); }
				}
			}
		}
		a_connectionsIter++; b_connectionsIter++;
	}

	return;
}

void node_Graph::buildBridges(Cluster* a, Cluster* b, int singler, int key) {
	subbuildBridges(a, b, singler, this, this, true, key);
}

void node_Graph::buildClusterConnections() {
	for (auto clus = this->clusters.begin(); clus != this->clusters.end(); clus++) {
		this->buildClusterConnections((*clus).second);
	}
}

bool areNotConnected(PathNode* a, PathNode* b) {
	for (auto con = a->edges.begin(); con != a->edges.end(); con++) {
		if (con->second->nodes.first == b) {
			return false;
		}
		if (con->second->nodes.second == b) {
			return false;
		}
	}
	return true;
}

void node_Graph::buildClusterConnections(Cluster* c) {
	for (auto a_node = c->intraClusterNodes.begin(); a_node != c->intraClusterNodes.end(); a_node++) {
		for (auto b_node = c->intraClusterNodes.begin(); b_node != c->intraClusterNodes.end() && *b_node!=*a_node; b_node++) {
			if ((*a_node) != *b_node) {
				auto path = serchers::Astar_c_node((*a_node)->lowerEquvilant, (*b_node)->lowerEquvilant);
				if (path.size() > 0 && areNotConnected(*a_node, *b_node)) {
					auto e = new edge(*a_node, *b_node, path.back()->distance);
					#if HIGHMEMORY
					std::vector<PathNode*> vec(path.begin(), path.end());
					e->path = vec;
					#endif		
				}
			}
		}
	}
}

std::vector<PathNode*>node_Graph::Astar(std::vector<int> start, std::vector<int> end, int lenth) {
	// hpa* pathfinding 
	PathNode* startNode = this->buildNode(start, this->tempNodes);
	startNode->pos = start;
	PathNode* endNode = this->buildNode(end, this->tempNodes);
	endNode->pos = end;

	if (startNode == NULL || endNode == NULL) {
		return { NULL };
	}

	return this->buildInClusterPath(startNode, endNode, lenth);
}

std::vector<PathNode*>node_Graph::buildPath(PathNode* startNode, PathNode* endNode, int lenth){
	auto path = serchers::Astar_c_node(startNode, endNode);
	PathNode* lastPos = startNode;
	std::vector<PathNode*> res(lenth);
	auto res_iter = res.begin();
	for (auto currentToNode = path.begin(); currentToNode != path.end(); currentToNode++) {
		#if HIGHMEMORY
				buildPath(res, res_iter, lastPos, (*currentToNode));
		#else
				buildPath(res, res_iter, lastPos->lowerEquvilant, (*currentToNode)->lowerEquvilant);
		#endif	
		if (res_iter == res.end()) {
			return res;
		}
		lastPos = *currentToNode;
	}
	#if HIGHMEMORY
		* res_iter = lowerst(endNode);
	#endif

	return res;
}

// get the lowerst level version of the input node
PathNode* lowerst(PathNode* a) {
	if (a->lowerEquvilant == nullptr) {
		return a;
	}
	else {
		return lowerst(a->lowerEquvilant);
	}
}

#if HIGHMEMORY
std::vector<PathNode*> node_Graph::buildInClusterPath(PathNode* node_a, PathNode* node_b, int length) {
	if (node_a->lowerEquvilant == NULL) {
		auto path = serchers::Astar_c_node(node_a, node_b);
		path.push_front(node_a);
		std::vector<PathNode*> res(path.begin(), path.end());

		/*auto path = edge->path;

		std::vector<PathNode*> res(length);
		if (edge->nodes.first == node_a) {
			auto path_itr = path.begin();
			for (auto node_itr = res.begin(); node_itr != res.end() && path_itr != path.end(); (node_itr++, path_itr++)) {
				*node_itr = *path_itr;
			}
		}
		else {
			auto path_itr = path.rbegin();
			for (auto node_itr = res.begin(); node_itr != res.end() && path_itr != path.rend(); (node_itr++, path_itr++)) {
				*node_itr = *path_itr;
			}
		}*/
		return res;
	}
	if (node_a->clus == node_b->clus) {
		auto path = this->buildInClusterPath(node_a->lowerEquvilant, node_b->lowerEquvilant, length);
		if (path.size() == 0 || path[0] == NULL) {
			return this->buildPath(node_a, node_b, length);
		}
		return path;
	}
	return this->buildPath(node_a, node_b, length);
}

void node_Graph::buildPath(std::vector<PathNode*>& res, std::vector<PathNode*>::iterator& res_iter, PathNode* start, PathNode* end) {
	if (start == end) { return; }
	if ((res.begin() == res_iter || lowerst(start) != *(res_iter-1))) {
		*res_iter = lowerst(start);
		res_iter++;
		if (res_iter == res.end()) {
			return;
		}
	}

	std::vector<PathNode*> path;
	for (auto edge = start->edges.begin(); edge != start->edges.end(); edge++) {
		if (edge->first==end) {
			if (edge->second->nodes.second == start) {
				std::vector<PathNode*> vec(edge->second->path.rbegin(), edge->second->path.rend());
				path = vec;
				path.push_back(end->lowerEquvilant);
			}
			else {
				path = edge->second->path;
			}
			break;
		}
	}
	if (path.size() == 0) {
		path = {};
	}
	start = start->lowerEquvilant;
	for (auto Node = path.begin(); Node != path.end(); Node++) {
		if ((*Node)->lowerEquvilant == NULL) {
			if (*(res_iter-1) != lowerst(*Node)) {
				*res_iter = lowerst(*Node);
				res_iter++;
				if (res_iter == res.end()) {
					return;
				}
			}
		}
		else {
			buildPath(res, res_iter, start, (*Node));
		}
		start = *Node;
	}
	return;
}
#else
std::vector<PathNode*> node_Graph::buildInClusterPath(PathNode* node_a, PathNode* node_b, int length) {
	if (node_a->lowerEquvilant == NULL) {
		auto path = serchers::Astar_c_node(node_a, node_b);
		std::vector<PathNode*> res(length);
		auto path_itr = path.begin();
		for (auto node_itr = res.begin(); node_itr != res.end() && path_itr != path.end(); (node_itr++, path_itr++)) {
			*node_itr = *path_itr;
		}
		return res;
	}
	if (node_a->clus == node_b->clus) {
		auto path = this->buildInClusterPath(node_a->lowerEquvilant, node_b->lowerEquvilant, length);
		if (path.size() == 0 || path[0] == NULL) {
			return this->buildPath(node_a, node_b, length);
		}
		return path;
	}
	return this->buildPath(node_a, node_b, length);
}
void node_Graph::buildPath(std::vector<PathNode*>& res, std::vector<PathNode*>::iterator& res_iter, PathNode* start, PathNode* end) {
	auto path = serchers::Astar_c_node(start, end);
	if ((res.begin() == res_iter || lowerst(start) != *(res_iter-1))) {
		*res_iter = lowerst(start);
		res_iter++;
		if (res_iter == res.end()) {
			return;
		}
	}
	for (auto current = path.begin(); current != path.end(); current++) {
		if ((*current)->lowerEquvilant == nullptr) {
			*res_iter = (*current);
			res_iter++;
			if (res_iter == res.end()) {
				return;
			}
		}
		else {
			buildPath(res, res_iter, start->lowerEquvilant, (*current)->lowerEquvilant);
			if (res_iter == res.end()) {
				return;
			}
		}
		start = *current;
	}
	return;
}
#endif
PathNode* node_Graph::buildNode(std::vector<int> pos, std::unordered_set<PathNode*>& temps) {
	// build node for hpa* pathfinding

	auto lowerPos = jce::vector::mod(pos, this->size);
	auto innerPos = jce::vector::div(jce::vector::sub(pos, lowerPos), this->size);
	PathNode* buildingNode = new PathNode();
	temps.insert(buildingNode);

	size_t cluster_id = utils::buildNewPos(innerPos, this->superCluster->clusterShape);
	buildingNode->clus = this->clusters.at(cluster_id);

	if (this->lowerNodeGraphs.size() == 0) {
		if (this->clusters.at(cluster_id) == 0) {
			return NULL;
		}
		this->buildNode(lowerPos, this->clusters.at((int)cluster_id), buildingNode);
	}
	else {
		if (this->lowerNodeGraphs.count(cluster_id) == 0) {
			return NULL;
		}
		auto p = this->lowerNodeGraphs.at(cluster_id)->buildNode(lowerPos, temps);
		buildingNode->lowerEquvilant = p;
		buildingNode->pos = p->pos;
		this->buildNode(lowerPos, this->clusters.at(cluster_id), buildingNode);
	}

	
	return buildingNode;

}

void node_Graph::buildNode(std::vector<int> const & a, Cluster* b, PathNode* Node) {
	// build node connectivity
	PathNode* n;
	if (Node->lowerEquvilant == nullptr) {
		int cluster_id = utils::buildNewNodePos(a, b->clusterShape);  // redo this                             _________________________________________________________
		n = b->nodes.at(cluster_id);
		Node->lowerEquvilant = n;
		Node->pos = Node->lowerEquvilant->pos;
	}
	else {
		n = Node->lowerEquvilant;
	}

	// go throw every trans point ant build the corresponding edge
	for (auto toNode = b->intraClusterNodes.begin(); toNode != b->intraClusterNodes.end(); toNode++) {
		PathNode* o = (*toNode)->lowerEquvilant;
		if (n == o) {
			edge* e = new edge(Node, (*toNode), 0);
			return;
		}
		auto path = serchers::Astar_c_node(n, o);
		if (path.size()!=0) {
			edge* e = new edge(Node, (*toNode), o->distance);
			#if HIGHMEMORY
				std::vector<PathNode*> vec(path.begin(), path.end());
				e->path = vec;
			#endif
		}
	}

	return;
}

void node_Graph::cleanUp() {
	for (auto nodeIter = this->tempNodes.begin(); nodeIter != this->tempNodes.end(); nodeIter++) {
		delete ((*nodeIter));
	}
	this->tempNodes.clear();
}

std::vector<node_Graph*>node_Graph::getLowerKeys() {
	std::vector<node_Graph*>keys = {};
	for (auto itr = this->lowerNodeGraphs.begin(); itr != this->lowerNodeGraphs.end(); itr++) {
		keys.push_back(((*itr).second));
	}
	return keys;
}

std::vector<Cluster*>node_Graph::getLowerClusterKeys() {
	std::vector<Cluster*> keys = {};
	for (auto itr = this->clusters.begin(); itr != this->clusters.end(); itr++) {
		keys.push_back((*itr).second);
	}
	return keys;
}






PathNode* node_Graph::getPathNode(std::vector<int>pos) {
	auto lowerPos = jce::vector::mod(pos, this->size);
	auto innerPos = jce::vector::div(jce::vector::sub(pos, lowerPos), this->size);
	size_t cluster_id = utils::buildNewPos(innerPos, this->superCluster->clusterShape);
	
	if (this->lowerNodeGraphs.size() == 0) {
		auto& clus = this->clusters.at(cluster_id);
		size_t node_id = utils::buildNewNodePos(lowerPos, clus->clusterShape);
		if (clus->nodes.count(node_id) == 0) {
			return NULL;
		}
		return clus->nodes.at(node_id);
	}
	else {
		return this->lowerNodeGraphs.at(cluster_id)->getPathNode(lowerPos);
	}
}





