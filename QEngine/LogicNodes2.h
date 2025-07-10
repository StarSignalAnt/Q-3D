#pragma once

#include "LGDataNode.h"
#include "LGLogicNode.h"
#include "GraphNode.h"
#include "SceneGraph.h"
#include "Component.h"
#include "LightComponent.h"
#include "CameraComponent.h"
#include <glm/glm.hpp>
#include <random>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm> 
// --- GraphNode Transform & Hierarchy Nodes (Data) ---

/**
 * @brief Gets the local scale of a GraphNode.
 */
class LNodeGetScale : public LGDataNode {
public:
    LNodeGetScale() {
        SetName("Get Scale");
        SetTypeName("GetScaleNode");
        AddInput("Node", DataType::GraphNodeRef);
        AddOutput("Scale", DataType::Vec3);
    }
    std::string GetCategory() const override { return "Node|Transform"; }
    void CalculateOutputs() override {
        auto nodeOpt = GetInputValue<GraphNode*>("Node");
        if (nodeOpt && *nodeOpt) {
            m_Outputs[0]->SetValue((*nodeOpt)->GetScale());
        }
        else {
            m_Outputs[0]->SetValue(glm::vec3(1.0f));
        }
    }
};

/**
 * @brief Gets the local rotation of a GraphNode as Euler angles (in degrees).
 */
class NodeGetEulerRotation : public LGDataNode {
public:
    NodeGetEulerRotation() {
        SetName("Get Euler Rotation");
        SetTypeName("GetEulerRotationNode");
        AddInput("Node", DataType::GraphNodeRef);
        AddOutput("Euler Angles", DataType::Vec3);
    }
    std::string GetCategory() const override { return "Node|Transform"; }
    void CalculateOutputs() override {
        auto nodeOpt = GetInputValue<GraphNode*>("Node");
        if (nodeOpt && *nodeOpt) {
            m_Outputs[0]->SetValue((*nodeOpt)->GetEularRotation());
        }
        else {
            m_Outputs[0]->SetValue(glm::vec3(0.0f));
        }
    }
};

/**
 * @brief Gets the parent of a GraphNode.
 */
class NodeGetParent : public LGDataNode {
public:
    NodeGetParent() {
        SetName("Get Parent");
        SetTypeName("GetParentNode");
        AddInput("Node", DataType::GraphNodeRef);
        AddOutput("Parent", DataType::GraphNodeRef);
    }
    std::string GetCategory() const override { return "Node|Hierarchy"; }
    void CalculateOutputs() override {
        auto nodeOpt = GetInputValue<GraphNode*>("Node");
        if (nodeOpt && *nodeOpt) {
            m_Outputs[0]->SetValue((*nodeOpt)->GetRootNode());
        }
        else {
            m_Outputs[0]->SetValue<GraphNode*>(nullptr);
        }
    }
};

// --- GraphNode Transform & Hierarchy Nodes (Logic) ---

/**
 * @brief Sets the local scale of a GraphNode.
 */
class LNodeSetScale : public LGLogicNode {
public:
    LNodeSetScale() {
        SetName("Set Scale");
        SetTypeName("SetScaleNode");
        AddInput("Node", DataType::GraphNodeRef);
        AddInput("Scale", DataType::Vec3);
    }
    std::string GetCategory() const override { return "Node|Transform"; }
    void Exec() override {
        auto nodeOpt = GetInputValue<GraphNode*>("Node");
        auto scaleOpt = GetInputValue<glm::vec3>("Scale");
        if (nodeOpt && *nodeOpt && scaleOpt) {
            (*nodeOpt)->SetScale(*scaleOpt);
        }
        ExecNext();
    }
};

/**
 * @brief Sets the local rotation of a GraphNode using Euler angles (in degrees).
 */
