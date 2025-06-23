import engine

class Channel:
	def __init__(self,id):
		self.id = id;
	def Stop(self):
		engine.stopSoundGameAudio(self.id)
	def SetPitch(self,pitch):
		engine.setPitchGameAudio(self.id,pitch)
	def SetVolume(self,volume):
		engine.setVolumeGameAudio(self.id,volume)


class Sound:
	def __init__(self,path):
		self.ptr = engine.loadSoundGameAudio(path)
	def Play(self):
		return Channel(engine.playSoundGameAudio(self.ptr))

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