import glm

class TestComp(PyComponent):
    def __init__(self, name="TestComponent"):
        super().__init__(name)
        self.testSound = Sound("test/song.wav")
        self.chan1 = None 
        self.scene = SceneGraph()
    def Update(self,deltaTime):
        if GameInput.KeyDown(Key.W):
            result = self.scene.RayCast(self.node.GetPosition(),glm.vec3(0,-6,0))
            print(f"X:{result.point.x} Y:{result.point.y} Z:{result.point.z}")
            self.node.SetPosition(result.point+glm.vec3(0.0,0.1,0.0))

        if GameInput.KeyDown(Key.A):
            self.node.Turn(glm.vec3(1,0,0))
    def Stop(self):
        print("Stopped")
    def Play(self):
        print("Playing")
        





        


print("Parsed testcomp...")
    
