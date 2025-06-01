#pragma once
#include "SceneGizmo.h"
class RotateGizmo :
    public SceneGizmo
{
public:

	RotateGizmo();
	void Select(int index) override;
	void Move(glm::vec2 delta) override;
};

