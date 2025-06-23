from enum import Enum


class Key(Enum):
    W = 1
    A = 2
    S = 3
    D = 4
    Space = 5



class GameInput:
    @staticmethod
    def KeyDown(key):
        if engine.getKeyGameInput(key.value):
            return True
        else:
            return False

print("Parsed GameInput...")

