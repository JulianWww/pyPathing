# distutils: language = c++

cimport cy_node_graph_cppInterface as cppInter
from cython.operator cimport dereference as deref, preincrement as inc
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
        return f"node<edges: {deref(self.c_node).edges.size()}, name: {self.position}, id: {self.id}>"
    def __repr__(self):
        return self.__str__()

    # edge property aces
    @property
    def position(self):
        "the name of the node"
        return np.array(deref(self.c_node).pos)

    @position.setter
    def position(self, cnp.ndarray pos):
        if len(pos) != self.c_node.pos.size():
            raise DimentionMismatched(f"postion must be lenth {deref(self.c_node).pos.size()} not {len(pos)}")
        deref(self.c_node).pos = pos
    # get the edges set
    
    @property
    def edges(self):
        return set()
    
    #property
    @property
    def id(self):
        return deref(self.c_node).id
    
    @property
    def connectedNodes(self):
        cdef cppInter.cvector[int] connecteds = self.c_node.connectedNodes()
        return np.array(connecteds)


#py wrapper class for Cluster
cdef class PY_Cluster:
    cdef cppInter.Cluster* c_Cluster
    cdef list sizes

    def __cinit__(self):
        """if len(args)>0:
            if isinstance(args[0], np.ndarray):
                if len(args[0].shape) == 3:
                    self.build(*args)
                    self.sizes = list(args[0].shape)
                    return"""
                
        #self.c_Cluster = *cppInter.Cluster()

    def build(self, arr, int dir=0):
        "build node graph from 3d np array"
        cdef cppInter.Cluster* clus = new cppInter.Cluster(arr,  dir)
        self.c_Cluster = (clus)
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
            sizeMultiplyer*= dimentionSize
            
        if self.c_Cluster.nodes.count(identity) == 0:
            raise ValueError(f"pathNode not found: the postion you where loking at is unwalkable")
        cdef PY_node n = PY_node()
        n.c_node = self.c_Cluster.nodes[identity]
        return n


    def runAstar(self, PY_node start, PY_node end, int distanceKey=0, bint getVisited=False):
        """run A* pathfinding algorythem to find a path from start to end with distanceKey
        """
        cdef a = start.id
        cdef b = end.id
        cdef cnp.ndarray nodeIds

        nodeIds = np.array(self.c_Cluster.Astar(a, b, distanceKey, getVisited))
        if nodeIds.size == 0: raise PathingError(f"no valid path was found")
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
        if nodeIds.size == 0: raise PathingError(f"no valid path was found")
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
        
        if nodeIds.size == 0: raise PathingError(f"no valid path was found")
        cdef list nodes = []
        cdef PY_node n
        for idx in nodeIds:
            n = PY_node()
            n.c_node = self.c_Cluster.nodes[idx]
            nodes.append(n)
        return nodes
    
    def __str__(self):
        return f"Cluster<nodes: {len(self.nodes)}, size = {self.size}>"
    __repr__=__str__

    @property
    def nodes(self):
        res = {}
        cdef cppInter.cvector[int] keys = self.c_Cluster.getNodeKeys()
        cdef int itr
        cdef PY_node n
        for itr in keys:
            n = PY_node()
            n.c_node = self.c_Cluster.nodes[itr]
            res[itr] = n
        return res
    
    @property
    def pos(self):
        return np.array(self.c_node.postion)

