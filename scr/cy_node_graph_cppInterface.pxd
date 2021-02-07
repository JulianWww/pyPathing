# distutils: language = c++

from libcpp.set cimport set as cset
from libcpp.string cimport string as cstring
from libcpp.vector cimport vector as cvector
from libcpp.pair cimport pair as cpair
from libcpp.unordered_map cimport unordered_map as cunordered_map
cimport numpy as cnp
import numpy as np

cdef extern from "pathfinding/Edge.h":
    cppclass edge:
        edge()
        #edge(PathNode*, PathNode*)
        float length
        #cpair[PathNode*, PathNode*] nodes

cdef extern from "pathfinding/node.h": 
    cppclass PathNode:
        cvector[int] pos
        cset[edge*] edges
        PathNode()
        int id

cdef extern from "pathfinding/Cluster.h":
    cppclass Cluster:
        Cluster()
        Cluster(cvector[cvector[cvector[int]]], int)
        cunordered_map[int, PathNode*] nodes 

        cvector[int]Astar(int,int,int, bint)  
        cvector[int]bfs(int,int, bint)  
        cvector[int]dfs(int,int, bint)       


