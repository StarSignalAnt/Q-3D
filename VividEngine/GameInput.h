#pragma once
#include "BasicMath.hpp"

using namespace Diligent;

class GameInput
{
public:

	GameInput() {

		This = this;
		for (int i = 0; i < 512; i++) {
			m_KeyDown[i] = false;
		}

	}

	void SetKey(int id, bool val) {
		if (id < 0) return;
		if (id > 512) return;
		m_KeyDown[id] = val;
	}
	bool GetKey(int id) {

		return m_KeyDown[id];


	}

	static GameInput* This;



	bool m_KeyDown[512];
	float2 m_MousePos;
	float2 m_MouseDelta;

};

