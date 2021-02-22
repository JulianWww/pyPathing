import pypathing, time, tkinter, random, time, turtle, json
import numpy as np
import matplotlib.pyplot as plt

POINTS = 20
SIZE= 21

POS=(10, 10)

arr = np.array([pypathing.generators.rand_terain(SIZE,SIZE,0)], dtype=np.int)
print(list(arr))

        
plt.imshow(arr[0]);plt.show()

clus = pypathing.nodeGraph.Cluster()
clus.build(arr, 1)

e  = clus.getEdge(clus.getnode((0,0,0)), clus.getnode((0,0,1)))
e2 = clus.getEdge(clus.getnode((0,0,1)), clus.getnode((0,0,0)))
e.nodeMoves = -1


goal      = pypathing.nodeGraph.goalCluster(clus, True)
goal.goal = clus.getnode((0,*POS))

print(goal)
goal.speed = 1
print(goal)

goal.update()


pypathing.nodeGraph.debugRenderDirections(goal, arr, clus)
