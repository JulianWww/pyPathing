import pathing, time
import matplotlib.pyplot as plt
import numpy as np


size=10000

print("generating")
t = time.time()
a = pathing.generators.prim(size+1,size+1, seed=0, reprProb=.1)
print("geration time is", time.time() - t)
#a = np.load("./demoMazes/arr(1001, 1001)_2039720082613.npy")
arr = np.zeros((size+1,size+1, 3))
arr[:,:, 0] = a
arr[:,:, 1] = a
arr[:,:, 2] = a

t = time.time()
b = pathing.search.Astar(a, [0,0], [[size,size]], getScanned=False, fast=True)
print("astar Fast took", time.time()-t)
b2= pathing.search.Astar(a, [0,0], [[size,size]], getScanned=True, fast=True)
arr[0,0,0] = 0
arr[size,size, 0]=0
plt.imshow(arr)
plt.show()

arr[b2[:, 0], b2[:, 1], 1] = 0
arr[b[:, 0], b[:, 1], 0] = 0

plt.imshow(arr)
plt.show()
