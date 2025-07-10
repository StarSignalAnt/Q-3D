#pragma once

#include "LGDataNode.h"
#include "LGLogicNode.h"
#include "GraphNode.h"
#include "SceneGraph.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/norm.hpp>
#include <iostream>
#include <string>
#include "NodeAddBase.h"
#include "NodeMultBase.h"
#include <sstream>

// --- Base Template Nodes (for reusability) ---

/**
 * @brief Base template for subtraction operations (A - B).
 * Works with float, int, vec2, vec3, vec4.
 */
template<typename T>
class NodeSubtractBase : public LGDataNode {
public:
    NodeSubtractBase(DataType type, const std::string& nodeName, const std::string& typeName) {
        SetName(nodeName);
        SetTypeName(typeName);
        AddInput("A", type);
        AddInput("B", type);
        AddOutput("Result", type);
    }

    void CalculateOutputs() override {
        T valA = GetInputValue<T>("A").value_or(T{});
        T valB = GetInputValue<T>("B").value_or(T{});
        T result = valA - valB;
        m_Outputs[0]->SetValue(result);
    }
};

/**
 * @brief Base template for division operations (A / B).
 * Works with float, int, vec2, vec3, vec4.
 * Includes a check to prevent division by zero for scalar types.
 */
template<typename T>
class NodeDivideBase : public LGDataNode {
public:
    NodeDivideBase(DataType type, const std::string& nodeName, const std::string& typeName) {
        SetName(nodeName);
        SetTypeName(typeName);
        AddInput("A", type);
        AddInput("B", type);
        AddOutput("Result", type);
    }

    void CalculateOutputs() override {
        T valA = GetInputValue<T>("A").value_or(T{});
        T valB = GetInputValue<T>("B").value_or(T{});

        // Prevent division by zero for scalar types
        if constexpr (std::is_scalar_v<T>) {
            if (valB == T(0)) {
                // Return 0 or some other default to avoid crash/NaN
                m_Outputs[0]->SetValue(T{});
                return;
            }
        }

        T result = valA / valB;
        m_Outputs[0]->SetValue(result);
    }
};


// --- Concrete Math Node Implementations ---

// Subtraction
class NodeSubtractFloat : public NodeSubtractBase<float> {
public: NodeSubtractFloat() : NodeSubtractBase<float>(DataType::Float, "Subtract (Float)", "SubtractFloatNode") {}
      std::string GetCategory() const override { return "Math|Float"; }
};
class NodeSubtractVec3 : public NodeSubtractBase<glm::vec3> {
public: NodeSubtractVec3() : NodeSubtractBase<glm::vec3>(DataType::Vec3, "Subtract (Vec3)", "SubtractVec3Node") {}
      std::string GetCategory() const override { return "Math|Vector3"; }
};

// Division
class NodeDivideFloat : public NodeDivideBase<float> {
public: NodeDivideFloat() : NodeDivideBase<float>(DataType::Float, "Divide (Float)", "DivideFloatNode") {}
      std::string GetCategory() const override { return "Math|Float"; }
};
class NodeDivideVec3 : public NodeDivideBase<glm::vec3> {
public: NodeDivideVec3() : NodeDivideBase<glm::vec3>(DataType::Vec3, "Divide (Vec3)", "DivideVec3Node") {}
      std::string GetCategory() const override { return "Math|Vector3"; }
};

// Add/Mult nodes
class NodeAddVec3 : public NodeAddBase<glm::vec3> {
public: NodeAddVec3() : NodeAddBase<glm::vec3>(DataType::Vec3, "Add (Vec3)", "AddVec3Node") {}
      std::string GetCategory() const override { return "Math|Vector3"; }
};
class NodeMultFloat : public NodeMultBase<float> {
public: NodeMultFloat() : NodeMultBase<float>(DataType::Float, "Multiply (Float)", "MultiplyFloatNode") {}
      std::string GetCategory() const override { return "Math|Float"; }
};
class NodeMultVec3 : public NodeMultBase<glm::vec3> {
public: NodeMultVec3() : NodeMultBase<glm::vec3>(DataType::Vec3, "Multiply (Vec3)", "MultiplyVec3Node") {}
      std::string GetCategory() const override { return "Math|Vector3"; }
};


