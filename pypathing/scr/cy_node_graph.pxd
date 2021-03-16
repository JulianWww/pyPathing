cdef class Path:
    @property
    def path(self)
    cdef getPathStr(self)

cdef class StaticPath(Path):
cdef cppInter.cvector[cppInter.PathNode*] c_path_vec
    cdef Environment env
    cdef create(self, cppInter.cvector[cppInter.PathNode*] path, Environment env=None)
