import engine

class Sound:
	def __init__(self,path):
		self.ptr = engine.loadSoundGameAudio(path)
	def PlaySound(self):
		self.handle = engine.playSoundGameAudio(self.ptr)

	def StopSound(self):
		engine.stopSoundGameAudio(self.handle)

class GameAudio:
	def __init__(self, name="PyComponent"):
		self.name = name
	@staticmethod
	def PlaySong(path):
		print("Playing Song")
		engine.playSongGameAudio(path)
	@staticmethod
	def StopSong():
		engine.stopSongGameAudio()
	@staticmethod
	def LoadSound(path):
		return Sound



print("Parsed GameAudio...")