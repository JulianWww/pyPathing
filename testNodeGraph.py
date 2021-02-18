import pathing, time
import numpy as np
import matplotlib.pyplot as plt
SIZE = 50

arr = np.array([pathing.generators.prim(SIZE+1, SIZE+1, 0.2)],
               dtype=np.int)
arr[0,0,0] = 1
arr[0,SIZE,SIZE] = 1
arr[0,1,1] = 0
arr[0, SIZE, 0] = 1

plt.imshow(arr[0])
plt.show()

t = time.time()
clus = pathing.nodeGraph.Cluster()
clus.build(arr, pathing.directions.oneObstacleBlock)
print("A* build", time.time()-t)

t = time.time()
n = pathing.nodeGraph.nodeGraph()
n.buildFromArr(arr,
               np.array([25, 5]),
               movement=pathing.directions.oneObstacleBlock,
               buildKey=0)

print("HpA* build", time.time()-t)

t = time.time()
path = clus.runAstar(clus.getnode((0,0,0)), clus.getnode((0,SIZE,SIZE)))
print("A*", time.time()-t)

pathing.nodeGraph.debugRender(arr, path)

pathing.nodeGraph.debugRenderCluster(n, arr, colors=["red", "green"])


print("howdy")
t = time.time()
path = n.Astar(np.array([0,SIZE,0]), np.array([0,SIZE,SIZE]),200)
print("HpA*", time.time()-t)


print("lkdjsf")
pathing.nodeGraph.debugRenderCluster(n, arr, path=path, renderNodes=False, pathWidth=3)
pathing.nodeGraph.debugRenderCluster(n, arr, path=path, renderNodes=True,  pathWidth=3)
