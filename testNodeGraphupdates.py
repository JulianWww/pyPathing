import pathing, time
import numpy as np
import matplotlib.pyplot as plt

SIZE = 20

x2 = 0
y2 = 0
x1 = SIZE-1
y1 = 0

arr = np.array([pathing.generators.rand_terain(SIZE, SIZE, 0)],
               dtype=np.int)

t = time.time()
n = pathing.nodeGraph.nodeGraph()
n.buildFromArr(arr,
               np.array([5]),
               movement=pathing.directions.oneObstacleBlock,
               buildKey=0)

print("HpA* build", time.time()-t)




print("lkdjsf")
pathing.nodeGraph.debugRenderCluster(n, arr, renderNodes=True, pathWidth=6)

for x in range(SIZE-1):
    Node = n.getNode(np.array([0,3,x]))
    arr[0,3,x] = 0
    Node.walkable = False

path = n.Astar(np.array([0,x1,y1]), np.array([0, x2, y2]), 100)

pathing.nodeGraph.debugRenderCluster(n, arr, path=path, renderNodes=True,  pathWidth=6)
