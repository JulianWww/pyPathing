# distutils: language = c++

from libcpp.list cimport list as clist


cdef extern from "pathfinding/generatiors.h":
    clist[int] prim_Generator_C(int, int, int, int)

    
cpdef list[int] prim(int x_size, int y_size, int seed, int zeda):
    return prim_Generator_C(x_size, y_size, seed, zeda)