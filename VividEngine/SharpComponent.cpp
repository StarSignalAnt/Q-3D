#include "SharpComponent.h"
#include "GraphNode.h"
#include "SceneGraph.h"
#include "Vivid.h"
#include "Draw2D.h"
#include "GameVideo.h"
#include "Texture2D.h"
#include "Vivid.h"
#include "GameInput.h"

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

extern "C" __declspec(dllexport) void NodeGetWorldMatrix(void* node, float* out) {

	GraphNode* n = static_cast<GraphNode*>(node);

	// Flatten into float[16] column-major
	const float* ptr = glm::value_ptr(n->GetWorldMatrix());
	for (int i = 0; i < 16; ++i)
		out[i] = ptr[i];

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

extern "C" __declspec(dllexport) void* CreateVideo(const char* path) {

	GameVideo* r = new GameVideo(path);
	return r;

}

extern "C" __declspec(dllexport) void PlayVideo(void* video) {

	GameVideo* videoobj = static_cast<GameVideo*>(video);

	videoobj->Play();
}

extern "C" __declspec(dllexport) void UpdateVideo(void* video) {


	GameVideo* videoobj = static_cast<GameVideo*>(video);
	videoobj->Update();

}

extern "C" __declspec(dllexport) void* VideoGetFrame(void* video) {

	GameVideo* videoobj = static_cast<GameVideo*>(video);
	return (void*)videoobj->GetFrame();

}

extern "C" __declspec(dllexport) void RenderVideo(void* video, Vec2 pos, Vec2 size)
{



}

extern "C" __declspec(dllexport) void* CreateDraw2D() {

	auto draw = new Draw2D(SceneGraph::m_Instance->GetCamera());

	return (void*)draw;

}
extern "C" __declspec(dllexport) void DrawRect(void* draw,void*tex, Vec2 pos, Vec2 size)
{

	Draw2D* drawObj = static_cast<Draw2D*>(draw);

	drawObj->Rect(static_cast<Texture2D*>(tex), glm::vec2(pos.x, pos.y), glm::vec2(size.x, size.y), glm::vec4(1, 1, 1, 1));

	

}

extern "C" __declspec(dllexport) void* LoadTexture2D(const char* path) {

	return (void*)new Texture2D(path);

}

extern "C" __declspec(dllexport) int EngineGetWidth() {

	return Vivid::GetFrameWidth();

}
extern "C" __declspec(dllexport) int EngineGetHeight() {

	return Vivid::GetFrameHeight();

}


struct CastResult {

	CastResult() {};
	bool Hit;
	Vec3 HitPoint;
	float HitDistant;
	void* HitNode;

};

extern "C" __declspec(dllexport) void* SceneRayCast(Vec3 from, Vec3 to) {

	auto r = SceneGraph::m_Instance->RayCast(glm::vec3(from.x,from.y,from.z), glm::vec3(to.x,to.y,to.z));

	CastResult* res = new CastResult;
	res->Hit = r.m_Hit;
	if (r.m_Hit) {
		res->HitPoint = Vec3(r.m_Point.x, r.m_Point.y, r.m_Point.z);
		res->HitDistant = r.m_Distance;
		res->HitNode = r.m_Node;
	}
	else {
		res->HitPoint = Vec3(0, 0, 0);
		res->HitDistant = 0;
		res->HitNode = nullptr;
	}
	return res;





}

extern "C" __declspec(dllexport) bool CastResultHit(void* res) {

	CastResult* r = (CastResult*)res;
	return r->Hit;

}

extern "C" __declspec(dllexport) Vec3 CastResultPoint(void* res) {
	CastResult* r = (CastResult*)res;
	return r->HitPoint;

}

extern "C" __declspec(dllexport) void* CastResultNode(void* res) {

	CastResult* r = (CastResult*)res;
	return (void*)r->HitNode;

}


extern "C" __declspec(dllexport) void* SceneMousePick(Vec2 pos) {

	auto r = SceneGraph::m_Instance->MousePick((int)pos.x,(int)pos.y);

	CastResult* res = new CastResult;
	res->Hit = r.m_Hit;
	if (r.m_Hit) {
		res->HitPoint = Vec3(r.m_Point.x, r.m_Point.y, r.m_Point.z);
		res->HitDistant = r.m_Distance;
		res->HitNode = r.m_Node;
	}
	else {
		res->HitPoint = Vec3(0, 0, 0);
		res->HitDistant = 0;
		res->HitNode = nullptr;
	}
	return res;

}

extern "C" __declspec(dllexport) int GetMouseX() {
	return (int)GameInput::MousePosition.x;
}
extern "C" __declspec(dllexport) int GetMouseY()
{
	return (int)GameInput::MousePosition.y;
}


void SharpComponent::SetClass(MClass* cls,MAsm* as,MAsm* vivid) {


	m_StaticClass = cls; // This is a handle to the class type, not an instance.
	m_Assembly = as;
	m_Vivid = vivid;

	// Create a C# wrapper for our GraphNode.
	m_GraphClass = CreateGraphNode();
	if (!m_GraphClass) {
		std::cerr << "Error: Failed to create GraphNode wrapper in SetClass." << std::endl;
		return;
	}

	// Create an instance of the user's script (e.g., "PlayerScript").
	m_Instance = m_StaticClass->CreateInstance();
	if (!m_Instance) {
		std::cerr << "Error: Failed to create instance for script class." << std::endl;
		delete m_GraphClass;
		return;
	}

	// Set the "Node" property on the C# script instance to our GraphNode wrapper.
	m_Instance->SetFieldClass("Node", m_GraphClass);

	// Set the internal pointer of the C# GraphNode wrapper to this component's owner.
	// This creates the link from C# back to the C++ GraphNode.
	m_GraphClass->SetNativePtr("NodePtr", (void*)m_Owner);
	/*
	m_StaticClass = cls;
	m_Assembly = as;
	m_Vivid = vivid;

	auto gnode = m_Vivid->GetClass("Vivid.Scene", "GraphNode");
	auto gnode_inst = gnode->CreateInstance();


	m_GraphClass = gnode_inst;

	m_Instance = m_StaticClass->CreateInstance();
	m_Instance->SetFieldClass("Node", gnode_inst);

	gnode_inst->SetNativePtr("NodePtr", (void*)m_Owner);
	*/
}
void SharpComponent::SetScript(std::string dll,std::string name) {
	// Initialization code here
	MonoLib* monoLib = Vivid::GetMonoLib();
	if (!monoLib) {
		std::cerr << "Error: SetScript failed because MonoLib is not available." << std::endl;
		return;
	}

	// 2. Use MonoLib to get the C# class definition.
	MClass* scriptClass = monoLib->GetClass(name); // Assumes "Vivid" namespace
	if (!scriptClass) {
		std::cerr << "Error: Script class definition not found: Vivid." << name << std::endl;
		return;
	}

	// 3. Get the assembly wrappers from MonoLib.
	MAsm* gameAssembly = monoLib->GetAssembly();
	MAsm* vividAssembly = monoLib->GetVivid();

	// 4. Call the main setup function with the retrieved class/assemblies.
	SetClass(scriptClass, gameAssembly, vividAssembly);

	// 5. Clean up the temporary class definition wrapper.
	delete scriptClass;
	/*
	m_Vivid = new MAsm("cs/VividEngine.dll", "C:\\Vivid3D\\Vivid3D\\CS\\");

	m_Assembly = new MAsm("cs/"+dll,"C:\\Vivid3D\\Vivid3D\\CS\\");
	m_StaticClass = m_Assembly->GetClass("Vivid",name);


	auto gnode = m_Vivid->GetClass("Vivid.Scene", "GraphNode");
	auto gnode_inst = gnode->CreateInstance();

	m_GraphClass = gnode_inst;

	m_Instance = m_StaticClass->CreateInstance();
	m_Instance->SetFieldClass("Node", gnode_inst);

	gnode_inst->SetNativePtr("NodePtr", (void*)m_Owner);
	*/
	int b = 5;
}

void SharpComponent::OnPlay() {
	m_Instance->CallFunction("OnPlay");
	m_Playing = true;
}

void SharpComponent::OnStop() {

	m_Instance->CallFunction("OnStop");
	m_Playing = false;

}

void SharpComponent::OnRender(GraphNode* cam) {

	if (m_Playing) {

		auto gnode = m_Vivid->GetClass("Vivid.Scene", "GraphNode");
		auto gnode_inst = gnode->CreateInstance();

		//m_GraphClass = gnode_inst;

		gnode_inst->SetFieldValue("NodePtr", (void*)cam);

		//m_Instance = m_StaticClass->CreateInstance();
		//m_Instance->SetFieldClass("Node", gnode_inst);

		m_Instance->CallFunction("OnRender", gnode_inst);
	} if (m_Playing && m_Instance)
	{
		// To pass the camera to C#, we must wrap the C++ GraphNode*
		// in a C# GraphNode object instance.
		MClass* camNodeWrapper = CreateGraphNode();
		if (camNodeWrapper)
		{
			// Set the C# wrapper's internal pointer to our C++ camera object.
			camNodeWrapper->SetNativePtr("NodePtr", (void*)cam);

			// Call the C# OnRender method, passing the wrapped camera object.
			m_Instance->CallFunction("OnRender", camNodeWrapper);

			// Clean up the temporary wrapper object.
			delete camNodeWrapper;
		}
	}
}

void SharpComponent::OnUpdate(float deltaTime) {
	// Update logic here
	
	if (m_Playing) {
		std::cout << "OnsharpUpdate" << std::endl;
		m_Instance->CallFunction("OnUpdate", deltaTime);
	}
	//if (m_Instance) {
	//	m_Instance->CallMethod("Update", deltaTime);
//	}
}

MClass* SharpComponent::CreateGraphNode() {

	auto gnode = m_Vivid->GetClass("Vivid.Scene", "GraphNode");
	auto gnode_inst = gnode->CreateInstance();

	return gnode_inst;

}

void SharpComponent::ReInit()
{
	std::cout << "Re-initializing script: " << m_Name << std::endl;

	// 1. Clean up old, invalid Mono objects.
	// The C# objects they pointed to are already gone with the old AppDomain.
	// We are just cleaning up the C++ wrapper classes.
	delete m_Instance;
	delete m_GraphClass;
	m_Instance = nullptr;
	m_GraphClass = nullptr;
	m_StaticClass = nullptr;
	m_Assembly = nullptr;
	m_Vivid = nullptr;

	// 2. Call the original setup logic again.
	// The m_Name (the script's class name) was preserved. We use it to
	// find the class in the *new* AppDomain and set everything up again.
	// The empty "cs/" string for the first parameter of SetScript is a placeholder;
	// this function needs the class name, which it gets from m_Name.
	SetScript("cs/", m_Name);

	// 3. If the component was playing, call its OnPlay method again to restore state.
	if (m_Playing)
	{
		OnPlay();
	}
}
