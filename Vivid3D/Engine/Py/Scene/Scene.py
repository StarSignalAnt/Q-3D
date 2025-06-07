import engine
import glm

class CastResult:
	def __init__(self):
		self.hit = False
		self.point = glm.vec3(0.0,0.0,0.0)
		self.distance = 0.0
		self.hitNode = None
		self.normal = glm.vec3(0.0,0.0,0.0)

class SceneGraph:
	def __init__(self):
		self.scene = engine.getScene()
	def RayCast(self,position,dir):
		return engine.rayCast(position,dir)
		

