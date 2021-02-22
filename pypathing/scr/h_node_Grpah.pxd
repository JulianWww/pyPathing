from libcpp.vector cimport cvector
from libcpp.unordered_map cimport cunordered_map
from libcpp.unordered_set cimport cunordered_set


cdef extern from "pathfinding/node_Graph.h":
    cppclass node_Graph:
        node_Graph()
        node_Graph(cvector[cvector[cvector[int]]],         int,  int)
        node_Graph(cvector[cvector[cvector[int]]], cvector[int], int)
        cvector[pathNode*]Astar(cvector[int], cvector[int], int)

        cunordered_map[int, Cluster*]clusters
        Cluster* superCluster
        cunordered_set[PathNode*] tempNodes