# distutils: language = c++

from libcpp.set cimport set as cset
from libcpp.string cimport string as cstring
from libcpp.vector cimport vector as cvector
from libcpp.pair cimport pair as cpair
from libcpp.unordered_map cimport unordered_map as cunordered_map
from libcpp.unordered_set cimport unordered_set as cunordered_set

cimport numpy as cnp
import numpy as np

cdef extern from "pathfinding/node.h": 
    cppclass PathNode:
        cvector[int] pos
        #cunordered_map[PathNode*, edge*] edges
        PathNode()
        int id
        bint walkable
        bint oneDirectional
        void setWalkable(bint)
        cvector[int]connectedNodes()

cdef extern from "pathfinding/Edge.h":
    cppclass edge:
        edge()
        edge(PathNode*, PathNode*, float, bint)
        edge(PathNode*, PathNode*)
        #edge(PathNode*, PathNode*)
        float length
        float dirCoefficient

        bint oneDirectional
        PathNode* getNode(bint)

cdef extern from "pathfinding/Cluster.h":
    cppclass Cluster:
        Cluster()
        Cluster(cvector[int])
        Cluster(cvector[cvector[cvector[int]]], short&)
        cunordered_map[int, PathNode*] nodes 

        cvector[int]Astar(int,int,int, bint, int)  
        cvector[int]bfs(int,int, bint)  
        cvector[int]dfs(int,int, bint)

        cvector[int]getNodeKeys()
        cvector[PathNode*]getNodes() 
        cvector[int]postion  
        cvector[int]clusterShape 

        edge* c_getEdge(PathNode*, PathNode*) except +

        void addNode(cvector[int] pos) except +

cdef extern from "pathfinding/node_Graph.h":
    cppclass node_Graph:
        node_Graph()
        node_Graph(cvector[cvector[cvector[int]]], cvector[int], short, int, int)
        cvector[PathNode*]Astar(cvector[int], cvector[int], int)

        cvector[node_Graph*]getLowerKeys()
        cvector[Cluster*]getLowerClusterKeys()

        cunordered_map[int, Cluster*]clusters
        cunordered_map[int, node_Graph*]lowerNodeGraphs
        Cluster* superCluster
        cunordered_set[PathNode*] tempNodes
        int size
        void cleanUp()

        PathNode* getPathNode(cvector[int])

cdef extern from "pathfinding/GoalPathing.h":
    cppclass GoalCluster:
        Cluster* clus
        GoalCluster()
        void buildNodes()
        void buildGraph(int, int) except +
        PathNode* getNextPos(int)

        void setGoal(int)
        PathNode* liveGetNextNode(int, int, unsigned int) except +

cdef extern from "pathfinding/funcs.h":
    edge* makeEdge(PathNode*, PathNode*, float, bint) except +

cdef extern from "pathfinding/DPAstar.h":
    cppclass DPAstarPath:
        DPAstarPath(PathNode*, PathNode*, int, int) except +


        cvector[PathNode*] path
        float connectedNodes
    
