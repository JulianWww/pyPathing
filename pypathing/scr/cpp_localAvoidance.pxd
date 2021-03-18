


cdef extern from "pathfinding/obstacle.h" namespace "obstacle":
    cppclass Baise:
        cvector[float] getOrigin()

    cppclass sphere(Baise):
        float r

        sphere(float, cvector[float])