import pathing, time, tkinter, random, time, turtle, json
import numpy as np
import matplotlib.pyplot as plt

POINTS = 10
SIZE= 21

POS=(10, 10)

arr = np.array([pathing.generators.rand_terain(SIZE,SIZE,0)], dtype=np.int)

        
plt.imshow(arr[0]);plt.show()

clus = pathing.nodeGraph.Cluster()
clus.build(arr, pathing.directions.oneObstacleBlock)


goal      = pathing.nodeGraph.goalCluster(clus, True)
goal.goal = clus.getnode((0,*POS))


print(goal.getNext(clus.getnode((0,0,0))))
pathing.nodeGraph.debugRenderDirections(goal, arr, clus)
