from setuptools import setup, find_packages, Extension
from Cython.Build import cythonize
from numpy import get_include
from os import system


extensions = [Extension( "pathing.scr.cy_generators", 
                        ["pathing/scr/cy_generators.pyx"]),
                        
              Extension( "pathing.scr.cy_searchers",
                        ["pathing/scr/cy_searchers.pyx"],
                        include_dirs=[get_include()]),

              Extension( "pathing.scr.cy_nodeGraph",
                        [   "pathing/scr/cy_nodeGraph.pyx",
                            "pathing/scr/pathfinding/node_Graph.cpp",
                            "pathing/scr/pathfinding/node.cpp",
                            "pathing/scr/pathfinding/Edge.cpp",
                            "pathing/scr/pathfinding/Cluster.cpp",
                            "pathing/scr/pathfinding/Astar.cpp",
                            "pathing/scr/pathfinding/distance.cpp",
                            "pathing/scr/pathfinding/pathfinders.cpp",
                            "pathing/scr/pathfinding/hpA_builders.cpp",
                            "pathing/scr/pathfinding/GoalPathing.cpp",
                            #"pathing/scr/pathfinding/node_Graph.cpp",
                        ],
                        include_dirs=[get_include()])]


setup(
    ext_modules=cythonize(extensions, annotate=True, force =True, gdb_debug=True)
)

# python36 setup.py build_ext --inplace

# D:\myCode\AGreatYear2021\reposetories\pyPathing
