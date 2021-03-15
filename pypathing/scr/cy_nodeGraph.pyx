# distutils: language = c++

cimport cy_node_graph_cppInterface as cppInter
from cython.operator cimport dereference as deref, preincrement as inc
cimport numpy as cnp
import  numpy as np


cdef class PathingError(Exception):
    "exception raised when no path was found"

cdef class DimentionMismatched(PathingError):
    "error raised when dimentions dont fit"

cdef class DimValOutOfRange(DimentionMismatched):
    "raised when value is to big for dimention"

cdef class NodeFindingError(PathingError):
    "thrown when the node dose not exist"

cdef class ClusterBuildError(PathingError):
    "thrown when sublusters dont fit"
cdef class SubClustersDontFitError(ClusterBuildError):
    "throw when subcluster sizes dont fit"

#cy_obstacle
cdef class BaseObstacle:
    cdef cppInter.Baise* ptr
    @property
    def origin(self):
        return np.array(self.ptr.getOrigin())
    
cdef class Sphere(BaseObstacle):
    cdef cppInter.sphere* subptr
    def __cinit__(self, float r, cnp.ndarray[float,ndim=1] pos):
        self.subptr = new cppInter.sphere(r, pos)
        self.ptr = self.subptr
    
    cdef _set(self, cppInter.sphere* s):
        self.ptr = s
        self.subptr = s
    
    @property
    def r(self):
        return self.subptr.r

cdef class eventHandler:
    cdef list to_update

    def __cinit__(self):
        self.to_update = []
        
    def add(self, other):
        self.to_update.append(other)
    
    def remove(self, other):
        self.to_update.remove(other)
    
    @property
    def toUpdate(self):
        """a list of everything that schould be updated"""
        return self.to_update
    
    @toUpdate.setter
    def toUpdate(self, list to_update):
        self.to_update = to_update

cdef class updateEventHandler(eventHandler):
    def add(self, other):
        if not hasattr(other, "update"): raise TypeError(f"{other} has no update method")
        eventHandler.add(self, other)

    cdef update(self, PY_updateEvent event):
        for val in self.to_update:
            try:
                val.update(event)
            except Exception as e:
                print(e, )



#py wraper class for node
cdef class PY_node:
    cdef cppInter.PathNode* c_node
    #def __cinit__(self), cppInter.PathNode node):
    #    self.c_node = node

    ## conversions
    def __str__(self):
        #edges: {deref(self.c_node).edges.size()}, 
        return f"node<name: {self.position}, id: {self.id}, walkable: {self.walkable}>"
    __repr__ = __str__

    # edge property aces
    @property
    def position(self)-> cnp.ndarray[int]:
        "the name of the node"
        return np.flip(np.array(deref(self.c_node).pos))
    
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
    def connectedNodes(self) -> list:
        cdef cppInter.cvector[cppInter.PathNode*] connecteds = self.c_node.connectedNodes()
        cdef list res = []
        cdef cppInter.PathNode* current 
        for current in connecteds:
            node = PY_node()
            node.c_node = current
            res.append(node)
        return res


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
    def oneDirectional(self) -> bint:
        "weather or not the edge only goas in one direction"
        return self.c_edge.oneDirectional
    
    @property
    def walkable(self):
        return self.c_edge.walkable
    
    @walkable.setter
    def walkable(self, bint value):
        self.c_edge.walkable = value

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
        cdef cppInter.cunordered_set[cppInter.PathNode*] c_nodes = self.c_event.inserts

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
        cdef cppInter.cunordered_set[cppInter.PathNode*] c_nodes = self.c_event.deletions

        for node in c_nodes:
            currentNode = PY_node()
            currentNode.c_node = node
            nodes.append(currentNode)
        return nodes


