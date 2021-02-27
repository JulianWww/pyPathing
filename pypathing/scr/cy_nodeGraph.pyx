# distutils: language = c++

cimport cy_node_graph_cppInterface as cppInter
from cython.operator cimport dereference as deref, preincrement as inc
cimport numpy as cnp
import  numpy as np


cdef class PathingError(Exception):
    "exception raised when no path was found"

cdef class DimentionMismatched(PathingError):
    "error raised when dimentions dont fit"

cdef class NodeFindingError(PathingError):
    "thrown when the node dose not exist"

cdef class ClusterBuildError(PathingError):
    "thrown when sublusters dont fit"
cdef class SubClustersDontFitError(ClusterBuildError):
    "throw when subcluster sizes dont fit"



#py wraper class for node
cdef class PY_node:
    cdef cppInter.PathNode* c_node
    #def __cinit__(self), cppInter.PathNode node):
    #    self.c_node = node

    ## conversions
    def __str__(self):
        #edges: {deref(self.c_node).edges.size()}, 
        return f"node<name: {self.position}, id: {self.id}, walkable: {self.walkable}>"
    def __repr__(self):
        return self.__str__()

    # edge property aces
    @property
    def position(self)-> cnp.ndarray[int]:
        "the name of the node"
        return np.flip(np.array(deref(self.c_node).pos))

    @position.setter
    def position(self, cnp.ndarray pos) -> void:
        if len(pos) != self.c_node.pos.size():
            raise DimentionMismatched(f"postion must be lenth {deref(self.c_node).pos.size()} not {len(pos)}")
        deref(self.c_node).pos = np.flip(pos)
    # get the edges set
    
    @property
    def edges(self) -> void:
        return set()
    
    #property
    @property
    def id(self) -> int:
        return deref(self.c_node).id
    
    @property
    def walkable(self) -> bint:
        "boolean value weather or not the node is walkable"
        return deref(self.c_node).walkable
    
    @walkable.setter
    def walkable(self, bint NewWalkablility) -> void:
        deref(self.c_node).setWalkable(NewWalkablility)
    
    @property
    def connectedNodes(self) -> cnp.ndarray[int]:
        cdef cppInter.cvector[int] connecteds = self.c_node.connectedNodes()
        return np.array(connecteds)


## py wrapper for the edge
cdef class PY_edge:
    cdef cppInter.edge* c_edge
    cdef bint reverse
    
    def __cinit__(self, reverse=False):
        self.reverse = False

    def __str__(self) -> str:
        return f"""edge<len: {self.length}, nodes: {self.nodes}, moveSpeed: {self.nodeMoves}, reverse: {self.reverse}, oneDirectional: {self.oneDirectional}>"""
    def __repr__(self) -> str:
        return self.__str__()

    @property
    def length(self) -> float:
        "the lenth bewean the connected nodes"
        return self.c_edge.length
    
    @length.setter
    def length(self, float val) -> void:
        self.c_edge.updateLength(val)

    @property
    def nodes(self) -> tuple:
        def getNode(bint val):
            cdef PY_node Pynode = PY_node()
            Pynode.c_node = self.c_edge.getNode(val)
            return Pynode
        
        return getNode(not self.reverse), getNode(self.reverse)
    
    @property
    def nodeMoves(self) -> float:
        "warning this dose not necesarely work with new Nodes"
        if (self.reverse):
            return -self.c_edge.dirCoefficient
        return self.c_edge.dirCoefficient
    
    @nodeMoves.setter
    def nodeMoves(self, float val) -> void:
        if (self.reverse):
            self.c_edge.dirCoefficient = -val
        else:
            self.c_edge.dirCoefficient =  val
    
    @property
    def oneDirectional(self) -> void:
        "weather or not the edge only goas in one direction"
        return self.c_edge.oneDirectional

