import glm

class TestComp(PyComponent):
    def __init__(self,cpp, name="TestComponent"):
        super().__init__(cpp,name)
        self.testSound = Sound("test/song.wav")
        self.chan1 = None 
        self.scene = SceneGraph()
        self.rootNode = self.scene.GetRootNode()
        n1 = self.rootNode[0]
        self.node.SetBodyType(BodyType.ConvexHull)
        self.tester = GraphNode()

    def Update(self,deltaTime):
        self.tester.SetPosition(self.tester.GetPosition()+glm.vec3(0.01,0.0,0.0))
    def Stop(self):
        print("Stopped")
    def Play(self):
     

        print("Playing")
    def Debug(self,node):
        print("Debugging node")

        
        





        


print("Parsed testcomp...")
    
