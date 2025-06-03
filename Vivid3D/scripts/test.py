import glm

class TestComp(PyComponent):
    def __init__(self, name="TestComponent"):
        super().__init__(name)
        print("Created PyComponent")
        print("Test Con")
        self.test1 = 0.1
        self.intTest = 25
        self.testString = "Antony"
    def Update(self,deltaTime):
        print(f"Updated component. DT:{deltaTime}")
        if GameInput.KeyDown(Key.W):
            self.node.Turn(glm.vec3(2,1,0))
        if GameInput.KeyDown(Key.A):
            self.node.Turn(glm.vec3(1,0,0))


print("Parsed testcomp...")
    
