#pragma once
#include <glm/glm.hpp>
enum Key {

	Key_W = 1,
	Key_A = 2,
	Key_S = 3,
	Key_D = 4,
	Key_Space = 5

};

class GameInput
{
public:

	static glm::vec2 MousePosition;
	static bool m_Key[255];
	static bool m_Button[16];

private:
};

