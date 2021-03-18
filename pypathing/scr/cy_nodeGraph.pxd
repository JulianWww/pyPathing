from libcpp.vector cimport vector as cvector

cdef class Path:
    cdef list _path
    cdef getPathStr(self)

cdef cvector[cvector[int]] getPosesFromPath(Path path)

