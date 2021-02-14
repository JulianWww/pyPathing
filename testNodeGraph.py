import pathing, time
import numpy as np
import matplotlib.pyplot as plt

arr = np.ones((2,16,16), dtype=np.int)
arr = np.array([pathing.generators.rand_terain(16,16, 0.2)], dtype=np.int)
arr[0,0,0] = 1
arr[0,15,15] = 1
plt.imshow(arr[0])
plt.show()

n = pathing.nodeGraph.nodeGraph()
n.buildFromArr(arr, np.array([8,4]), movement=pathing.directions.towObstacleBlock)
#print(n)

pathing.nodeGraph.debugRenderCluser(n, arr, colors=["red", "green"])


path = n.Astar(np.array([0,0,0]), np.array([0,15,15]), 100)
pathing.nodeGraph.debugRenderCluser(n, arr, path=path, renderNodes=False)
pathing.nodeGraph.debugRenderCluser(n, arr, path=path, renderNodes=True)
