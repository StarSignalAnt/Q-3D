#pragma once
#include "SceneGizmo.h"

class TranslateGizmo : public SceneGizmo
{
public:

	TranslateGizmo();
	void Select(int index) override;
	void Move(glm::vec2 delta) override;

private:
};

