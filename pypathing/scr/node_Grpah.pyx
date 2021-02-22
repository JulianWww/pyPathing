cimport h_node_Grpah as cppExt
from numpy cimport ndarray
import cy_nodeGraph.pyx as cyHandler

cdef class Py_nodeGraph():
    """
    an abstaction handler for Clusters
    """
    cdef cppExt.node_Graph cppHandler
    
    cdef buildFromArr(self, ndarray[int, ndim=3] arr, ndarray[int, ndim=1] sizes, int movement=0):
        self.cppHandler = cppExt.node_Graph(arr, sizes, movement)
    

    cdef serch(self, ndarray[int, ndim=1] start, ndarray[int, ndim=1] end, int lenght):
        cdef cppExt.cvector[PathNode*] nodes = self.cppHandler.Astar(start, end, lenght)
        cdef list Pynodes=[]
        for currentNode in nodes:
            Pynodes.append(cyHandler.PY_node())
            Pynodes[-1].c_node = currentNode
        return Pynodes