class NodeSetEulerRotation : public LGLogicNode {
public:
    NodeSetEulerRotation() {
        SetName("Set Euler Rotation");
        SetTypeName("SetEulerRotationNode");
        AddInput("Node", DataType::GraphNodeRef);
        AddInput("Euler Angles", DataType::Vec3);
    }
    std::string GetCategory() const override { return "Node|Transform"; }
    void Exec() override {
        auto nodeOpt = GetInputValue<GraphNode*>("Node");
        auto eulerOpt = GetInputValue<glm::vec3>("Euler Angles");
        if (nodeOpt && *nodeOpt && eulerOpt) {
            (*nodeOpt)->SetRotation(*eulerOpt);
        }
        ExecNext();
    }
};

/**
 * @brief Makes a GraphNode orient itself to look at a target world position.
 */
class NodeLookAt : public LGLogicNode {
public:
    NodeLookAt() {
        SetName("Look At");
        SetTypeName("LookAtNode");
        AddInput("Node", DataType::GraphNodeRef);
        AddInput("Target", DataType::Vec3);
    }
    std::string GetCategory() const override { return "Node|Transform"; }
    void Exec() override {
        auto nodeOpt = GetInputValue<GraphNode*>("Node");
        auto targetOpt = GetInputValue<glm::vec3>("Target");
        if (nodeOpt && *nodeOpt && targetOpt) {
            // The LookAt function in GraphNode needs the direction FROM the node to the target.
            glm::vec3 direction = *targetOpt - (*nodeOpt)->GetWorldPosition();
            (*nodeOpt)->LookAt(direction);
        }
        ExecNext();
    }
};

/**
 * @brief Attaches one GraphNode to another, making it a child.
 */
class NodeAttachTo : public LGLogicNode {
public:
    NodeAttachTo() {
        SetName("Attach To");
        SetTypeName("AttachToNode");
        AddInput("Child", DataType::GraphNodeRef);
        AddInput("Parent", DataType::GraphNodeRef);
    }
    std::string GetCategory() const override { return "Node|Hierarchy"; }
    void Exec() override {
        auto childOpt = GetInputValue<GraphNode*>("Child");
        auto parentOpt = GetInputValue<GraphNode*>("Parent");

        if (childOpt && *childOpt && parentOpt && *parentOpt) {
            GraphNode* child = *childOpt;
            GraphNode* parent = *parentOpt;

            // First, detach from any existing parent to avoid being in two places
            if (child->GetRootNode()) {
                child->GetRootNode()->RemoveNode(child);
            }
            parent->AddNode(child);
        }
        ExecNext();
    }
};

/**
 * @brief Detaches a GraphNode from its parent, making it a root-level node.
 */
class NodeDetach : public LGLogicNode {
public:
    NodeDetach() {
        SetName("Detach");
        SetTypeName("DetachNode");
        AddInput("Node", DataType::GraphNodeRef);
    }
    std::string GetCategory() const override { return "Node|Hierarchy"; }
    void Exec() override {
        auto nodeOpt = GetInputValue<GraphNode*>("Node");
        if (nodeOpt && *nodeOpt) {
            GraphNode* node = *nodeOpt;
            if (node->GetRootNode() && SceneGraph::m_Instance) {
                // Remove from old parent and add to the scene's root
                node->GetRootNode()->RemoveNode(node);
                SceneGraph::m_Instance->AddNode(node);
            }
        }
        ExecNext();
    }
};

// --- SceneGraph Interaction Nodes ---

/**
 * @brief Gets the main camera from the active SceneGraph.
 */
class NodeGetMainCamera : public LGDataNode {
public:
    NodeGetMainCamera() {
        SetName("Get Main Camera");
        SetTypeName("GetMainCameraNode");
        AddOutput("Camera Node", DataType::GraphNodeRef);
    }
    std::string GetCategory() const override { return "Scene|Query"; }
    void CalculateOutputs() override {
        if (SceneGraph::m_Instance) {
            m_Outputs[0]->SetValue(SceneGraph::m_Instance->GetCamera());
        }
        else {
            m_Outputs[0]->SetValue<GraphNode*>(nullptr);
        }
    }
};

