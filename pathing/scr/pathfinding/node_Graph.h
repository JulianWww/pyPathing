#pragma once
#include <vector>
#include <unordered_set>
#include <unordered_map>



class Cluster;
class PathNode;


class node_Graph {
	/* an abstrackted node graph
	*/
public: std::unordered_map < size_t, Cluster*> clusters;
private: std::vector<int> clusterSchape;

	  // the abstracted cluster;
public: Cluster* superCluster;
public: int size;
public: std::unordered_map<size_t, node_Graph*> lowerNodeGraphs;
public: node_Graph() {};
	  
	  // the nodes temporarily created and that will be deletet
public: std::unordered_set<PathNode*> tempNodes;


	  // spit the input matrix into its postitions and build the node pathings for arrays
public: node_Graph(std::vector<std::vector<std::vector<int>>> const& vec, int sizes, int movementKey = 0);
public: void std_init(std::vector<std::vector<std::vector<int>>> const& vec, int sizes, int movementKey = 0, std::vector<int >ofset = {0, 0, 0});
public: node_Graph(std::vector<std::vector<std::vector<int>>> const& vec, std::vector<int> sizes, int movementKey=0, int singler=0);
private: void buildMulit(std::vector<std::vector<std::vector<int>>> const& vec, std::vector<int> sizes, int movementKey, std::vector<int> ofset, int singler = 0);
private: void buildBridges(Cluster* a, Cluster* b, int singler=0);
private: void buildClusterConnections();
private: void buildClusterConnections(Cluster*);
	   // build the superclusters nodes
private: void buildSuperNodes();

	   // build bridges bewean lower level clusters
private: void buildClusterBridges(int);
	   // build bridges bewean lower level clusters
private: void buildClusterBridges(Cluster*, Cluster*, int, int, int, node_Graph*, node_Graph*);

public: std::vector<PathNode*>Astar(std::vector<int> start, std::vector<int> end, int lenth);
private: PathNode* buildNode(std::vector<int> pos, std::unordered_set<PathNode*>&);
private: void buildNode(std::vector<int>const&, Cluster*, PathNode*);
private: void buildPath(std::vector<PathNode*>&, std::vector<PathNode*>::iterator&, PathNode*, PathNode*, int);

// remove temporary memory data
public: void cleanUp();
};

	// subrooten to build connections bewean clusters
void subbuildBridges(Cluster*, Cluster*, int, node_Graph*, node_Graph*, bool);
