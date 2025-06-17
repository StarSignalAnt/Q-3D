#include "NodeBoolConstant.h"

NodeBoolConstant::NodeBoolConstant() {

    SetName("Bool Constant");
    SetTypeName("Bool Constant");
    AddInput("Value", DataType::Bool);
    AddOutput("Value", DataType::Bool);

}