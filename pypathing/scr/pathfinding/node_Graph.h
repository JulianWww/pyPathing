#pragma once
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "node.h";



class Cluster;

class node_Graph {
	/* an abstrackted node graph
	*/
public: std::unordered_map < int, Cluster*> clusters = {};

	  // the abstracted cluster;
public: Cluster* superCluster;
public: int size;
public: std::unordered_map<int, node_Graph*> lowerNodeGraphs = {};
public: node_Graph() {};
	  
	  // the nodes temporarily created and that will be deletet
public: std::unordered_set<PathNode*> tempNodes;


	  // spit the input matrix into its postitions and build the node pathings for arrays
public: node_Graph(std::vector<std::vector<std::vector<int>>> const& vec, int sizes, short movementKey, int bridgeKey);
public: void std_init(std::vector<std::vector<std::vector<int>>> const& vec, int sizes, short& movementKey, std::vector<int >ofset = {0, 0, 0}, int=0, int bridgeKey=0);
public: node_Graph(std::vector<std::vector<std::vector<int>>> const& vec, std::vector<int> sizes, int movementKey, int singler=0, int bridgeKey=0);
private: void buildMulit(std::vector<std::vector<std::vector<int>>> const& vec, std::vector<int> sizes, short& movementKey, std::vector<int> ofset, int singler = 0, int bridgeKey=0);
private: void buildBridges(Cluster* a, Cluster* b, int singler=0, int=0);
private: void buildClusterConnections();
private: void buildClusterConnections(Cluster*);
private: void buildSuperBridges();
	   // build the superclusters nodes
private: void buildSuperNodes();

	   // build bridges bewean lower level clusters
private: void buildClusterBridges(int, int);
	   // build bridges bewean lower level clusters
private: void buildClusterBridges(Cluster*, Cluster*, int, int, int, node_Graph*, node_Graph*, int);

public: std::vector<PathNode*>Astar(std::vector<int> start, std::vector<int> end, int lenth);
private: PathNode* buildNode(std::vector<int> pos, std::unordered_set<PathNode*>&);
private: void buildNode(std::vector<int>const&, Cluster*, PathNode*);
private: void buildPath(std::vector<PathNode*>&, std::vector<PathNode*>::iterator&, PathNode*, PathNode*);

// remove temporary memory data
public: void cleanUp();
public: std::vector<node_Graph*>getLowerKeys();
public: std::vector<Cluster*>getLowerClusterKeys();

public: PathNode* getPathNode(std::vector<int>);
	  
private: std::vector<PathNode*> buildInClusterPath(PathNode*, PathNode*, int);
private: std::vector<PathNode*> buildPath(PathNode*, PathNode*, int);
};

	// subrooten to build connections bewean clusters
void subbuildBridges(Cluster*, Cluster*, int, node_Graph*, node_Graph*, bool, int=0);
PathNode* lowerst(PathNode* a);
bool areNotConnected(PathNode* a, PathNode* b);