#python wrapper for the c++ node Graph class
cdef class Py_nodeGraph():
    """
    an abstaction handler for Clusters
    """
    cdef cppInter.node_Graph* cppHandler
    
    def buildFromArr(self, cnp.ndarray[int, ndim=3] arr, cnp.ndarray[int, ndim=1] sizes, int movement=0, int singler=0):
        cdef cppInter.node_Graph* graph = new cppInter.node_Graph(arr, sizes, movement, singler)
        self.cppHandler = graph
    

    def serch(self, cnp.ndarray[int, ndim=1] start, cnp.ndarray[int, ndim=1] end, int lenght):
        cdef cppInter.cvector[cppInter.PathNode*] nodes = self.cppHandler.Astar(start, end, lenght)
        cdef list Pynodes=[]
        cdef cppInter.PathNode* currentNode
        for currentNode in nodes:
            n = PY_node()
            n.c_node = currentNode
            Pynodes.append(n)
        return Pynodes
    
    def __str__(self):
        return f"abstract node Graph"
    __repr__=__str__

    @property
    def size(self)->list[int]:
        return self.cppHandler.size
    
    @property
    def abstractCluster(self):
        clus = PY_Cluster()
        clus.c_Cluster = self.cppHandler.superCluster
        clus.sizes = list(np.array(clus.c_Cluster.clusterShape) * self.size)
        return clus
    
    @property
    def lowerNodeGraphs(self):
        lowerGrphs = []
        cdef cppInter.cvector[cppInter.node_Graph*] graphs = deref(self.cppHandler).getLowerKeys()
        cdef cppInter.node_Graph* graph
        for graph in graphs:
            g = Py_nodeGraph()
            g.cppHandler = graph
            lowerGrphs.append(g)
        return lowerGrphs
    
    @property
    def clusters(self):
        res = []
        cdef cppInter.cvector[cppInter.Cluster*] clusters = self.cppHandler.getLowerClusterKeys()
        cdef cppInter.Cluster* clus
        for clus in clusters:
            c = PY_Cluster()
            c.c_Cluster = clus
            res.append(c)
        return res

    def Astar(self, cnp.ndarray[int, ndim=1] start, cnp.ndarray[int, ndim=1] end, int length, bint cleanup=True):
        cdef cppInter.cvector[cppInter.PathNode*] path = self.cppHandler.Astar(start, end, length)
        cdef cppInter.cvector[cppInter.PathNode*].iterator itr = path.begin()
        res = []
        if path.size() == 0: raise PathingError(f"no valid path was found")

        while itr != path.end():
            if deref(itr) == NULL:
                return res;
            n = PY_node()
            n.c_node = deref(itr)
            res.append(n)
            inc(itr)

        if cleanup: self.cleanUp()
        return res
    
    def cleanUp(self):
        deref(self.cppHandler).cleanUp()
        return

#py wrapper class for the c++ goal Cluster Class
cdef class Py_GoalCluster():
    cdef cppInter.GoalCluster* c_goal
    cdef PY_node goal
    cdef bint _hasInitiated
    cdef bint _buildLive

    def __cinit__(self, PY_Cluster clus, bint buildLive = False):
        self._buildLive = not buildLive
        self.c_goal = new cppInter.GoalCluster();
        self.c_goal.clus = clus.c_Cluster
        deref(self.c_goal).buildNodes()
        self._hasInitiated=False
    
    @property
    def goal(self):
        """the goal to move to will do all internal builds automaticly
            """
        if not self._hasInitiated:
            return None
        return self.goal
    
    @goal.setter
    def goal(self, PY_node node):
        if self._buildLive:
            print("SET GOAL")
            self.c_goal.setGoal(node.id)
        else:
            self.c_goal.buildGraph(node.id)
        self.goal = node
        self._hasInitiated = True
    
    @property
    def buildLive(self):
        """build required vectors whenen neded
            live building is a diferent way of building the vector field. instead of building a new field when a new goal is se
            this method will update all the required nodesvctors to point to the new goal if they dont point correctly yet otherwise
            the precalculateds will be used"""
        return self._buildLive
    
    @buildLive.setter
    def buildLive(self, bint live):
        self._buildLive = live
        if live == False:
            self.goal = self.goal
    
    def getNext(self, PY_node node, int distanceKey=0):
        """get the next node to move to to get to the goal"""
        cdef cppInter.PathNode* nextNode
        if self._buildLive:
            nextNode = self.c_goal.liveGetNextNode(node.id, distanceKey)
        else:
            nextNode = self.c_goal.getNextPos(node.id)
        if nextNode == NULL:
            raise PathingError(f"no path Found")
        n = PY_node()
        n.c_node = nextNode
        return n





    


