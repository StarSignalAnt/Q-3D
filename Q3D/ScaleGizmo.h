#pragma once
#include "SceneGizmo.h"
class ScaleGizmo :
    public SceneGizmo
{
public:
	
	ScaleGizmo();
	void Select(int index) override;
	void Move(glm::vec2 delta) override;

};

