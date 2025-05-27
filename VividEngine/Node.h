#pragma once
#include <vector>
#include <string>
#include "BasicMath.hpp"
#include "Bounds.h"
using namespace Diligent;

class Bounds;
class VClass;
class VFile;
class MaterialBase;



class Node
{
public:

	Node();

	//transform

	//hierarchy
	void AddNode(Node* node);
	int NodesCount();
	Node* GetNode(int index);
	std::vector<Node*> GetNodes();

	//Transform
	void SetPosition(float3 position);
	void SetRotation(float pitch, float yaw, float roll, bool edit = false);
	void SetScale(float3 scale);
	float3 GetRotationEU();
	float3 GetPosition();
	void Move(float3 delta);
	void Translate(float3 delta);
	void Rotate(float pitch, float yaw, float roll);
	void Turn(float pitch, float yaw, float roll, bool local);
	float4x4 GetRotation() {
		return m_Rotation;
	}
	void SetRotation(float4x4 rotation) {
		m_Rotation = rotation;
		BuildGeo();

	}
	float3 TransformVector(float3 vector);
	virtual float4x4 GetWorldMatrix();
	virtual void Render(bool sp) {
		RenderChildren(sp);
	};
	virtual void RenderForcedMaterial(MaterialBase* material);
	virtual void RenderDepth() {
		RenderDepthChildren();
	}
	void RenderDepthChildren() {
		for (auto node : m_Nodes) {
			node->RenderDepth();
		}
	}
	void RenderChildren(bool sp) {
		for (auto node : m_Nodes) {
			node->Render(sp);
		}
	}
	void RenderChildrenForcedMaterial(MaterialBase* material);
	std::string GetName() {

		return m_Name;
	}
	void SetName(std::string name) {
		m_Name = name;
	}
	float3 GetScale() {
		return m_Scale;
	}
	void SetEnable(bool enable) {
		m_Enabled = enable;
	}
	bool GetEnabled() {
		return m_Enabled;
	}
	std::string GetFullName();
	void SetRoot(Node* node);
	virtual void Update(float delta);
	void AddScript(std::string path);
	std::vector<VClass*> GetScripts();
	void BeginPlay();
	void Stop();
	virtual void Push();
	virtual void Pop();
	virtual void BuildGeo() {};
	void SetStatic(bool stat);
	bool GetStatic();
	virtual Bounds* GetBounds() { return new Bounds(); };
	void SetResourcePath(std::string path);
	std::string GetResourcePath();
	virtual void WriteNode(VFile* file);
	virtual void ReadNode(VFile* file);
	void SetRTEnable() {
		m_RTEnable = true;
	}
	bool GetRTEnable() {
		return true;
	}
	bool IsUpdated() {
		return m_Updated;
	}
	void Updated() {
		m_Updated = true;
		for (auto n : m_Nodes) {
			n->Updated();
		}
	}
	void UpdateComplete() {
		m_Updated = false;
	}
	void UpdateScripts(float delta);
	void WriteScripts(VFile* file);
	void ReadScripts(VFile* file);
	VClass* FindClass(std::string name, int index = 0);
	VClass* ReadClass(VFile* file);
	void LookAt(float3 target);
	void Remove();
	void RemoveNode(Node* node);
	Node* GetRoot() {
		return m_Root;
	}
	float4x4 GetWorldRotation() {

		float4x4 root = float4x4::Identity();
		if (m_Root != nullptr) {
			root = m_Root->GetWorldRotation();
		}

		return m_Rotation * root;

	
	}
	bool IsChildOf(Node* node) {

		if (node == this) {
			return true;
		}
		for (auto s : m_Nodes) {
			bool r = s->IsChildOf(node);
			if (r) return true;
		}
		return false;
	}
protected:

	std::string m_Name;

	//transform
	float4x4 m_Rotation;
	float3 m_Position;
	float3 m_Scale;
	float3 m_ERotation;
	//hierarchy. 
	Node* m_Root;
	std::vector<Node*> m_Nodes;
	bool m_Enabled = true;
	VClass* m_NodeClass;
	std::vector<VClass*> m_Scripts;
	bool m_IsPlaying = false;

	//
	float4x4 m_PushedRotation;
	float3 m_PushedPosition;
	float3 m_PushedScale;
	bool m_Static = true;
	std::string m_ResourcePath = "";
	bool m_RTEnable = false;
	bool m_Updated = true;
};



