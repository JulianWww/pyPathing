"""node Graphs and pathfinding algorythems"""

import pypathing.scr.cy_nodeGraph as cy_node_graph

__all__ = ["nodeGraph", "node", "edgeCls", "edge", "goalGraph", "runDPAstar", "Path", "updateEvent", "Handler", "UpdateHandler"]

nodeGraph = cy_node_graph.PY_BasicNodeGraph
"""A Cluster of nodes witch is basicly just a collection of nodes

        currently only 3 dimentions are suported
        """

node = cy_node_graph.PY_node
"""py node inplementation

        a node is a postion in a pathfinding maze
        it hase its owne ndimentional postion, id and edges connected to it
        """

edgeCls = cy_node_graph.PY_edge
"""py edge implementation

        an edge is a path betwean 2 nodes of a certain lenth
        """
edge = cy_node_graph.makeEdge
"edge factory function"

goalGraph = cy_node_graph.Py_GoalCluster
"""a cluster extension that uses goal based pathfinding
        """

runDPAstar = cy_node_graph.PY_DPAstarPath


updateEvent = cy_node_graph.PY_updateEvent

Handler = cy_node_graph.eventHandler
UpdateHandler = cy_node_graph.updateEventHandler
Path = cy_node_graph.Path