#py wrapper class for Cluster
cdef class PY_Cluster:
    cdef cppInter.Cluster* c_Cluster
    cdef list sizes
    cdef updateEventHandler updateHandler

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
        if not np.ndarray is type(arr): raise TypeError(f"arr must be ndarray not {type(arr).__name__}")
        cdef cppInter.Cluster* clus = new cppInter.Cluster(arr,  dir)
        self.c_Cluster = (clus)
        self.sizes = list(arr.shape)
    
    def createEmpty(self, cnp.ndarray[int, ndim=1]sizes):
        "make empty nodeGraph"
        pos = np.where(sizes < 1)
        if (pos[0].size !=0):
            raise ValueError(f"no negative values or zeros allowed in sizes")
        self.sizes = list(sizes)
        self.c_Cluster =  new cppInter.Cluster(np.flip(sizes))

    
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
                if postition >= dimentionSize or postition < 0: raise DimentionMismatched(f"pos {postition} out of grid for dimention {dimentionidx} of size {dimentionSize}")
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
    getNode = getnode
    
    def getNodes(self, ids) -> list:
        "get nodes form id"
        cdef list res = np.array([])
        for ident in ids:
            res.append(self.getnode(ident))
        return res


    def runAstar(self, PY_node start, PY_node end, int distanceKey=0, bint getVisited=False, int speed=0) -> Path:
        """run A* pathfinding algorythem to find a path from start to end with distanceKey
        """

        cdef cppInter.cvector[cppInter.PathNode*] nodesC = self.c_Cluster.Astar(start.c_node, end.c_node, 
                                                                               distanceKey, getVisited, speed)
        cdef cppInter.PathNode* current

        if nodesC.size() == 0: raise PathingError(f"no valid path was found")
        cdef list nodes = []
        cdef PY_node n
        for current in nodesC:
            n = PY_node()
            n.c_node = current
            nodes.append(n)

        return Path(nodes)
    
    def runDijstara(self, PY_node start, PY_node end, bint getVisited=False, int speed=0) -> Path:
        """runn the dijstara algorythem on the cluster"""
        return self.runAstar(start, end, -1, getVisited, speed)
    
    def runBfs(self, PY_node start, PY_node end, bint getVisited=False) -> Path:
        """run A* pathfinding algorythem to find a path from start to end with distanceKey
        """
        cdef cppInter.cvector[cppInter.PathNode*] Nodes
        cdef cppInter.PathNode* current
        try:
            Nodes = self.c_Cluster.bfs(start.c_node, end.c_node, getVisited)
        except:
            raise PathingError("no path was found")
        if Nodes.size() == 0: raise PathingError(f"no valid path was found")
        cdef list nodes = np.array([])
        cdef PY_node n
        for current in Nodes:
            n = PY_node()
            n.c_node = current
            nodes.append(n)
        return Path(nodes)
    
    def runDfs(self, PY_node start, PY_node end, bint getVisited=False) -> Path:
        """run A* pathfinding algorythem to find a path from start to end with distanceKey
        """
        raise Exception("not functionaly implemented")
        cdef cppInter.cvector[cppInter.PathNode*] Nodes
        cdef cppInter.PathNode* current
        try:
            Nodes = self.c_Cluster.dfs(start.c_node, end.c_node, getVisited)
        except:
            raise PathingError("no path was found")
        if Nodes.size() == 0: raise PathingError(f"no valid path was found")
        cdef list nodes = np.array([])
        cdef PY_node n
        for current in Nodes:
            n = PY_node()
            n.c_node = current
            nodes.append(n)
        return Path(nodes)
    
    def __str__(self):
        return f"Cluster<nodes: {len(self.nodes)}, size = {self.size}>"
    __repr__=__str__

    @property
    def updateHandler(self)->updateEventHandler:
        return self.updateHandler
    @updateHandler.setter
    def updateHandler(self, updateEventHandler handler):
        self.updateHandler = handler
    @updateHandler.deleter
    def updateHandler(self):
        self.updateHandler = None
        
    
    @property
    def size(self):
        return self.sizes

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
        cdef cppInter.edge* cEdge = self.c_Cluster.c_getEdge(a.c_node, b.c_node);
        cdef PY_edge edge = PY_edge()
        edge.c_edge = cEdge

        cdef cppInter.PathNode* leftNode = cEdge.getNode(True)

        if (leftNode == a.c_node):
            edge.reverse = True

        return edge
    
    def update(self) -> PY_updateEvent:
        event = PY_updateEvent()
        event.c_event = self.c_Cluster.updateConnections()

        if self.updateHandler != None:
            self.updateHandler.update(event);
        return event

