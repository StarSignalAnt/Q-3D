class PyComponent:
    def __init__(self,cpp, name="PyComponent"):
        self.node = GraphNode()
        print("PyCon")
        self.name = name
        self.SetNode(cpp)
    def Update(self,deltaTime):
        pass
    def SetNode(self,CPP):
        self.node.SetCPP(CPP)
        self.node.Populate()
 

print("PyComponent parsed...")