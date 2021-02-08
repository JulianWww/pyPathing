import pathing, time
import numpy as np

arr = np.array([pathing.generators.rand_terain(20,20, .5),
                pathing.generators.rand_terain(20,20, .4),
                pathing.generators.rand_terain(20,20, 0)])
arr[0,0,0]=1
arr[0,19,19] = 1
pathing.nodeGraph.debugRender(arr, [])

c = pathing.nodeGraph.Cluster()
print("hi")
c.build(arr, pathing.directions.noDiagonal)
print("built")

a = c.getnode((0,19,19))
b = c.getnode((0,0,0))
print(a, b)

t = time.time()
d = c.runAstar(a,
               b,
               distanceKey = pathing.pathfinding.fastDiagonal, getVisited=False)

print(time.time() - t)




pathing.nodeGraph.debugRender(arr, d)
