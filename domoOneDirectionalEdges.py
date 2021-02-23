import pypathing, time, json
import matplotlib.pyplot as plt
import numpy as np

def getConnection(arr, x, y, dx, dy):
    if y+dy < 0 or y+dy >= arr.shape[1] or x+dx < 0 or x+dx >= arr.shape[2]:
        return x, y
    if arr[0, y+dy, x+dx] == 0:
        return x, y

    return getConnection(arr, x+dx, y+dy, dx, dy)
    

with open("./demoMazes/changeOnly.json") as f:
    arr = np.array(json.load(f))


clus = pypathing.nodeGraph.Cluster()
clus.createEmpty(np.array([1,14,14]))

poses = np.where(arr==1)
directions = [(0,1), (1,0), (0,-1), (-1,0)]
for x, y, z in zip(*poses):
    clus.addNode(np.array([x,y,z], dtype=np.int))


pypathing.nodeGraph.debugRenderCluster(clus, arr)
nodes = clus.nodes
for node in nodes.values():
    [z,y,x] = node.position
    for dx, dy in directions:
        newx, newy = getConnection(arr, x, y, dx, dy)
        if (x!=newx or y!=newy):
            otherNode = clus.getnode((0, newy, newx))
            distance = float(abs(x-newx) + abs(y-newy))
            e = pypathing.nodeGraph.edge(node, otherNode, 1, True)



pypathing.nodeGraph.debugRenderCluster(clus, arr)

start = clus.getnode((0, 7,  5))
end   = clus.getnode((0, 0, 13))

path = [start] + clus.runAstar(start, end, distanceKey=pypathing.distance.manhattan)
pypathing.nodeGraph.debugRenderCluster(clus, arr, path=path)
