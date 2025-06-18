#pragma once
#include "LGDataNode.h"

template<typename T>
class NodeMultBase : public LGDataNode
{
public:


    NodeMultBase(DataType type, const std::string& nodeName, const std::string& typeName) {
        SetName(nodeName);
        SetTypeName(typeName);

        // All "Add" nodes have two inputs and one output of the same type.
        AddInput("A", type);
        AddInput("B", type);
        AddOutput("Result", type);
    }

    void CalculateOutputs() override {
        // Get the values from the input pins. Default to a zero-equivalent if not connected.
        T valA = GetInputValue<T>("A").value_or(T{});
        T valB = GetInputValue<T>("B").value_or(T{});

        // The '+' operator works automatically for int, float, and glm vectors.
        T result = valA * valB;

        // Set the result on our output pin.
        m_Outputs[0]->SetValue(result);


    }

};