// --- Vector Math Nodes ---

class NodeDotProduct : public LGDataNode {
public:
    NodeDotProduct() {
        SetName("Dot Product (Vec3)");
        SetTypeName("DotProductVec3Node");
        AddInput("A", DataType::Vec3);
        AddInput("B", DataType::Vec3);
        AddOutput("Result", DataType::Float);
    }
    std::string GetCategory() const override { return "Math|Vector3"; }
    void CalculateOutputs() override {
        auto vecA = GetInputValue<glm::vec3>("A").value_or(glm::vec3(0.0f));
        auto vecB = GetInputValue<glm::vec3>("B").value_or(glm::vec3(0.0f));
        float result = glm::dot(vecA, vecB);
        m_Outputs[0]->SetValue(result);
    }
};

class NodeCrossProduct : public LGDataNode {
public:
    NodeCrossProduct() {
        SetName("Cross Product (Vec3)");
        SetTypeName("CrossProductVec3Node");
        AddInput("A", DataType::Vec3);
        AddInput("B", DataType::Vec3);
        AddOutput("Result", DataType::Vec3);
    }
    std::string GetCategory() const override { return "Math|Vector3"; }
    void CalculateOutputs() override {
        auto vecA = GetInputValue<glm::vec3>("A").value_or(glm::vec3(0.0f));
        auto vecB = GetInputValue<glm::vec3>("B").value_or(glm::vec3(0.0f));
        glm::vec3 result = glm::cross(vecA, vecB);
        m_Outputs[0]->SetValue(result);
    }
};

class NodeNormalizeVec3 : public LGDataNode {
public:
    NodeNormalizeVec3() {
        SetName("Normalize (Vec3)");
        SetTypeName("NormalizeVec3Node");
        AddInput("Input", DataType::Vec3);
        AddOutput("Result", DataType::Vec3);
    }
    std::string GetCategory() const override { return "Math|Vector3"; }
    void CalculateOutputs() override {
        auto vec = GetInputValue<glm::vec3>("Input").value_or(glm::vec3(0.0f));
        if (glm::length2(vec) > 0.0001f) {
            m_Outputs[0]->SetValue(glm::normalize(vec));
        }
        else {
            m_Outputs[0]->SetValue(glm::vec3(0.0f));
        }
    }
};

class NodeVectorLength : public LGDataNode {
public:
    NodeVectorLength() {
        SetName("Length (Vec3)");
        SetTypeName("VectorLengthNode");
        AddInput("Input", DataType::Vec3);
        AddOutput("Length", DataType::Float);
    }
    std::string GetCategory() const override { return "Math|Vector3"; }
    void CalculateOutputs() override {
        auto vec = GetInputValue<glm::vec3>("Input").value_or(glm::vec3(0.0f));
        m_Outputs[0]->SetValue(glm::length(vec));
    }
};

class NodeComposeVec3 : public LGDataNode {
public:
    NodeComposeVec3() {
        SetName("Compose Vec3");
        SetTypeName("ComposeVec3Node");
        AddInput("X", DataType::Float);
        AddInput("Y", DataType::Float);
        AddInput("Z", DataType::Float);
        AddOutput("Result", DataType::Vec3);
    }
    std::string GetCategory() const override { return "Math|Vector3"; }
    void CalculateOutputs() override {
        float x = GetInputValue<float>("X").value_or(0.0f);
        float y = GetInputValue<float>("Y").value_or(0.0f);
        float z = GetInputValue<float>("Z").value_or(0.0f);
        m_Outputs[0]->SetValue(glm::vec3(x, y, z));
    }
};

