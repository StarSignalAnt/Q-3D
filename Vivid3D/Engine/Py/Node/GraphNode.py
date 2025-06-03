import engine
import glm

class GraphNode:
    def __init__(self, name="GraphNode"):
        self.position = glm.vec3(0,0,0)
        self.scale = glm.vec3(1,1,1)
        self.rotation = glm.mat4(1.0)
        self.nodes = []
        self.name = "GraphNode"
        print("Created GraphNode")
    def SetC(self,ptr):
        self.cpp = ptr     
        print(f"GraphNode.SetC {ptr}")   
    def AddNode(self,node):
        if isinstance(node,GraphNode):
            self.nodes.append(node)
        else:
            raise TypeError("Only GraphNode instances can be added")
    def GetNodes(self):
        return self.nodes
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


print("GraphNode parsed...")