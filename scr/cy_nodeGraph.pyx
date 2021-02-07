# distutils: language = c++

cimport cy_node_graph_cppInterface as cppInter
cimport numpy as cnp
import  numpy as np

cdef class DimentionMismatched(Exception):
    "error raised when dimentions dont fit"
cdef class PathingError(Exception):
    "exception raised when no path was found"

## py wrapper for the edge
cdef class PY_edge:
    cdef cppInter.edge c_edge

    def __str__(self):
        return f"edge<len: len(self)>"
    
    @property
    def length(self):
        "the lenth bewean the connected nodes"
        return self.c_edge.length
    
    @length.setter
    def length(self, float val):
        self.c_edge.length = val

#py wraper class for node
cdef class PY_node:
    cdef cppInter.PathNode* c_node
    #def __cinit__(self), cppInter.PathNode node):
    #    self.c_node = node

    ## conversions
    def __str__(self):
        return f"node<edges: {self.c_node.edges.size()}, name: {self.position}, id: {self.id}>"
    def __repr__(self):
        return self.__str__()

    # edge property aces
    @property
    def position(self):
        "the name of the node"
        return np.array(self.c_node.pos)

    @position.setter
    def position(self, cnp.ndarray pos):
        if len(pos) != self.c_node.pos.size():
            raise DimentionMismatched(f"postion must be lenth {self.c_node.pos.size()} not {len(pos)}")
        self.c_node.pos = pos
    # get the edges set
    
    @property
    def edges(self):
        return set()
    
    #property
    @property
    def id(self):
        return self.c_node.id

#py wrapper class for Cluster
cdef class PY_Cluster:
    cdef cppInter.Cluster c_Cluster
    cdef list sizes

    def __cinit__(self):
        """if len(args)>0:
            if isinstance(args[0], np.ndarray):
                if len(args[0].shape) == 3:
                    self.build(*args)
                    self.sizes = list(args[0].shape)
                    return"""
                
        self.c_Cluster = cppInter.Cluster()

    def build(self, arr, int dir=0):
        "build node graph from 3d np array"
        self.c_Cluster = cppInter.Cluster(arr,  dir)
        self.sizes = list(arr.shape)

    @property
    def size(self):
        return self.sizes
    
    #atribute acces for the node tuple
    def getnode(self, poses):
        "get a node assuming the initaialisation was by nd matrix"
        if len(self.sizes) == 0: raise RuntimeError(f"class must be initiated")
        if len(poses) != len(self.sizes): raise DimentionMismatched()

        cdef int identity = 1
        cdef int dimentionidx = 0
        cdef int postition, dimentionSize, sizeMultiplyer;
        sizeMultiplyer = 1
        for postition, dimentionSize in zip(reversed(poses), reversed(self.sizes)):
            if postition >= dimentionSize: raise ValueError(f"pos{postition} out of grid for dimention {dimentionidx} of size {dimentionSize}")
            dimentionidx+=1
            identity += postition*sizeMultiplyer
            sizeMultiplyer *= dimentionSize
        cdef PY_node n = PY_node()
        n.c_node = self.c_Cluster.nodes[identity]
        return n


    def runAstar(self, PY_node start, PY_node end, int distanceKey=0, bint getVisited=False):
        """run A* pathfinding algorythem to find a path from start to end with distanceKey
        """
        cdef a = start.id
        cdef b = end.id
        cdef cnp.ndarray nodeIds
        try:
            nodeIds = np.array(self.c_Cluster.Astar(a, b, distanceKey, getVisited))
        except:
            raise PathingError("no path was found")
        cdef list nodes = []
        cdef PY_node n
        for idx in nodeIds:
            n = PY_node()
            n.c_node = self.c_Cluster.nodes[idx]
            nodes.append(n)
        return nodes
    
    def runBfs(self, PY_node start, PY_node end, bint getVisited=False):
        """run A* pathfinding algorythem to find a path from start to end with distanceKey
        """
        cdef a = start.id
        cdef b = end.id
        cdef cnp.ndarray nodeIds
        try:
            nodeIds = np.array(self.c_Cluster.bfs(a, b, getVisited))
        except:
            raise PathingError("no path was found")
        cdef list nodes = []
        cdef PY_node n
        for idx in nodeIds:
            n = PY_node()
            n.c_node = self.c_Cluster.nodes[idx]
            nodes.append(n)
        return nodes
    
    def runDfs(self, PY_node start, PY_node end, bint getVisited=False):
        """run A* pathfinding algorythem to find a path from start to end with distanceKey
        """
        raise Exception("not functionaly implemented")
        cdef a = start.id
        cdef b = end.id
        cdef cnp.ndarray nodeIds
        try:
            nodeIds = np.array(self.c_Cluster.dfs(a, b, getVisited))
        except:
            raise PathingError("no path was found")
        cdef list nodes = []
        cdef PY_node n
        for idx in nodeIds:
            n = PY_node()
            n.c_node = self.c_Cluster.nodes[idx]
            nodes.append(n)
        return nodes