class NodeDecomposeVec3 : public LGDataNode {
public:
    NodeDecomposeVec3() {
        SetName("Decompose Vec3");
        SetTypeName("DecomposeVec3Node");
        AddInput("Input", DataType::Vec3);
        AddOutput("X", DataType::Float);
        AddOutput("Y", DataType::Float);
        AddOutput("Z", DataType::Float);
    }
    std::string GetCategory() const override { return "Math|Vector3"; }
    void CalculateOutputs() override {
        auto vec = GetInputValue<glm::vec3>("Input").value_or(glm::vec3(0.0f));
        m_Outputs[0]->SetValue(vec.x);
        m_Outputs[1]->SetValue(vec.y);
        m_Outputs[2]->SetValue(vec.z);
    }
};


// --- GraphNode Interaction Nodes ---

class LNodeGetPosition : public LGDataNode {
public:
    LNodeGetPosition() {
        SetName("Get Position");
        SetTypeName("GetPositionNode");
        AddInput("Node", DataType::GraphNodeRef);
        AddOutput("Position", DataType::Vec3);
    }
    std::string GetCategory() const override { return "Node"; }
    void CalculateOutputs() override {
        auto nodeOpt = GetInputValue<GraphNode*>("Node");
        if (nodeOpt && *nodeOpt) {
            m_Outputs[0]->SetValue((*nodeOpt)->GetWorldPosition());
        }
        else {
            m_Outputs[0]->SetValue(glm::vec3(0.0f));
        }
    }
};

class LNodeGetRotation : public LGDataNode {
public:
    LNodeGetRotation() {
        SetName("Get Rotation");
        SetTypeName("GetRotationNode");
        AddInput("Node", DataType::GraphNodeRef);
        AddOutput("Rotation", DataType::Mat4);
    }
    std::string GetCategory() const override { return "Node"; }
    void CalculateOutputs() override {
        auto nodeOpt = GetInputValue<GraphNode*>("Node");
        if (nodeOpt && *nodeOpt) {
            m_Outputs[0]->SetValue((*nodeOpt)->GetRotation());
        }
        else {
            m_Outputs[0]->SetValue(glm::mat4(1.0f));
        }
    }
};

class NodeRotateNode : public LGLogicNode {
public:
    NodeRotateNode() {
        SetName("Rotate Node");
        SetTypeName("RotateNode");
        AddInput("Node", DataType::GraphNodeRef);
        AddInput("Delta (Euler)", DataType::Vec3);
        AddInput("Local Space", DataType::Bool);
    }
    std::string GetCategory() const override { return "Scene|Transform"; }
    void Exec() override {
        auto nodeOpt = GetInputValue<GraphNode*>("Node");
        auto deltaOpt = GetInputValue<glm::vec3>("Delta (Euler)");
        bool local = GetInputValue<bool>("Local Space").value_or(true);

        if (nodeOpt && *nodeOpt && deltaOpt) {
            (*nodeOpt)->Turn(*deltaOpt, local);
        }
        ExecNext();
    }
};

class NodeMoveNode : public LGLogicNode {
public:
    NodeMoveNode() {
        SetName("Move Node");
        SetTypeName("MoveNode");
        AddInput("Node", DataType::GraphNodeRef);
        AddInput("Delta", DataType::Vec3);
    }
    std::string GetCategory() const override { return "Scene|Transform"; }
    void Exec() override {
        auto nodeOpt = GetInputValue<GraphNode*>("Node");
        auto deltaOpt = GetInputValue<glm::vec3>("Delta");

        if (nodeOpt && *nodeOpt && deltaOpt) {
            (*nodeOpt)->Move(*deltaOpt);
        }
        ExecNext();
    }
};