/**
 * @brief Performs a raycast into the scene from the mouse position.
 */
class NodeMousePick : public LGDataNode {
public:
    NodeMousePick() {
        SetName("Mouse Pick");
        SetTypeName("MousePickNode");
        AddInput("Screen X", DataType::Int);
        AddInput("Screen Y", DataType::Int);
        AddOutput("Hit", DataType::Bool);
        AddOutput("Point", DataType::Vec3);
        AddOutput("Distance", DataType::Float);
        AddOutput("Node", DataType::GraphNodeRef);
    }
    std::string GetCategory() const override { return "Scene|Input"; }
    void CalculateOutputs() override {
        auto x = GetInputValue<int>("Screen X");
        auto y = GetInputValue<int>("Screen Y");

        if (x && y && SceneGraph::m_Instance) {
            HitResult r = SceneGraph::m_Instance->MousePick(*x, *y);
            m_Outputs[0]->SetValue(r.m_Hit);
            m_Outputs[1]->SetValue(r.m_Point);
            m_Outputs[2]->SetValue(r.m_Distance);
            m_Outputs[3]->SetValue(r.m_Node);
        }
        else {
            m_Outputs[0]->SetValue(false);
            m_Outputs[1]->SetValue(glm::vec3(0, 0, 0));
            m_Outputs[2]->SetValue(0.0f);
            m_Outputs[3]->SetValue<GraphNode*>(nullptr);
        }
    }
};

// --- Component Interaction Nodes ---

/**
 * @brief Gets the properties of a LightComponent from a GraphNode.
 */
class NodeGetLightProperties : public LGDataNode {
public:
    NodeGetLightProperties() {
        SetName("Get Light Properties");
        SetTypeName("GetLightPropertiesNode");
        AddInput("Light Node", DataType::GraphNodeRef);
        AddOutput("Color", DataType::Vec3);
        AddOutput("Intensity", DataType::Float);
        AddOutput("Range", DataType::Float);
    }
    std::string GetCategory() const override { return "Component|Light"; }
    void CalculateOutputs() override {
        auto nodeOpt = GetInputValue<GraphNode*>("Light Node");
        if (nodeOpt && *nodeOpt) {
            if (auto lightComp = (*nodeOpt)->GetComponent<LightComponent>()) {
                m_Outputs[0]->SetValue(lightComp->GetColor());
                m_Outputs[1]->SetValue(lightComp->GetIntensity());
                m_Outputs[2]->SetValue(lightComp->GetRange());
                return;
            }
        }
        // Default values if no node or component
        m_Outputs[0]->SetValue(glm::vec3(0.0f));
        m_Outputs[1]->SetValue(0.0f);
        m_Outputs[2]->SetValue(0.0f);
    }
};

/**
 * @brief Sets the properties of a LightComponent on a GraphNode.
 */
class NodeSetLightProperties : public LGLogicNode {
public:
    NodeSetLightProperties() {
        SetName("Set Light Properties");
        SetTypeName("SetLightPropertiesNode");
        AddInput("Light Node", DataType::GraphNodeRef);
        AddInput("Color", DataType::Vec3);
        AddInput("Intensity", DataType::Float);
        AddInput("Range", DataType::Float);
    }
    std::string GetCategory() const override { return "Component|Light"; }
    void Exec() override {
        auto nodeOpt = GetInputValue<GraphNode*>("Light Node");
        if (nodeOpt && *nodeOpt) {
            if (auto lightComp = (*nodeOpt)->GetComponent<LightComponent>()) {
                auto color = GetInputValue<glm::vec3>("Color");
                if (color) lightComp->SetColor(*color);

                auto intensity = GetInputValue<float>("Intensity");
                if (intensity) lightComp->SetIntensity(*intensity);

                auto range = GetInputValue<float>("Range");
                if (range) lightComp->SetRange(*range);
            }
        }
        ExecNext();
    }
};

