import pathing, time
import numpy as np
import matplotlib.pyplot as plt

SIZE = 10

x1 = 0
y1 = 0
x2 = SIZE
y2 = SIZE

arr = np.array([pathing.generators.rand_terain(SIZE+1, SIZE+1, 0)],
               dtype=np.int)
arr[0,0,0] = 1
arr[0,SIZE,SIZE] = 1
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
               np.array([7,5]),
               movement=pathing.directions.oneObstacleBlock,
               buildKey=0)

print("HpA* build", time.time()-t)

t = time.time()
path = clus.runAstar(clus.getnode((0,x1,y1)), clus.getnode((0,x2,y2)), pathing.distance.dijkara)
print("A*", time.time()-t)

pathing.nodeGraph.debugRender(arr, path)

pathing.nodeGraph.debugRenderCluster(n, arr, colors=["red", "green"])


print("howdy")
t = time.time()
path = n.Astar(np.array([0,x1,y1]), np.array([0,x2,y2]),200)
print("HpA*", time.time()-t)


print("lkdjsf")
pathing.nodeGraph.debugRenderCluster(n, arr, path=path, renderNodes=False, pathWidth=6)
pathing.nodeGraph.debugRenderCluster(n, arr, path=path, renderNodes=True,  pathWidth=6)
