import pathing, time
import numpy as np
import matplotlib.pyplot as plt

arr = np.array([pathing.generators.rand_terain(20,20, .05)], dtype=np.int)
arr[0,10,10] = 0

clus = pathing.nodeGraph.Cluster()
clus.build(arr)

goal      = pathing.nodeGraph.goalCluster(clus)
goal.goal = clus.getnode((0,10,10))


pathing.nodeGraph.debugRenderDirections(goal, arr, clus)