// --- Physics Nodes ---

/**
 * @brief Sets the physics body type for a GraphNode. This will create the physics actor
 * when the scene is played.
 */
class NodeSetPhysicsBody : public LGLogicNode {
public:
    NodeSetPhysicsBody() {
        SetName("Set Physics Body");
        SetTypeName("SetPhysicsBodyNode");
        AddInput("Node", DataType::GraphNodeRef);
        // In a real editor, this would be an enum dropdown.
        // We'll use an integer for now: 0=Box, 1=Sphere, 2=Convex, 3=TriMesh
        AddInput("Body Type (Int)", DataType::Int);
    }
    std::string GetCategory() const override { return "Node|Physics"; }
    void Exec() override {
        auto nodeOpt = GetInputValue<GraphNode*>("Node");
        auto typeOpt = GetInputValue<int>("Body Type (Int)");

        if (nodeOpt && *nodeOpt && typeOpt) {
            BodyType bodyType = static_cast<BodyType>(*typeOpt);
            (*nodeOpt)->SetBody(bodyType);
        }

        ExecNext();
    }
};

/**
 * @brief Gets the bounding box of a GraphNode.
 */
class NodeGetBounds : public LGDataNode {
public:
    NodeGetBounds() {
        SetName("Get Bounds");
        SetTypeName("GetBoundsNode");
        AddInput("Node", DataType::GraphNodeRef);
        AddInput("Include Children", DataType::Bool);
        AddOutput("Center", DataType::Vec3);
        AddOutput("Size", DataType::Vec3);
        AddOutput("Min", DataType::Vec3);
        AddOutput("Max", DataType::Vec3);
    }
    std::string GetCategory() const override { return "Node|Utility"; }
    void CalculateOutputs() override {
        auto nodeOpt = GetInputValue<GraphNode*>("Node");
        bool includeChildren = GetInputValue<bool>("Include Children").value_or(true);

        if (nodeOpt && *nodeOpt) {
            Bounds bounds = (*nodeOpt)->GetStaticMeshBounds(includeChildren);
            if (bounds.IsValid()) {
                m_Outputs[0]->SetValue(bounds.center);
                m_Outputs[1]->SetValue(bounds.size);
                m_Outputs[2]->SetValue(bounds.min);
                m_Outputs[3]->SetValue(bounds.max);
                return;
            }
        }

        // Default values
        m_Outputs[0]->SetValue(glm::vec3(0.0f));
        m_Outputs[1]->SetValue(glm::vec3(0.0f));
        m_Outputs[2]->SetValue(glm::vec3(0.0f));
        m_Outputs[3]->SetValue(glm::vec3(0.0f));
    }
};

// --- Logic & Comparison Nodes ---

class NodeCompareFloat : public LGDataNode {
public:
    NodeCompareFloat() {
        SetName("Compare (Float)");
        SetTypeName("CompareFloatNode");
        AddInput("A", DataType::Float);
        AddInput("B", DataType::Float);
        AddOutput("A > B", DataType::Bool);
        AddOutput("A < B", DataType::Bool);
        AddOutput("A == B", DataType::Bool);
    }
    std::string GetCategory() const override { return "Logic|Comparison"; }

    void CalculateOutputs() override {
        float a = GetInputValue<float>("A").value_or(0.0f);
        float b = GetInputValue<float>("B").value_or(0.0f);
        m_Outputs[0]->SetValue(a > b);
        m_Outputs[1]->SetValue(a < b);
        m_Outputs[2]->SetValue(std::abs(a - b) < 0.0001f); // Epsilon for float comparison
    }
};

class NodeBooleanAND : public LGDataNode {
public:
    NodeBooleanAND() {
        SetName("AND (Boolean)");
        SetTypeName("BooleanAndNode");
        AddInput("A", DataType::Bool);
        AddInput("B", DataType::Bool);
        AddOutput("Result", DataType::Bool);
    }
    std::string GetCategory() const override { return "Logic|Boolean"; }

