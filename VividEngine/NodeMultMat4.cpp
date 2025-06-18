#include "NodeMultMat4.h"
NodeMultMat4::NodeMultMat4()
    : NodeMultBase<glm::mat4>(DataType::Mat4, "Mult (mat4)", "Mult (mat4)")
{
    // That's it! The base template handles all the work.
}