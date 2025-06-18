#include "NodeCreateRotationMatrix.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
NodeCreateRotationMatrix::NodeCreateRotationMatrix() {
    SetName("Create Rotation Matrix");
    SetTypeName("Create Rotation Matrix");

    // --- How Data Nodes Work ---
    // 1. We create an INPUT pin. The editable UI widgets in the node editor
    //    are tied to the *default value* of an input pin.
    AddInput("Delta", DataType::Vec3);

    // 2. We create a corresponding OUTPUT pin of the same type. This is what
    //    other nodes will connect to.
    AddOutput("Output", DataType::Mat4);
}


void NodeCreateRotationMatrix::CalculateOutputs() {

  
    auto rotation = GetInputValue<glm::vec3>("Delta").value();


    auto m_Rotation = glm::mat4(1.0f);

    // Convert to radians and apply rotations in ZYX order
    glm::vec3 radians = glm::radians(rotation);

    m_Rotation = glm::rotate(m_Rotation, radians.z, glm::vec3(0, 0, 1)); // Roll
    m_Rotation = glm::rotate(m_Rotation, radians.y, glm::vec3(0, 1, 0)); // Yaw
    m_Rotation = glm::rotate(m_Rotation, radians.x, glm::vec3(1, 0, 0)); // Pitch

    m_Outputs[0]->SetValue(m_Rotation);


}