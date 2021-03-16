# distutils: language = c++

from libcpp.set cimport set as cset
from libcpp.string cimport string as cstring
from libcpp.vector cimport vector as cvector
from libcpp.list cimport list as clist
from libcpp.pair cimport pair as cpair
from libcpp.unordered_map cimport unordered_map as cunordered_map
from libcpp.unordered_set cimport unordered_set as cunordered_set
from libcpp cimport cast as ccast

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
        cvector[PathNode*]connectedNodes()
    
    cppclass VisNode:
        PathNode* node
        clist[Baise*] obstacles

cdef extern from "pathfinding/Edge.h":
    cppclass edge:
        edge()
        edge(PathNode*, PathNode*, float, bint)
        edge(PathNode*, PathNode*)
        #edge(PathNode*, PathNode*)
        float length
        float dirCoefficient

        bint oneDirectional
        bint walkable
        PathNode* getNode(bint)

        void updateLength(float) except +

cdef extern from "pathfinding/obstacle.h" namespace "obstacle":
    cppclass Baise:
        cvector[float] getOrigin()

    cppclass sphere(Baise):
        float r

        sphere(float, cvector[float])

cdef extern from "pathfinding/Cluster.h":
    cppclass Cluster:
        Cluster()
        Cluster(cvector[int])
        Cluster(cvector[cvector[cvector[int]]], short&) except+
        cunordered_map[int, PathNode*] nodes 

        cvector[PathNode*]Astar(PathNode*,PathNode*,int, bint, int)  
        cvector[PathNode*]bfs(PathNode*, PathNode*, bint)  
        cvector[PathNode*]dfs(PathNode*, PathNode*, bint)

        cvector[int]getNodeKeys()
        cvector[PathNode*]getNodes() 
        cvector[int]postion  
        cvector[int]clusterShape 

        edge* c_getEdge(PathNode*, PathNode*) except +

        void addNode(cvector[int] pos) except +
        updateEvent* updateConnections() except +

    
    cppclass Environment(Cluster):

        Environment()
        Environment(cvector[int])
        Environment(cvector[cvector[cvector[int]]], short&) except+

        cvector[PathNode*]ThetaStar(PathNode*, PathNode*, int, bint, int) except +

        #actual c++ VisGraph class
        clist[VisNode*]blockNodes
        clist[Baise*] obstacles

        bint line_of_sight(PathNode*, PathNode*)
        void updateObstacle(Baise*)

cdef extern from "pathfinding/updateEvent.h":
    cppclass updateEvent:
        cunordered_set[PathNode*] inserts
        cunordered_set[PathNode*] deletions

cdef extern from "pathfinding/node_Graph.h":
    cppclass node_Graph:
        node_Graph()
        node_Graph(cvector[cvector[cvector[int]]], cvector[int], short, int, int)except+
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
        GoalCluster(bint) except +
        void buildNodes() except +
        void buildGraph(int, int) except +
        PathNode* getNextPos(int) except +

        void setGoal(int)
        PathNode* liveGetNextNode(int, int, unsigned int) except +

        void update(updateEvent*, int, int) except +
        void updateDels(updateEvent*, int, int) except +

        bint liveBuild
        void setLiveBuild(bint)

cdef extern from "pathfinding/funcs.h":
    edge* makeEdge(PathNode*, PathNode*, float, bint) except +



cdef extern from "pathfinding/path.h":
    cppclass Path:
        cvector[PathNode*] path
        float cost
        float speed
        int key
        bint valid
    
    cppclass AstarPath(Path):
        AstarPath(PathNode*, PathNode*, int, int) except +

    cppclass DPAstarPath(AstarPath):
        DPAstarPath(PathNode*, PathNode*, int, int) except +
        void update(updateEvent*, PathNode*, int) except +
        void cheapUpdate(updateEvent*, PathNode*, int) except +

    cppclass LPAstarPath(AstarPath):
        LPAstarPath(PathNode*, PathNode*, int, int) except +
        bint update(updateEvent*, PathNode*, int) except +



cdef extern from * nogil:
    sphere* dynamic_cast_sphere_ptr "dynamic_cast<obstacle::sphere*>" (Baise*) except NULL
    