class NodeFindNodeByName : public LGDataNode {
public:
    NodeFindNodeByName() {
        SetName("Find Node By Name");
        SetTypeName("FindNodeByNameNode");
        AddInput("Name", DataType::String);
        AddOutput("Node", DataType::GraphNodeRef);
        AddOutput("Found", DataType::Bool);
    }
    std::string GetCategory() const override { return "Scene|Query"; }
    void CalculateOutputs() override {
        auto nameOpt = GetInputValue<std::string>("Name");
        if (nameOpt && SceneGraph::m_CurrentGraph) {
            GraphNode* foundNode = SceneGraph::m_CurrentGraph->FindNode(*nameOpt);
            if (foundNode) {
                m_Outputs[0]->SetValue(foundNode);
                m_Outputs[1]->SetValue(true);
            }
            else {
                m_Outputs[0]->SetValue<GraphNode*>(nullptr);
                m_Outputs[1]->SetValue(false);
            }
        }
        else {
            m_Outputs[0]->SetValue<GraphNode*>(nullptr);
            m_Outputs[1]->SetValue(false);
        }
    }
};


// --- Input / Constant Nodes ---

class NodeConstantFloat : public LGDataNode {
public:
    NodeConstantFloat() {
        SetName("Constant (Float)");
        SetTypeName("ConstantFloatNode");
        AddOutput("Value", DataType::Float);
        m_Outputs[0]->SetValue(1.0f);
    }
    std::string GetCategory() const override { return "Input|Constants"; }
    void CalculateOutputs() override { /* Value is already set */ }
};

class NodeConstantVec3 : public LGDataNode {
public:
    NodeConstantVec3() {
        SetName("Constant (Vec3)");
        SetTypeName("ConstantVec3Node");
        AddOutput("Value", DataType::Vec3);
        m_Outputs[0]->SetValue(glm::vec3(1.0f, 0.0f, 0.0f));
    }
    std::string GetCategory() const override { return "Input|Constants"; }
    void CalculateOutputs() override { /* Value is already set */ }
};

class NodeConstantString : public LGDataNode {
public:
    NodeConstantString() {
        SetName("Constant (String)");
        SetTypeName("ConstantStringNode");
        AddOutput("Value", DataType::String);
        m_Outputs[0]->SetValue(std::string("Default"));
    }
    std::string GetCategory() const override { return "Input|Constants"; }
    void CalculateOutputs() override { /* Value is already set */ }
};


// --- Flow Control Nodes ---

class NodeBranch : public LGLogicNode {
public:
    NodeBranch() {
        SetName("Branch");
        SetTypeName("BranchNode");
        AddInput("Condition", DataType::Bool);
        AddExecOutput("True");
        AddExecOutput("False");
    }
    std::string GetCategory() const override { return "Flow Control"; }
    void Exec() override {
        bool condition = GetInputValue<bool>("Condition").value_or(false);
        if (condition) {
            ExecNext(0);
        }
        else {
            ExecNext(1);
        }
    }
};

class NodePrintString : public LGLogicNode {
public:
    NodePrintString() {
        SetName("Print String");
        SetTypeName("PrintStringNode");
        AddInput("In", DataType::String);
    }
    std::string GetCategory() const override { return "Debug"; }
    void Exec() override {
        auto str = GetInputValue<std::string>("In").value_or("[Not Connected]");
        std::cout << "[GRAPH LOG] " << str << std::endl;
        ExecNext();
    }
};


// --- Conversion Nodes ---

class NodeVec3ToString : public LGDataNode {
public:
    NodeVec3ToString() {
        SetName("Vec3 To String");
        SetTypeName("Vec3ToStringNode");
        AddInput("In", DataType::Vec3);
        AddOutput("Out", DataType::String);
    }
    std::string GetCategory() const override { return "Conversion"; }
    void CalculateOutputs() override {
        auto vec = GetInputValue<glm::vec3>("In").value_or(glm::vec3(0.0f));
        std::stringstream ss;
        ss << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
        m_Outputs[0]->SetValue(ss.str());
    }
};

class NodeFloatToString : public LGDataNode {
public:
    NodeFloatToString() {
        SetName("Float To String");
        SetTypeName("FloatToStringNode");
        AddInput("In", DataType::Float);
        AddOutput("Out", DataType::String);
    }
    std::string GetCategory() const override { return "Conversion"; }
    void CalculateOutputs() override {
        float val = GetInputValue<float>("In").value_or(0.0f);
        m_Outputs[0]->SetValue(std::to_string(val));
    }
};
