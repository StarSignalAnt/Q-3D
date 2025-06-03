import glm

class TestComp(PyComponent):
    def __init__(self, name="TestComponent"):
        super().__init__(name)
        print("Created PyComponent")
        print("Test Con")
        self.test1 = 0.1
        self.teststring = "Antony"
    def Update(self,deltaTime):
        print(f"Updated component. DT:{deltaTime}")
        self.node.Turn(glm.vec3(0,1,0))
        self.node.SetPosition(self.node.GetPosition() + glm.vec3(self.test1,0,0))
        print(f"TestString:{self.teststring}")

    
    