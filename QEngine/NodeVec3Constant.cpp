#include "NodeVec3Constant.h"

NodeVec3Constant::NodeVec3Constant() {
    SetName("Vec3 Constant");
    SetTypeName("Vec3 Constant");

    // --- How Data Nodes Work ---
    // 1. We create an INPUT pin. The editable UI widgets in the node editor
    //    are tied to the *default value* of an input pin.
    AddInput("Value", DataType::Vec3);

    // 2. We create a corresponding OUTPUT pin of the same type. This is what
    //    other nodes will connect to.
    AddOutput("Output", DataType::Vec3);
}   