cdef class PY_BasicNodeGraph(PY_Cluster):
    "same as cluster but you can add nodes"

    def addNode(self, cnp.ndarray[int, ndim=1] pos):
        for inx, (val, maxv) in enumerate(zip(pos, self.sizes)):
            if val >= maxv:
                raise ValueError(f"pos {val} is out of bounds for dimention {inx+1} of size {maxv}")
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

#visual nodeGraph py wrapper.
cdef class VisCluster(object):
    cdef cppInter.VisGraph* c_visClus

    def update_obstacle(self, BaseObstacle other):
        "updates a certain obstacle"
        self.c_visClus.updateObstacle(other.ptr)
    add_opstacle = update_obstacle

    @property
    def opstacles(self):
        res = []
        cdef cppInter.Baise* current

        cdef cppInter.sphere* sphere_test
        cdef Sphere sphere

        for current in self.c_visClus.obstacles:

            sphere_test = cppInter.dynamic_cast_sphere_ptr(current)
            if sphere_test != NULL:
                sphere = Sphere()
                sphere._set(sphere_test)
                res.append(sphere)


            



#python wrapper for the c++ node Graph class to doc
cdef class Py_nodeGraph():
    """
    an abstaction handler for Clusters
    """
    cdef cppInter.node_Graph* cppHandler
    
    def buildFromArr(self, cnp.ndarray[int, ndim=3] arr, cnp.ndarray[int, ndim=1] sizes, short movement=0, int singler=0, int buildKey=0):
        
        cdef int last = sizes[0]
        for lowerDim in sizes[1:]:
            if last % lowerDim != 0:
                raise SubClustersDontFitError(f"""could not build SubClusters: subClusters dont fit neatyl in Supercluster, supercluster size ({last}) must be multiple of subcluster size ({lowerDim})""")
            last = lowerDim
        
        cdef cppInter.node_Graph* graph = new cppInter.node_Graph(arr, sizes, movement, singler, buildKey)
        self.cppHandler = graph
    
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
        return Path(res)
    def serch(self, PY_node start, PY_node end, int length, bint cleanUp=True):
        return self.Astar(start.position, end.position, length, cleanUp)

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
    cdef unsigned int _speed

    def __cinit__(self, PY_BasicNodeGraph clus, bint buildLive = False, unsigned int speed=0):
        self._speed = speed
        self.c_goal = new cppInter.GoalCluster(buildLive);
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
        if self.c_goal.liveBuild:
            self.c_goal.setGoal(node.id)
        else:
            self._update()
        self._hasInitiated = True
    
    @property
    def buildLive(self):
        """build required vectors whenen neded
            live building is a diferent way of building the vector field. instead of building a new field when a new goal is se
            this method will update all the required nodesvctors to point to the new goal if they dont point correctly yet otherwise
            the precalculateds will be used"""
        return self.c_goal.liveBuild
    
    @buildLive.setter
    def buildLive(self, bint live):
        self.c_goal.setLiveBuild(live)
        if live == False:
            self.goal = self.goal
    
    def getNext(self, PY_node node, int distanceKey=0):
        """get the next node to move to to get to the goal"""
        if not node.walkable:
            raise NodeFindingError(f"node {node} is not walkable");

        cdef cppInter.PathNode* nextNode
        if self.buildLive:
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
    
    cdef _update(self):
        "update movement"
        self.c_goal.buildGraph(self.goal.id, self.speed)
    
    def update(self, PY_updateEvent event, int distanceKey=0, int speed=0):
        "update node Graph"
        self.c_goal.update(event.c_event, distanceKey, speed)
    
    def cheapUpdate(self, PY_updateEvent event, int distanceKey=0, int speed=0):
        "only update node graph at needed postions"
        self.c_goal.updateDels(event.c_event, distanceKey, speed)

