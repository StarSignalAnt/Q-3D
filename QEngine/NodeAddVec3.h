#pragma once
#include "NodeAddBase.h"
#include <glm/glm.hpp>
#include <string>

class NodeAddVec3 :
    public NodeAddBase<glm::vec3>
{
public:
    NodeAddVec3();
    std::string GetCategory() const override { return "Maths"; }

};

