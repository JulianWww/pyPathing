import pathing, time
import numpy as np
import matplotlib.pyplot as plt
SIZE = 100

arr = np.array([pathing.generators.rand_terain(SIZE+1,SIZE+1, .2)], dtype=np.int)
arr[0,0,0] = 1
arr[0,SIZE,SIZE] = 1
arr[0,1,1] = 1
plt.imshow(arr[0])
plt.show()


clus = pathing.nodeGraph.Cluster()
clus.build(arr, pathing.directions.oneObstacleBlock)

t = time.time()
n = pathing.nodeGraph.nodeGraph()
n.buildFromArr(arr, np.array([10]), movement=pathing.directions.oneObstacleBlock)
print(time.time()-t)

node = n.getNode(np.array([0,1,1]))
node.walkable=False

node = clus.getnode((0,1,1))
node.walkable = False
arr[0,1,1]=0

t = time.time()
path = clus.runAstar(clus.getnode((0,0,0)), clus.getnode((0,SIZE,SIZE)))
print("A*", time.time()-t)

pathing.nodeGraph.debugRender(arr, path)

#pathing.nodeGraph.debugRenderCluster(n, arr, colors=["red", "green"])

def test():
    t = time.time()
    path = n.Astar(np.array([0,0,0]), np.array([0,SIZE,SIZE]),10)
    print("HpA*", time.time()-t)

    for node in path:
        print(node)

    pathing.nodeGraph.debugRenderCluster(n, arr, path=path, renderNodes=False)
    pathing.nodeGraph.debugRenderCluster(n, arr, path=path, renderNodes=True)

test()
