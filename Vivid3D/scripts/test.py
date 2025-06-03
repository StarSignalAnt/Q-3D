import glm

class TestComp(PyComponent):
    def __init__(self, name="TestComponent"):
        super().__init__(name)
        print("Created PyComponent")
        print("Test Con")
        self.test1 = 0.1
        self.intTest = 25
        self.teststring = "Antony"
    def Update(self,deltaTime):
        print(f"Updated component. DT:{deltaTime}")
        self.node.Turn(glm.vec3(2,1,0))


    
    