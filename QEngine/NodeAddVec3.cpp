#include "NodeAddVec3.h"
NodeAddVec3::NodeAddVec3()
    : NodeAddBase<glm::vec3>(DataType::Vec3, "Add(Vec3)", "Add(Vec3)")
{
    // That's it! The base template handles all the work.
}