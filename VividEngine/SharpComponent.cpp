#include "SharpComponent.h"
#include "GraphNode.h"
#include "SceneGraph.h"
#include "Vivid.h"

extern "C" __declspec(dllexport) void NodeTurn(void* node,float x, float y, float z)
{

	GraphNode* n = static_cast<GraphNode*>(node);
	n->Turn(glm::vec3(x, y, z), false);

//	std::cout << "ITS WORKING!!!" << std::endl;

}

extern "C" __declspec(dllexport) void NodeSetPosition(void* node,Vec3 pos)
{
	GraphNode* n = static_cast<GraphNode*>(node);

	n->SetPosition(glm::vec3(pos.x, pos.y, pos.z));




}


extern "C" __declspec(dllexport) Vec3 NodeGetPosition(void* node)
{
	GraphNode* n = static_cast<GraphNode*>(node);
	auto pos = n->GetPosition();
	Vec3 r;
	r.x = pos.x;
	r.y = pos.y;
	r.z = pos.z;
	return r;

}

extern "C" __declspec(dllexport) void NodeGetRotation(void* node, float* out) {

	//glm::mat4 mat = static_cast<GraphNode*>(node)->GetTransformMatrix(); // or however you get it
	GraphNode* n = static_cast<GraphNode*>(node);

	// Flatten into float[16] column-major
	const float* ptr = glm::value_ptr(n->GetRotation());
	for (int i = 0; i < 16; ++i)
		out[i] = ptr[i];

	
}

extern "C" __declspec(dllexport) void NodeSetRotation(void* node, float* out) {

	//glm::mat4 mat = static_cast<GraphNode*>(node)->GetTransformMatrix(); // or however you get it
	GraphNode* n = static_cast<GraphNode*>(node);
	glm::mat4 mat = glm::make_mat4(out);
	n->SetRotation(mat);

	// Flatten into float[16] column-major
//	const float* ptr = glm::value_ptr(n->GetRotation());
//	for (int i = 0; i < 16; ++i)
//		out[i] = ptr[i];



}

extern "C" __declspec(dllexport) void NodeSetScale(void* node, Vec3 scale)
{
	GraphNode* n = static_cast<GraphNode*>(node);
	n->SetScale(glm::vec3(scale.x, scale.y, scale.z));
}

extern "C" __declspec(dllexport) Vec3 NodeGetScale(void* node) {
	GraphNode* n = static_cast<GraphNode*>(node);
	return Vec3{ n->GetScale().x, n->GetScale().y, n->GetScale().z };
}

extern "C" __declspec(dllexport) void* SceneGetRoot() {

	GraphNode* node = SceneGraph::m_Instance->GetRootNode();
	return static_cast<void*>(node);


}

extern "C" __declspec(dllexport) int NodeNodeCount(void* node) {

	GraphNode* n = static_cast<GraphNode*>(node);
	return n->GetNodes().size();

}
extern "C" __declspec(dllexport) void* NodeGetNode(void* node, int index) {

	GraphNode* n = static_cast<GraphNode*>(node);
	return static_cast<void*>(n->GetNodes()[index]);

}
extern "C" __declspec(dllexport) void* NodeGetName(void* node) {
	GraphNode* n = static_cast<GraphNode*>(node);
	const char* msg= n->GetName().c_str();
	char* result = new char[strlen(msg) + 1];
	strcpy(result, msg);
	return result;
}
extern "C" __declspec(dllexport) void ConsoleLog(const char* str)
{
	Vivid::DebugLog(str);

}




void SharpComponent::SetClass(MClass* cls,MAsm* as,MAsm* vivid) {

	m_StaticClass = cls;
	m_Assembly = as;
	m_Vivid = vivid;

	auto gnode = m_Vivid->GetClass("Vivid.Scene", "GraphNode");
	auto gnode_inst = gnode->CreateInstance();


	m_GraphClass = gnode_inst;

	m_Instance = m_StaticClass->CreateInstance();
	m_Instance->SetFieldClass("Node", gnode_inst);

	gnode_inst->SetNativePtr("NodePtr", (void*)m_Owner);

}
void SharpComponent::SetScript(std::string dll,std::string name) {
	// Initialization code here
	
	m_Vivid = new MAsm("cs/VividEngine.dll", "C:\\Vivid3D\\Vivid3D\\CS\\");

	m_Assembly = new MAsm("cs/"+dll,"C:\\Vivid3D\\Vivid3D\\CS\\");
	m_StaticClass = m_Assembly->GetClass("Vivid",name);


	auto gnode = m_Vivid->GetClass("Vivid.Scene", "GraphNode");
	auto gnode_inst = gnode->CreateInstance();

	m_GraphClass = gnode_inst;

	m_Instance = m_StaticClass->CreateInstance();
	m_Instance->SetFieldClass("Node", gnode_inst);

	gnode_inst->SetNativePtr("NodePtr", (void*)m_Owner);

	int b = 5;
}

void SharpComponent::OnUpdate(float deltaTime) {
	// Update logic here
	
	std::cout << "OnsharpUpdate" << std::endl;
	m_Instance->CallFunction("OnUpdate", deltaTime);

	//if (m_Instance) {
	//	m_Instance->CallMethod("Update", deltaTime);
//	}
}

MClass* SharpComponent::CreateGraphNode() {

	auto gnode = m_Vivid->GetClass("Vivid.Scene", "GraphNode");
	auto gnode_inst = gnode->CreateInstance();

	return gnode_inst;

}