#basic path
cdef class Path:
    cdef list _path

    def __init__(self, path):
        self._path = path

    def __str__(self):
        return f"Path <path: {self.path}, cost: {self.cost}, movementKey: {self.movementKey}"
    __repr__=__str__

    @property
    def path(self) -> list:
        return self._path

# paths py wrappers
cdef class PY_Path(Path):
    cdef cppInter.Path* c_path
    
    def __init__(self):
        pass

    cpdef getPath(self):
        self._path = []

        if not self.valid:
            raise PathingError(f"no valid path found");

        cdef cppInter.PathNode* node
        cdef PY_node Pynode

        for node in self.c_path.path:
            Pynode = PY_node()
            Pynode.c_node = node
            self._path.append(Pynode)
    
    @property
    def cost(self) -> float:
        return self.c_path.cost
    
    @cost.setter
    def cost(self, float val) -> void:
        self.c_path.cost = val
    
    @property
    def movementKey(self) -> int:
        return self.c_path.key
    
    @property
    def valid(self) -> bint:
        return self.c_path.valid
    
    @property
    def path(self) -> list:
        if not self.valid:
            raise PathingError("latest update run found no valid path.")

        return self._path


# py wrapper of the DPAstarPath class
cdef class PY_DPAstarPath(PY_Path):
    cdef cppInter.DPAstarPath* c_DPAstarPath
    def __init__(self, *args, **kwargs):
        pass
        
    def __cinit__(PY_DPAstarPath self, PY_node start, PY_node end, int posKey=0, int speed=0):
        self.c_DPAstarPath = new cppInter.DPAstarPath(start.c_node, end.c_node, posKey, speed)
        cdef cppInter.Path* p = self.c_DPAstarPath
        self.c_path = p

        self.getPath()

    def __str__(self):
        return f"DPA* path <path: {self.path}, cost: {self.cost}, movementKey: {self.movementKey}"
    def __repr__(self):
        return "DPA* path"

    def update(self, PY_updateEvent event, PY_node current=None, int key=-2, bint cheap=0) -> void:
        if key == -2:
            key = self.movementKey

        if current is None and self.valid:
            current = self.path[0]
        
        if event is None:#raise error??
            return; 
        if cheap:
            self.c_DPAstarPath.cheapUpdate(event.c_event, current.c_node, key)
        else:
            self.c_DPAstarPath.update(event.c_event, current.c_node, key)

        self.getPath()
    
    def cheapUpdate(self, PY_updateEvent event, PY_node current=None, int key=-2) -> void:
        return self.update(event, current, key, True)

#py wrapper for the c++ LPAstarPath class.
cdef class PY_LPAstarPath(PY_Path):
    cdef cppInter.LPAstarPath* c_LPAstarPath

    def __init__(self, *args, **kwargs):
        pass

    def __cinit__ (PY_LPAstarPath self, PY_node start, PY_node end, int posKey=0, int speed=0):
        self.c_LPAstarPath = new cppInter.LPAstarPath(start.c_node, end.c_node, posKey, speed)

        cdef cppInter.Path* p = self.c_LPAstarPath
        self.c_path = p

        self.getPath()
    
    def __str__(self):
        return f"LPA* path <path: {self.path}, cost: {self.cost}, movementKey: {self.movementKey}"
    
    def update(self, PY_updateEvent event, PY_node current=None, int key=-2, bint cheap=0) -> void:
        if key == -2:
            key = self.movementKey

        if current is None and self.valid:
            current = self.path[0]
        
        if event is None:#raise error??
            return; 
        self.c_LPAstarPath.update(event.c_event, current.c_node, key)

        self.getPath()
    
    def cheapUpdate(self, PY_updateEvent event, PY_node current=None, int key=-2) -> void:
        return self.update(event, current, key, True)
# funcs
def makeEdge(PY_node a, PY_node b, float length, bint oneDirectional):
    if a is None: raise ValueError(f"param a: None is not a valid Node")
    if b is None: raise ValueError(f"param b: None is not a valid Node")

    cdef PY_edge Edge = PY_edge()

    Edge.c_edge = cppInter.makeEdge(a.c_node, b.c_node, length, oneDirectional)
    return Edge