# py wrapper for the updateEvent struct
cdef class PY_updateEvent:
    cdef cppInter.updateEvent* c_event

    def __str__(self):
        return f"updateEvent <inserts: {self.inserts}, deletions: {self.deletions}>"
    
    @property
    def inserts(self) -> list:
        cdef list nodes = []
        cdef PY_node currentNode
        cdef cppInter.PathNode* node
        cdef cppInter.clist[cppInter.PathNode*] c_nodes = self.c_event.inserts
        for node in c_nodes:
            currentNode = PY_node()
            currentNode.c_node = node
            nodes.append(currentNode)
        return nodes
    
    @property
    def deletions(self) -> list:
        cdef list nodes = []
        cdef PY_node currentNode
        cdef cppInter.PathNode* node
        for node in self.c_event.deletions:
            currentNode = PY_node()
            currentNode.c_node = node
            nodes.append(currentNode)
        return nodes


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
    
    def createEmpty(self, cnp.ndarray[int, ndim=1]sizes):
        pos = np.where(sizes < 1)
        if (pos[0].size !=0):
            raise ValueError(f"no negative values or zeros allowed in sizes")
        self.sizes = list(sizes)
        self.c_Cluster =  new cppInter.Cluster(np.flip(sizes))

    @property
    def size(self):
        return self.sizes
    
    #atribute acces for the node tuple
    def getnode(self, poses) -> PY_node:
        "get a node assuming the initaialisation was by nd matrix"
        if len(self.sizes) == 0: raise RuntimeError(f"class must be initiated")
        if len(poses) != len(self.sizes): raise DimentionMismatched()

        cdef int identity = 1
        cdef int dimentionidx = 0
        cdef int postition, dimentionSize, sizeMultiplyer;
        sizeMultiplyer = 1
        if isinstance(poses, (tuple, list, cnp.ndarray)):
            for postition, dimentionSize in zip(reversed(poses), reversed(self.sizes)):
                if postition >= dimentionSize: raise ValueError(f"pos {postition} out of grid for dimention {dimentionidx} of size {dimentionSize}")
                dimentionidx+=1
                identity += postition*sizeMultiplyer
                sizeMultiplyer*= dimentionSize

        elif isinstance(poses, int):
            identity = poses
        
        else: 
            raise TypeError(f"param poses must be list, tuple or int not {type(poses).__name__}")
            
        if self.c_Cluster.nodes.count(identity) == 0:
            raise ValueError(f"pathNode not found: the postion you where loking at is unwalkable")
        cdef PY_node n = PY_node()
        n.c_node = self.c_Cluster.nodes[identity]
        return n
    
    def getNodes(self, ids) -> list:
        "get nodes form id"
        cdef list res = np.array([])
        for ident in ids:
            res.append(self.getnode(ident))
        return res


    def runAstar(self, PY_node start, PY_node end, int distanceKey=0, bint getVisited=False, int speed=0) -> list:
        """run A* pathfinding algorythem to find a path from start to end with distanceKey
        """
        cdef a = start.id
        cdef b = end.id

        cdef cnp.ndarray[int, ndim=1] nodeIds = np.array(self.c_Cluster.Astar(a, b, distanceKey, getVisited, speed))
        if nodeIds.size == 0: raise PathingError(f"no valid path was found")
        cdef list nodes = []
        cdef PY_node n
        for idx in nodeIds:
            n = PY_node()
            n.c_node = self.c_Cluster.nodes[idx]
            nodes.append(n)
        return nodes
    
    def runDijstara(self, PY_node start, PY_node end, bint getVisited=False, int speed=0) -> list:
        """runn the dijstara algorythem on the cluster"""
        return self.runAstar(start, end, -1, getVisited, speed)
    
    def runBfs(self, PY_node start, PY_node end, bint getVisited=False) -> list:
        """run A* pathfinding algorythem to find a path from start to end with distanceKey
        """
        cdef a = start.id
        cdef b = end.id
        cdef cnp.ndarray[int, ndim=1] nodeIds
        try:
            nodeIds = np.array(self.c_Cluster.bfs(a, b, getVisited))
        except:
            raise PathingError("no path was found")
        if nodeIds.size == 0: raise PathingError(f"no valid path was found")
        cdef list nodes = np.array([])
        cdef PY_node n
        for idx in nodeIds:
            n = PY_node()
            n.c_node = self.c_Cluster.nodes[idx]
            nodes.append(n)
        return nodes
    
    def runDfs(self, PY_node start, PY_node end, bint getVisited=False) -> list:
        """run A* pathfinding algorythem to find a path from start to end with distanceKey
        """
        raise Exception("not functionaly implemented")
        cdef a = start.id
        cdef b = end.id
        cdef cnp.ndarray[int, ndim=1] nodeIds
        try:
            nodeIds = np.array(self.c_Cluster.dfs(a, b, getVisited))
        except:
            raise PathingError("no path was found")
        
        if nodeIds.size == 0: raise PathingError(f"no valid path was found")
        cdef list nodes = np.array([])
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
    
    def getEdge(self, PY_node a, PY_node b) -> PY_edge:
        cdef cppInter.edge* cEdge = deref(self.c_Cluster).c_getEdge(a.c_node, b.c_node);
        cdef PY_edge edge = PY_edge()
        edge.c_edge = cEdge

        cdef cppInter.PathNode* leftNode = cEdge.getNode(True)

        if (leftNode == a.c_node):
            edge.reverse = True

        return edge
    
    def update(self) -> PY_updateEvent:
        event = PY_updateEvent()
        event.c_event = self.c_Cluster.updateConnections()
        return event