    void CalculateOutputs() override {
        bool a = GetInputValue<bool>("A").value_or(false);
        bool b = GetInputValue<bool>("B").value_or(false);
        m_Outputs[0]->SetValue(a && b);
    }
};

class NodeBooleanOR : public LGDataNode {
public:
    NodeBooleanOR() {
        SetName("OR (Boolean)");
        SetTypeName("BooleanOrNode");
        AddInput("A", DataType::Bool);
        AddInput("B", DataType::Bool);
        AddOutput("Result", DataType::Bool);
    }
    std::string GetCategory() const override { return "Logic|Boolean"; }

    void CalculateOutputs() override {
        bool a = GetInputValue<bool>("A").value_or(false);
        bool b = GetInputValue<bool>("B").value_or(false);
        m_Outputs[0]->SetValue(a || b);
    }
};

class NodeBooleanNOT : public LGDataNode {
public:
    NodeBooleanNOT() {
        SetName("NOT (Boolean)");
        SetTypeName("BooleanNotNode");
        AddInput("In", DataType::Bool);
        AddOutput("Result", DataType::Bool);
    }
    std::string GetCategory() const override { return "Logic|Boolean"; }

    void CalculateOutputs() override {
        bool in = GetInputValue<bool>("In").value_or(false);
        m_Outputs[0]->SetValue(!in);
    }
};
template <typename T>
constexpr const T& my_max(const T& a, const T& b) {
    return (a < b) ? b : a;
}

template <typename T>
constexpr const T& my_min(const T& a, const T& b) {
    return (a < b) ? a : b;
}
// --- Advanced Math Nodes ---

class NodeLerpFloat : public LGDataNode {
public:
    NodeLerpFloat() {
        SetName("Lerp (Float)");
        SetTypeName("LerpFloatNode");
        AddInput("A", DataType::Float);
        AddInput("B", DataType::Float);
        AddInput("Alpha", DataType::Float);
        AddOutput("Result", DataType::Float);
    }
    std::string GetCategory() const override { return "Math|Interpolation"; }

    void CalculateOutputs() override {
        float a = GetInputValue<float>("A").value_or(0.f);
        float b = GetInputValue<float>("B").value_or(0.f);
        float alpha = GetInputValue<float>("Alpha").value_or(0.f);
        alpha = my_max(0.0f, my_min(1.0f, alpha)); // Clamp alpha
        m_Outputs[0]->SetValue(a + alpha * (b - a));
    }
};



class NodeLerpVec3 : public LGDataNode {
public:
    NodeLerpVec3() {
        SetName("Lerp (Vec3)");
        SetTypeName("LerpVec3Node");
        AddInput("A", DataType::Vec3);
        AddInput("B", DataType::Vec3);
        AddInput("Alpha", DataType::Float);
        AddOutput("Result", DataType::Vec3);
    }
    std::string GetCategory() const override { return "Math|Interpolation"; }

    void CalculateOutputs() override {
        glm::vec3 a = GetInputValue<glm::vec3>("A").value_or(glm::vec3(0.f));
        glm::vec3 b = GetInputValue<glm::vec3>("B").value_or(glm::vec3(0.f));
        float alpha = GetInputValue<float>("Alpha").value_or(0.f);
        alpha = my_max(0.0f, my_min(1.0f, alpha)); // Clamp alpha
        m_Outputs[0]->SetValue(glm::mix(a, b, alpha));
    }
};

class NodeSine : public LGDataNode {
public:
    NodeSine() {
        SetName("Sine");
        SetTypeName("SineNode");
        AddInput("In", DataType::Float);
        AddOutput("Result", DataType::Float);
    }
    std::string GetCategory() const override { return "Math|Waves"; }

    void CalculateOutputs() override {
        float in = GetInputValue<float>("In").value_or(0.f);
        m_Outputs[0]->SetValue(sin(in));
    }
};

