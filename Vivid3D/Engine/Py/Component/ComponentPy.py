class PyComponent:
    def __init__(self, name="PyComponent"):
        self.node = GraphNode()
        print("PyCon")
        self.name = name
    def Update(self,deltaTime):
        pass
    def SetNode(self,CPP):
        self.node.SetCPP(CPP)
 

print("PyComponent parsed...")