cdef class PY_BasicNodeGraph(PY_Cluster):
    "same as cluster but you can add nodes"

    def addNode(self, cnp.ndarray[int, ndim=1] pos):
        for inx, (val, maxv) in enumerate(zip(pos, self.sizes)):
            if val >= maxv:
                raise IndexError(f"pos {val} is out of bounds for dimention {inx+1} of size {maxv}")
            if val < 0:
                raise ValueError(f"cant use negative postions")
        try:
            self.c_Cluster.addNode(pos)
        except IndexError:
            raise IndexError(f"node at postion {pos} already exists")
    
    def getAddNode(self, cnp.ndarray[int, ndim=1] pos):
        try:
            self.addNode(pos)
        finally:
            return self.getnode(tuple(pos))

#python wrapper for the c++ node Graph class
cdef class Py_nodeGraph():
    """
    an abstaction handler for Clusters
    """
    cdef cppInter.node_Graph* cppHandler
    
    def buildFromArr(self, cnp.ndarray[int, ndim=3] arr, cnp.ndarray[int, ndim=1] sizes, short movement=0, int singler=0, int buildKey=0):
        
        cdef int last = sizes[0]
        for lowerDim in sizes[1:]:
            print(lowerDim)
            if last % lowerDim != 0:
                raise SubClustersDontFitError(f"""could not build SubClusters: subClusters dont fit neatyl in Supercluster, supercluster size ({last}) must be multiple of subcluster size ({lowerDim})""")
            last = lowerDim
        
        cdef cppInter.node_Graph* graph = new cppInter.node_Graph(arr, sizes, movement, singler, buildKey)
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

        #check if the node exists
        self.getNode(start)
        self.getNode(end)

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
    
    def getNode(self, cnp.ndarray[int, ndim=1] pos):
        "get a node at postion pos"
        cdef cppInter.PathNode* nextNode
        nextNode = self.cppHandler.getPathNode(pos)
        if nextNode == NULL:
            raise NodeFindingError(f"no node was found at postion {pos}")
        cdef PY_node res = PY_node()
        res.c_node = nextNode
        return res

