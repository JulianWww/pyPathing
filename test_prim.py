print("hi")
import pathing
import matplotlib.pyplot as plt
import time
from random import randint
import numpy as np
import jpe_types

seed = jpe_types.conversions.int.convertTointFrombaseStr("Theodor", 64)
a = np.load("speedTest.npy")
#a = np.array([[],[],[]])
astarFast = a[0]
astarSlow = a[1]
Dijkara   = a[2]

plt.plot(Dijkara)
plt.plot(astarSlow)
plt.plot(astarFast)
plt.legend(["Dijkara", "astarSlow", "astarFast"])
plt.show()

for size in range(len(a[0])*10+10, 100000, 10):
    print(size)
    a = pathing.generators.prim(size+1,size+1, seed=seed, reprProb=.5)

    t = time.time()
    b = pathing.search.Astar(a, [0,0], [[size,size]], getScanned=True, fast=False)
    astarSlow = np.append(astarSlow, [time.time()-t])

    t = time.time()
    b = pathing.search.Astar(a, [0,0], [[size,size]], getScanned=True, fast=True)
    astarFast = np.append(astarFast, [time.time()-t])

    t = time.time()
    b = pathing.search.Dijkstra(a, [0,0], [[size,size]], getScanned=True)
    Dijkara = np.append(Dijkara, [time.time()-t])

    np.save("speedTest.npy", np.array([astarFast, astarSlow, Dijkara]))
