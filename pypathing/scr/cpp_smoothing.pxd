from libcpp.vector cimport vector as cvector


cdef extern from "pathfinding/node.h":
    cppclass PathNode:
        cvector[int] pos


cdef extern from "pathfinding/smoothers.h" namespace "smoothing":
    cppclass LagrangePolynomial:
        LagrangePolynomial(cvector[cvector[int]]) except+
        cvector[float] poses(float)
