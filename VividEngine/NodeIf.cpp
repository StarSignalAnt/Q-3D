#include "NodeIf.h"
#include <iostream>

NodeIf::NodeIf() {

    SetName("If");
    SetTypeName("If");

    // An If node has one boolean condition
    AddInput("Condition", DataType::Bool);

    // And two execution outputs
    AddExecOutput("True");
    AddExecOutput("False");

}


void NodeIf::Exec()
{
    // 1. Get the value from the "Condition" input pin.
    auto conditionOpt = GetInputValue<bool>("Condition");

    // 2. Default to false if not connected or type is wrong.
    bool condition = conditionOpt.value_or(false);

    std::cout << "If Node: Condition is " << (condition ? "True" : "False") << std::endl;

    // 3. Fire the correct execution output pin based on the condition.
    if (condition) {
        ExecNext(0); // Fire the "True" pin (index 0)
    }
    else {
        ExecNext(1); // Fire the "False" pin (index 1)
    }
}