import pathing
import matplotlib.pyplot as plt
import cv2, time
import numpy as np


size = 100
idx = 0

t = time.time()
a = pathing.generators.prim(size+1,size+1, seed=3)
print("generation took", time.time() - t)
plt.imshow(a)
plt.show()

t = time.time()
juScanned  = pathing.search.Astar(a, (0,0), ((size, size),), getScanned=False, fast=False)
print("ju slow took", time.time()-t)

t = time.time()
juSolution = pathing.search.Astar(a, (0,0), ((size, size),), getScanned=False , fast=True)
print("ju fast took", time.time()-t)

juImg = np.copy(a)

juImg[juSolution[:,0], juSolution[:,1]]=2
juImg[juScanned[:,0], juScanned[:,1]]=3

t = time.time()
movements = [(0, 1), (1, 0), (0, -1), (-1,0)]

pos = (0,0)
end = (size, size)
queue = []
visited = []
initiated = {pos:"end"}


while pos!=end:
    visited.append(pos)
    for dx, dy in movements:
        newPos = pos[0] + dx, pos[1] + dy
        if newPos[0] < a.shape[0] and newPos[1] < a.shape[0]:
            if newPos[0] >= 0 and newPos[1] >= 0 and not newPos in visited and a[newPos]==1:
                queue.append(newPos)
                a[newPos] = 2
                initiated[newPos] = pos

    
    newPos = queue[idx]
    del queue[idx]
    a[pos] = 3
    a[newPos] = 4
    pos = newPos
    
    
loc = pos
path = []
while loc != "end":
    path.append(loc)
    loc = initiated[loc]

for x, y in path:
    a[x, y] = 4
    juImg[x, y] = 4
print("nico took", time.time()-t)
    

plt.imshow(a);plt.show()


plt.imshow(juImg); plt.show()
