# distutils: language = c++

from libcpp.pair cimport pair as cpair
from libcpp.set cimport set as cset
from libcpp.vector cimport vector as cvec
cimport numpy as np
import numpy as pnp

cdef extern from "pathfinding/searchers.h": 
    cvec[cpair[int, int]] Dijkstra_c(cvec[cvec[int]], cpair[int, int], cset[cpair[int, int]], bint)
    cvec[cpair[int, int]]    Astar_c(cvec[cvec[int]], cpair[int, int], cset[cpair[int, int]], bint, bint)


cpdef np.ndarray Dijkstra_cy(np.ndarray maze, list start, list end, bint getScanned=False):
    return pnp.array(Dijkstra_c(maze, start, end, getScanned))

cpdef np.ndarray Astar_cy(np.ndarray maze, list start, list end, bint getScanned=False, bint fast=False):
    return pnp.array(Astar_c(maze, start, end, getScanned, fast))