import engine
import glm

from enum import Enum


class BodyType(Enum):
    Box = 0
    Sphere = 1
    ConvexHull = 2
    TriMesh = 3
    TerrainMesh = 4
    Nill = 5

class GraphNode:
    def __init__(self, name="GraphNode"):
        self.position = glm.vec3(0,0,0)
        self.scale = glm.vec3(1,1,1)
        self.rotation = glm.mat4(1.0)
        self.nodes = []
        self.name = name
        self.cpp = 0
        print("Created GraphNode")
    def SetBodyType(self,type):
        engine.setBodyType(self,type.value)
    def SetC(self,ptr):
        self.cpp = ptr     
        print(f"GraphNode.SetC {ptr}")      
    def Populate(self):
        nodeCount = engine.getNodeCount(self)
        for i in range(nodeCount):
            lNode = engine.getSubNode(self,i)
            lNode.Populate()
            self.nodes.append(lNode)
            print("Added node")
        print(f"Pop NC:{nodeCount}")

    def AddNode(self,node):
        if isinstance(node,GraphNode):
            self.nodes.append(node)
        else:
            raise TypeError("Only GraphNode instances can be added")
    def GetNodes(self):
        return self.nodes
    def GetNodeCount(self):
        return len(self.nodes)
    def GetPosition(self):
        self.position = engine.getPositionGraphNode(self.cpp)
        return self.position

    def SetPosition(self,position):
        self.position = position
        engine.updateGraphNode(self.cpp,self.position)
    def Turn(self,delta):
        engine.turnGraphNode(self.cpp,delta)

    def GetRotation(self):
        self.rotation = engine.getRotationGraphNode(self.cpp)
        return self.rotation
    def SetRotation(self,rotation):
        engine.setRotationGraphNode(self.cpp,rotation)
        self.rotation = rotation

    def SetCPP(self,cobj):
        self.cpp = cobj
        print(f"node.SetCPP {cobj}")
    def __eq__(self, other):
        return self.cpp == other.cpp
    def __getitem__(self, index):
        # You can customize this however you want
        print(f"Index:{index}")
        return self.nodes[index]
    def GetComponent(self,name):
        return engine.getComponent(self.cpp,name)
        print("Getting component")


print("GraphNode parsed...")