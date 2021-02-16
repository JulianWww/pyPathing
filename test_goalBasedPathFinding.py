import pathing, time, tkinter, random, time, turtle, json
import numpy as np
import matplotlib.pyplot as plt

POINTS = 10
SIZE= 21

POS=(10, 10)

arr = np.array([pathing.generators.rand_terain(SIZE,SIZE,0)], dtype=np.int)

with open("./demoMazes/goalPathing.json") as file:
    arr = np.array(json.load(file))

        
plt.imshow(arr[0]);plt.show()

clus = pathing.nodeGraph.Cluster()
clus.build(arr, pathing.directions.oneObstacleBlock)


goal      = pathing.nodeGraph.goalCluster(clus, True)
goal.goal = clus.getnode((0,*POS))



pathing.nodeGraph.debugRenderDirections(goal, arr, clus)