#py wrapper class for the c++ goal Cluster Class
cdef class Py_GoalCluster():
    cdef cppInter.GoalCluster* c_goal
    cdef PY_node goal
    cdef bint _hasInitiated
    cdef bint _buildLive
    cdef unsigned int _speed

    def __cinit__(self, PY_Cluster clus, bint buildLive = False, speed=0):
        self._speed = speed
        self._buildLive = buildLive
        self.c_goal = new cppInter.GoalCluster();
        self.c_goal.clus = clus.c_Cluster
        self.c_goal.buildNodes()
        self._hasInitiated=False
    
    def __str__(self):
        return f"Goal Pathfinding Graph <goal: {self.goal}, speed: {self.speed}, live build: {self.buildLive}>"
    __rept__ = __str__


    @property
    def goal(self):
        """the goal to move to will do all internal builds automaticly
            """
        if not self._hasInitiated:
            return None
        return self.goal
    
    @goal.setter
    def goal(self, PY_node node):
        "what the ultimate goal of the pathing is"
        self.goal = node
        if self._buildLive:
            self.c_goal.setGoal(node.id)
        else:
            self.update()
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
            nextNode = self.c_goal.liveGetNextNode(node.id, distanceKey, self.speed)
        else:
            nextNode = self.c_goal.getNextPos(node.id)
        if nextNode == NULL:
            raise PathingError(f"no path Found")
        n = PY_node()
        n.c_node = nextNode
        return n

    @property
    def speed(self):
        "sped of the things moving you have to update manualy"
        return self._speed
    
    @speed.setter
    def speed(self, unsigned int newSpeed):
        self._speed = newSpeed
    
    def update(self):
        "update movement"
        self.c_goal.buildGraph(self.goal.id, self.speed)

# paths py wrappers
cdef class PY_Path:
    cdef cppInter.Path* c_path
    cdef list _path

    def __str__(self):
        return f"Path <path: {self.path}, cost: {self.cost}, movementKey: {self.movementKey}"
    __repr__=__str__

    cdef getPath(self):
        self._path = []
        cdef cppInter.PathNode* node
        cdef PY_node Pynode
        self._path = []

        for node in self.c_path.path:
            Pynode = PY_node()
            Pynode.c_node = node
            self._path.append(Pynode)
    
    @property
    def path(self) -> list:
        return self._path
    
    @property
    def cost(self) -> float:
        return self.c_path.cost
    
    @cost.setter
    def cost(self, float val) -> void:
        self.c_path.cost = val
    
    @property
    def movementKey(self) -> int:
        return self.c_path.key


# py wrapper of the DPAstarPath class
cdef class PY_DPAstarPath(PY_Path):
    cdef cppInter.DPAstarPath* c_DPAstarPath
    def __cinit__(PY_DPAstarPath self, PY_node start, PY_node end, int posKey=0, int speed=0):
        self.c_DPAstarPath = new cppInter.DPAstarPath(start.c_node, end.c_node, posKey, speed)
        cdef cppInter.Path* p = self.c_DPAstarPath
        self.c_path = p

        self.getPath()

    def __str__(self):
        return f"DPA* path <path: {self.path}, cost: {self.cost}, movementKey: {self.movementKey}"
    __repr__=__str__

    def update(self, PY_updateEvent event, PY_node current=None, int key=-2):
        if key == -2:
            key = self.movementKey
        if current is None:
            current = self.path[0]
        
        if event is None:#raise error??
            return; 

        self.c_DPAstarPath.update(event.c_event, current.c_node, key)

        self.getPath()


# funcs
def makeEdge(PY_node a, PY_node b, float length, bint oneDirectional):
    if a is None: raise ValueError(f"param a: None is not a valid Node")
    if b is None: raise ValueError(f"param b: None is not a valid Node")

    cdef PY_edge Edge = PY_edge()

    Edge.c_edge = cppInter.makeEdge(a.c_node, b.c_node, length, oneDirectional)
    return Edge




