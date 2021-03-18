# distutils: language = c++

try:
    cimport cy_nodeGraph as graph
except ImportError:
    graph = None

cimport cpp_smoothing as inter
from numpy cimport ndarray
from numpy import array, append, poly1d, polyfit, arange, flip

__all__ = ["LagrangePolynomialPath"]

cdef ndarray[int, ndim=2] extendSides(ndarray[int, ndim=2] arr, int d):
    cdef int delta, v, idx
    cdef list sub = []
    cdef list sub2 = []
    for idx in range(arr[0].size):

        delta = arr[0][idx]- arr[1][idx]
        v = arr[0][idx]
        sub.append(v+delta)

        delta = arr[-1][idx]- arr[-2][idx]
        v = arr[-1][idx]
        sub2.append(v+delta)
    
    arr = append(array([sub]), arr, axis=0)
    arr = append(arr, array([sub2]), axis=0)

    if (d > 1):
        return extendSides(arr, d-1)
    return arr

        



cdef class LagrangePolynomialPath():
    cdef graph.Path _path
    cdef list polys
    cdef unsigned int m
    cdef unsigned int straitens

    def __cinit__(self, graph.Path p, unsigned int m=0,  unsigned int straitens=5):
        self._path = p
        self.m = m
        self.straitens = straitens
        if m == 0:
            m = len(p._path)
        self.update(p, m)
    
    cpdef update(self, graph.Path p, int m=0):
        if m == 0:
            m = self.m
        if m == 0:
            m = len(p._path)

        cdef ndarray[int, ndim=2] arr = extendSides(array(graph.getPosesFromPath(p)), self.straitens)
        cdef ndarray[int, ndim=1] x = arange(0, arr.shape[0])-(self.straitens)
        cdef int idx

        self.polys = []
        for idx in range(arr.shape[1]):
            self.polys.append(poly1d(polyfit(x, arr[:, idx], m)))
    
    def getPoint(self, distance):
        cdef ndarray[double, ndim=1] poses = array([])
        for poly in reversed(self.polys):
            poses = append(poses, array(poly(distance)))
        return poses
    
    def getPoints(self, ndarray arr):
        cdef ndarray[double, ndim=2] res = array([self.getPoint(arr[0])])
        cdef ndarray[double, ndim=2] other
        cdef double val
        for val in arr[1:]:
            other = array([self.getPoint(val)])
            res = append(res, other, axis=0)
        return res
    
    @property
    def m(self):
        return self.m
    @m.setter
    def m(self, unsigned int val):
        self.m = val