class NodeRandomFloatInRange : public LGDataNode {
public:
    NodeRandomFloatInRange() {
        SetName("Random Float In Range");
        SetTypeName("RandomFloatNode");
        AddInput("Min", DataType::Float);
        AddInput("Max", DataType::Float);
        AddOutput("Result", DataType::Float);
    }
    std::string GetCategory() const override { return "Math|Random"; }

    void CalculateOutputs() override {
        float min = GetInputValue<float>("Min").value_or(0.f);
        float max = GetInputValue<float>("Max").value_or(1.f);
        if (min > max) std::swap(min, max);

        static std::mt19937 generator(std::random_device{}());
        std::uniform_real_distribution<float> distribution(min, max);

        m_Outputs[0]->SetValue(distribution(generator));
    }
};

// --- Time Nodes ---

class NodeGetDeltaTime : public LGDataNode {
public:
    NodeGetDeltaTime() {
        SetName("Get Delta Time");
        SetTypeName("GetDeltaTimeNode");
        AddOutput("Seconds", DataType::Float);
    }
    std::string GetCategory() const override { return "Time"; }

    void CalculateOutputs() override {
        // m_Outputs[0]->SetValue(QEngine::GetDeltaTime());
    }
};

// --- String Manipulation ---

class NodeAppendString : public LGDataNode {
public:
    NodeAppendString() {
        SetName("Append String");
        SetTypeName("AppendStringNode");
        AddInput("A", DataType::String);
        AddInput("B", DataType::String);
        AddOutput("Result", DataType::String);
    }
    std::string GetCategory() const override { return "String"; }

    void CalculateOutputs() override {
        std::string a = GetInputValue<std::string>("A").value_or("");
        std::string b = GetInputValue<std::string>("B").value_or("");
        m_Outputs[0]->SetValue(a + b);
    }
};

// --- Flow Control Nodes ---

class NodeSequence : public LGLogicNode {
public:
    NodeSequence() {
        SetName("Sequence");
        SetTypeName("SequenceNode");
        // Add multiple execution outputs.
        AddExecOutput("Then 0");
        AddExecOutput("Then 1");
        AddExecOutput("Then 2");
    }
    std::string GetCategory() const override { return "Flow Control"; }

    void Exec() override {
        // Execute each output pin in order.
        ExecNext(0);
        ExecNext(1);
        ExecNext(2);
    }
};

class NodeForLoop : public LGLogicNode {
public:
    NodeForLoop() {
        SetName("For Loop");
        SetTypeName("ForLoopNode");
        AddInput("First Index", DataType::Int);
        AddInput("Last Index", DataType::Int);
        AddExecOutput("Loop Body");
        AddExecOutput("Completed");
        AddOutput("Index", DataType::Int);
    }
    std::string GetCategory() const override { return "Flow Control"; }

    void Exec() override {
        int first = GetInputValue<int>("First Index").value_or(0);
        int last = GetInputValue<int>("Last Index").value_or(0);

        for (int i = first; i <= last; ++i) {
            // Set the current loop index on our data output pin
            m_Outputs[0]->SetValue(i);
            // Fire the "Loop Body" execution pin
            ExecNext(0);
        }
        // After the loop finishes, fire the "Completed" pin
        ExecNext(1);
    }
};

class NodeDoOnce : public LGLogicNode {
public:
    NodeDoOnce() {
        SetName("Do Once");
        SetTypeName("DoOnceNode");
        AddInput("Reset", DataType::Bool); // Use a data input for resetting
        AddExecOutput("Completed");
    }
    std::string GetCategory() const override { return "Flow Control|Gates"; }

    void Exec() override {
        bool reset = GetInputValue<bool>("Reset").value_or(false);
        if (reset) {
            m_hasFired = false;
        }

        if (!m_hasFired) {
            m_hasFired = true;
            ExecNext(0);
        }
    }

private:
    bool m_hasFired = false;
};
