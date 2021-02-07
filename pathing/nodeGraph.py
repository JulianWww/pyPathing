"""node graph py wraper see cluster"""

import pathing.scr.cy_nodeGraph as cy_node_graph
from numpy import ndarray, copy as npcopy, array
import typing
from math import sqrt, ceil

class Cluster(cy_node_graph.PY_Cluster):
    """A Cluster of nodes witch is basicly just a collection of nodes

        currently only 3 dimentions are suported
        """
    pass

class node(cy_node_graph.PY_node):
    """py node inplementation

        a node is a postion in a pathfinding maze
        it hase its owne ndimentional postion, id and edges connected to it
        """
    pass

class edge(cy_node_graph.PY_edge):
    """py edge implementation

        an edge is a path betwean 2 nodes of a certain lenth
        """
    pass

def debugRender(arr3d: ndarray, poses: typing.List[node]=[], layers=None):
    import matplotlib.pyplot as plt
    layers = layers if layers!=None else range(0, arr3d.shape[0])
    size = ceil(sqrt(len(layers)))
    f, axarr = plt.subplots(ceil(len(layers)/size), size)
    axarr = array([axarr]).flatten()
    print(axarr)
    for idx, layer in enumerate(layers):
        arr = npcopy(arr3d[layer])
        for pos in poses:
            if pos.position[2] == layer:
                arr[pos.position[1], pos.position[0]] = 2
        
        axarr[idx].imshow(arr)
        axarr[idx].set_title(f"dim {layer}")

    plt.show()
    
