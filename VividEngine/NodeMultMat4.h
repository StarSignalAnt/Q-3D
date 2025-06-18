#pragma once
#include "NodeMultBase.h"
#include <glm/glm.hpp>
class NodeMultMat4 :
    public NodeMultBase<glm::mat4>
{
public:
    NodeMultMat4();
    std::string GetCategory() const override { return "Maths"; }


};

