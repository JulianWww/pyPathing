import pathing, time
import numpy as np
import matplotlib.pyplot as plt


arr = np.array([pathing.generators.rand_terain(32,32,.1)], dtype=np.int)
arr[0,0,0] = 1
arr[0,30,30] = 1
plt.imshow(arr[0])
plt.show()


clus = pathing.nodeGraph.Cluster()
clus.build(arr, pathing.directions.oneObstacleBlock)

path = clus.runAstar(clus.getnode((0,0,0)), clus.getnode((0,30,30)))

pathing.nodeGraph.debugRender(arr, path)

n = pathing.nodeGraph.nodeGraph()
n.buildFromArr(arr, np.array([8,4]), movement=pathing.directions.oneObstacleBlock)
#print(n)

pathing.nodeGraph.debugRenderCluster(n, arr, colors=["red", "green"])


path = n.Astar(np.array([0,0,0]), np.array([0,30,30]),1000)

pathing.nodeGraph.debugRenderCluster(n, arr, path=path, renderNodes=False)
pathing.nodeGraph.debugRenderCluster(n, arr, path=path, renderNodes=True)
