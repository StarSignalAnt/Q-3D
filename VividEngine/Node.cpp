#include "pch.h"
#include "Node.h"
#include "Engine.h"
#include "MathsHelp.h"
#include "GameInput.h"

#ifndef M_PI_2

#define M_PI_2 1.57079632679489661923 // π/2

#endif

#include "BasicMath.hpp"
#include "ScriptHost.h"
#include "VClass.h"
#include "VVar.h"
#include "FileHelp.h"
#include "VFunction.h"
#include "VFile.h"
#include "NodeActor.h"
#include "Animator.h"
#include "Bone.h"
#include "VScope.h"
#include "VVar.h"
bool first_node = true;
using namespace Diligent;


float4x4 CreateRotationMatrix(float pitch, float yaw, float roll)
{
	// Convert degrees to radians


	float cosPitch = std::cos(pitch);
	float sinPitch = std::sin(pitch);
	float cosYaw = std::cos(yaw);
	float sinYaw = std::sin(yaw);
	float cosRoll = std::cos(roll);
	float sinRoll = std::sin(roll);

	float4x4 rotationMatrix;
	rotationMatrix[0][0] = cosYaw * cosRoll + sinYaw * sinPitch * sinRoll;
	rotationMatrix[0][1] = cosRoll * sinYaw * sinPitch - cosYaw * sinRoll;
	rotationMatrix[0][2] = cosPitch * sinYaw;
	rotationMatrix[0][3] = 0.0f;

	rotationMatrix[1][0] = cosPitch * sinRoll;
	rotationMatrix[1][1] = cosPitch * cosRoll;
	rotationMatrix[1][2] = -sinPitch;
	rotationMatrix[1][3] = 0.0f;

	rotationMatrix[2][0] = sinYaw * sinRoll * cosPitch - cosRoll * cosYaw;
	rotationMatrix[2][1] = cosYaw * sinRoll * cosPitch + sinYaw * cosRoll;
	rotationMatrix[2][2] = cosYaw * cosPitch;
	rotationMatrix[2][3] = 0.0f;

	rotationMatrix[3][0] = 0.0f;
	rotationMatrix[3][1] = 0.0f;
	rotationMatrix[3][2] = 0.0f;
	rotationMatrix[3][3] = 1.0f;

	return rotationMatrix;
}

void RotationMatrixToEulerAngles(const float4x4& rotationMatrix, float& pitch, float& yaw, float& roll)
{
	if (std::abs(rotationMatrix[2][0]) < 1.0f)
	{
		yaw = std::atan2(rotationMatrix[1][0], rotationMatrix[0][0]);
		pitch = std::asin(-rotationMatrix[2][0]);
		roll = std::atan2(rotationMatrix[2][1], rotationMatrix[2][2]);
	}
	else
	{
		// Gimbal lock case
		yaw = 0.0f;
		if (rotationMatrix[2][0] <= -1.0f)
		{
			pitch = M_PI_2; // 90 degrees
			roll = std::atan2(rotationMatrix[0][1], rotationMatrix[0][2]);
		}
		else
		{
			pitch = -M_PI_2; // -90 degrees
			roll = std::atan2(-rotationMatrix[0][1], -rotationMatrix[0][2]);
		}
	}

}


VVar* CF_Debug(const std::vector<VVar*>& args)
{
	int b = 5;

	std::vector<VVar*> vec(args);

	printf("Debug:");
	printf(vec[0]->ToString().c_str());
	printf("\n");
	printf("F:");
	printf("%f",vec[0]->ToFloat());
	printf("\n");

	//printf("Debug1:%d\n",vec[0]->ToInt());

 //   printf("Debug2:%f\n", vec[0]->ToFloat());

	return nullptr;
}


float3 CF_GetFloat3(VClass* vec)
{

	float x = vec->GetScope()->FindVar("X")->ToFloat();
	float y = vec->GetScope()->FindVar("Y")->ToFloat();
	float z = vec->GetScope()->FindVar("Z")->ToFloat();
	return float3(x, y, z);

}

VVar* CF_NodeGetPosition(const std::vector<VVar*>& args)
{
	auto c = (Node*)args[0]->ToC();
	int b = 5;

	auto pos = c->GetPosition();

	VVar* res = Engine::m_ScriptHost->CRVec3(pos.x, pos.y, pos.z);

	return res;
}

VVar* CF_NodeSetPosition(const std::vector<VVar*>& args)
{
	auto c = (Node*)args[0]->ToC();

	float3 v;

	v.x= args[1]->GetClassValue()->FindVar("X")->ToFloat();
	v.y = args[1]->GetClassValue()->FindVar("Y")->ToFloat();
	v.z = args[1]->GetClassValue()->FindVar("Z")->ToFloat();





	c->SetPosition(v);

	return nullptr;
}


VVar* CF_TurnNode(const std::vector<VVar*>& args)
{
	int b = 5;

	std::vector<VVar*> vec(args);
	
	//printf("Turn Node!\n");
	auto node = (Node*)vec[0]->ToC();

	node->Turn(vec[1]->ToFloat(), vec[2]->ToFloat(), vec[3]->ToFloat(),false);

	//printf("Debug1:%d\n",vec[0]->ToInt());

 //   printf("Debug2:%f\n", vec[0]->ToFloat());

	return nullptr;
}

VVar* CF_PlayAnimNode(const std::vector<VVar*>& args)
{

	auto act = (NodeActor*)args[0]->ToC();
	auto anim = args[1]->ToString();
	bool restart = args[2]->ToInt() == 1;
	int b = 5;

	act->GetAnimator()->PlayAnimation(anim);
	act->GetAnimator()->SetAnimation1("");
	act->GetAnimator()->SetAnimation2("");

	return nullptr;

}

VVar* CF_PlayAnim1Node(const std::vector<VVar*>& args)
{

	auto act = (NodeActor*)args[0]->ToC();
	auto anim = args[1]->ToString();
//	bool restart = args[2]->ToInt() == 1;
	int b = 5;

	act->GetAnimator()->SetAnimation1(anim);


	return nullptr;

}

VVar* CF_PlayAnim2Node(const std::vector<VVar*>& args)
{

	auto act = (NodeActor*)args[0]->ToC();
	auto anim = args[1]->ToString();
//	bool restart = args[2]->ToInt() == 1;
	int b = 5;

	act->GetAnimator()->SetAnimation2(anim);


	return nullptr;

}

VVar* CF_InputKeyDown(const std::vector<VVar*> args) {


	auto key = args[0]->ToInt();


	VVar* res = new VVar;

	if (GameInput::This->GetKey(key))
	{

		res->SetInt(1);
	}
	else {
		res->SetInt(0);
	}
	res->SetType(T_Int);
	return res;
}

VVar* CF_GetBoneNode(const std::vector<VVar*> args) {

	auto act = (NodeActor*)args[0]->ToC();
	auto b_name = args[1]->ToString();

	auto b1 = act->FindBone(b_name);

	int b = 5;

	VVar* r = new VVar;
	r->SetType(T_CObject);
	r->SetC(b1);

	return r;
}

VVar* CF_SetRotateBone(const std::vector<VVar*> args)
{

	auto bone = (Bone*)args[0]->ToC();
	auto p = args[1]->ToFloat();
	auto y = args[2]->ToFloat();
	auto r = args[3]->ToFloat();

	bone->OffsetRotate(float3(p, y, r));

	return nullptr;

}

VVar* CF_InputMouseMoveX(const std::vector<VVar*> args)
{

	VVar* r = new VVar;
	r->SetType(T_Float);
	r->SetFloat(GameInput::This->m_MouseDelta.x);

	return r;

}

VVar* CF_InputMouseMoveY(const std::vector<VVar*> args)
{

	VVar* r = new VVar;
	r->SetType(T_Float);
	r->SetFloat(GameInput::This->m_MouseDelta.y);

	return r;

}




VVar* CF_LookAtNode(const std::vector<VVar*> args)
{

	auto node = (Node*)args[0]->ToC();
	auto pos = args[1]->GetClassValue();
	auto pf = CF_GetFloat3(pos);

	node->LookAt(pf);


	return nullptr;

}

VVar* CF_GetWorldNode(const std::vector<VVar*> args) {

	auto node = (Node*)args[0]->ToC();
	auto res = new VVar;
	res->SetClassType("Matrix");
	
	auto mat = ScriptHost::m_This->CreateInstance("Matrix");

	auto nm = node->GetWorldMatrix();

	auto nm2 = new float4x4;

	nm2->m00 = nm.m00;
	nm2->m10 = nm.m10;
	nm2->m20 = nm.m20;
	nm2->m30 = nm.m30;

	nm2->m01 = nm.m01;
	nm2->m11 = nm.m11;
	nm2->m21 = nm.m21;
	nm2->m31 = nm.m31;

	nm2->m02 = nm.m02;
	nm2->m12 = nm.m12;
	nm2->m22 = nm.m22;
	nm2->m32 = nm.m32;

	nm2->m03 = nm.m03;
	nm2->m13 = nm.m13;
	nm2->m23 = nm.m23;
	nm2->m33 = nm.m33;



	auto cc = mat->GetScope()->FindVar("C");

	cc->SetC(nm2);
	

	res->SetClassValue(mat);


	return res;



}

VVar* CF_VecTimesMatrix(const std::vector<VVar*> args)
{

	auto vec = args[0]->GetClassValue();
	auto mat = args[1]->GetClassValue();

	float3 v = CF_GetFloat3(vec);
	float4x4 m = *(float4x4*)mat->GetScope()->FindVar("C")->ToC();



	float3 res = v * m;

	VVar* r = new VVar;
	r->SetClassType("Vec3");
	r->SetClassValue(ScriptHost::m_This->CreateInstance("Vec3"));
	r->GetClassValue()->GetScope()->FindVar("X")->SetFloat(res.x);
	r->GetClassValue()->GetScope()->FindVar("Y")->SetFloat(res.y);
	r->GetClassValue()->GetScope()->FindVar("Z")->SetFloat(res.z);


	return r;

}

VVar* CF_TransformNode(const std::vector<VVar*> args)
{

	auto node = (Node*)args[0]->ToC();
	auto vec = args[1]->GetClassValue();

	auto v3 = ScriptHost::m_This->GetVec3(vec);

	auto r = node->TransformVector(v3);


	return ScriptHost::m_This->CRVec3(r.x, r.y, r.z);

}

Node::Node() {

	if (first_node) {

		Engine::m_ScriptHost->AddCFunction("TurnNode", CF_TurnNode);
		Engine::m_ScriptHost->AddCFunction("Debug", CF_Debug);
		Engine::m_ScriptHost->AddCFunction("GetPositionNode", CF_NodeGetPosition);
		Engine::m_ScriptHost->AddCFunction("SetPositionNode", CF_NodeSetPosition);
		Engine::m_ScriptHost->AddCFunction("PlayAnimNode", CF_PlayAnimNode);
		Engine::m_ScriptHost->AddCFunction("InputKeyDown", CF_InputKeyDown);
		Engine::m_ScriptHost->AddCFunction("PlayAnim1Node", CF_PlayAnim1Node);
		Engine::m_ScriptHost->AddCFunction("PlayAnim2Node", CF_PlayAnim2Node);
		Engine::m_ScriptHost->AddCFunction("GetBoneNode", CF_GetBoneNode);
		Engine::m_ScriptHost->AddCFunction("SetRotateBone", CF_SetRotateBone);
		Engine::m_ScriptHost->AddCFunction("InputMouseMoveX", CF_InputMouseMoveX);
		Engine::m_ScriptHost->AddCFunction("InputMouseMoveY", CF_InputMouseMoveY);
		Engine::m_ScriptHost->AddCFunction("LookAtNode", CF_LookAtNode);
		Engine::m_ScriptHost->AddCFunction("GetWorldNode", CF_GetWorldNode);
		Engine::m_ScriptHost->AddCFunction("VecTimesMatrix", CF_VecTimesMatrix);
		Engine::m_ScriptHost->AddCFunction("TransformNode", CF_TransformNode);
		first_node = false;
	}
	m_Rotation = float4x4::Identity();
	m_Position = float3(0, 0, 0);
	m_Scale = float3(1, 1, 1);



	

	int b = 5;
}

void Node::AddNode(Node* node) {

	m_Nodes.push_back(node);
	node->SetRoot(this);

}

int Node::NodesCount() {

	return m_Nodes.size();

}

Node* Node::GetNode(int index) {

	return m_Nodes[index];

}

std::vector<Node*> Node::GetNodes()
{

	return m_Nodes;

}

void Node::SetPosition(float3 position) {

	if (position.x == m_Position.x && position.y == m_Position.y && position.z == m_Position.z)
	{
		return;
	}
	m_Position = position;
	
	
	BuildGeo();
	Updated();

}

float3 Node::GetPosition() {


	//auto matrix = GetWorldMatrix();
	//return float3(matrix.m30, matrix.m31, matrix.m32);


	return m_Position;
}

void Node::SetRotation(float pitch, float yaw, float roll,bool edit) {

	yaw = MathsHelp::Deg2Rad(yaw);
	pitch = MathsHelp::Deg2Rad(pitch);
	roll = MathsHelp::Deg2Rad(roll);
	BuildGeo();

	//m_Rotation = CreateRotationMatrix(pitch, yaw, roll);

	if (edit) {
		m_Rotation = float4x4::RotationY(yaw) * float4x4::RotationX(pitch) * float4x4::RotationZ(roll);
	}
	else {
		m_Rotation = float4x4::RotationX(pitch) * float4x4::RotationY(yaw) * float4x4::RotationZ(roll);
	}
	Updated();
}

void Node::SetScale(float3 scale) {

	m_Scale = scale;
	BuildGeo();
	Updated();
}

void Node::Move(float3 delta) {

	if (m_Root != nullptr) {
		m_Position = m_Position + ((delta*m_Rotation));
	}
	else {
		m_Position = m_Position + (delta * m_Rotation);
	}
	BuildGeo();

	Updated();
}

void Node::Translate(float3 delta) {

	if (m_Root == nullptr) {
		m_Position = m_Position + delta;
	}
	else {
		m_Position = m_Position + (delta*m_Root->GetRotation().Inverse());
	}
	BuildGeo();

	Updated();

}

float3 Node::TransformVector(float3 vector) {

	return vector * m_Rotation;

}

void Node::Rotate(float pitch, float yaw, float roll) {


	yaw = MathsHelp::Deg2Rad(yaw);
	pitch = MathsHelp::Deg2Rad(pitch);
	roll = MathsHelp::Deg2Rad(roll);
	
	Updated();

}

void RotationMatrixToEulerAngles2(const float4x4& rotationMatrix, float& pitch, float& yaw, float& roll)
{
	yaw = std::atan2(rotationMatrix[0][2], rotationMatrix[2][2]);
	pitch = std::asin(-rotationMatrix[1][2]);
	roll = std::atan2(rotationMatrix[1][0], rotationMatrix[1][1]);

	// Convert radians to degrees and normalize
	pitch = MathsHelp::Rad2Deg(pitch);
	yaw = MathsHelp::Rad2Deg(yaw);
	roll = MathsHelp::Rad2Deg(roll);

}

float3 Node::GetRotationEU() {

	float x, y, z;
	 RotationMatrixToEulerAngles2(m_Rotation, x, y, z);
	 return float3(x, y, z);

	//float p, y, r;
//	RotationMatrixToEulerAngles(m_Rotation.Inverse() , p, y, r);
//	return float3(MathsHelp::Rad2Deg(p), MathsHelp::Rad2Deg(y), MathsHelp::Rad2Deg(r));

}

void Node::Turn(float pitch, float yaw, float roll,bool local) {

	yaw = MathsHelp::Deg2Rad(yaw);
	pitch = MathsHelp::Deg2Rad(pitch);
	roll = MathsHelp::Deg2Rad(roll);


	float4x4 turn = float4x4::RotationX(pitch) * float4x4::RotationY(yaw) * float4x4::RotationZ(roll);

	if (local) {
		m_Rotation = turn * m_Rotation;
		
	}
	else {

		m_Rotation = m_Rotation * turn;

	}
	BuildGeo();
	Updated();
}

float4x4 Node::GetWorldMatrix() {


	float4x4 root = float4x4::Identity();

	if (m_Root != nullptr) {
		root = m_Root->GetWorldMatrix();
	}

	float4x4 translationMatrix = float4x4::Translation(m_Position);
	float4x4 rotationMatrix = m_Rotation;
	float4x4 scaleMatrix = float4x4::Scale(m_Scale);

	// Combine the transformation matrices in the correct order (scale * rotation * translation)
	float4x4 worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

	return worldMatrix * root;

}

void Node::Update(float delta) {

	//Turn(0, 0.1f, 0, false);

	if (m_IsPlaying) {
		UpdateScripts(delta);
	}


	for (auto n : m_Nodes) {

		n->Update(delta);

	}

}

void Node::UpdateScripts(float delta) {

	if (m_IsPlaying) {
		for (auto gs : m_Scripts) {

			auto up_func = gs->FindFunction("Update");

			VVar* nv = new VVar;

			nv->SetType(T_Float);
			nv->SetFloat(delta);

			std::vector<VVar*> vars;

			vars.push_back(nv);
			nv->SetName("delta");
			up_func->Call(vars);


		}
	}

}

void Node::AddScript(std::string path) {


	auto name = FileHelp::GetFileName(path);

	Engine::m_ScriptHost->LoadModule(path);

	auto test_i = Engine::m_ScriptHost->CreateInstance(name);

	m_Scripts.push_back(test_i);


	//m_NodeClass = Engine::m_ScriptHost->CreateInstance("Node");


	VVar* node_v = test_i->FindVar("node");

	m_NodeClass = node_v->GetClassValue();
	auto obj = m_NodeClass->FindVar("C");



	//auto f1 = m_NodeClass->FindFunction("Node");

	//f1->Call(nullptr);


	//auto obj = m_NodeClass->FindVar("C");
//	auto pos = m_NodeClass->FindVar("Position");
//	auto scale = m_NodeClass->FindVar("Scale");
//	auto rot = m_NodeClass->FindVar("Rotation");
//
//	auto p_c = pos->GetClassValue()->FindVar("C");
//	auto s_c = scale->GetClassValue()->FindVar("C");
//	auto r_c = rot->GetClassValue()->FindVar("C");

//	p_c->SetC(&m_Position);
//	s_c->SetC(&m_Scale);
//	rot->SetC(&m_Rotation);

	obj->SetC((void*)this);




}

void Node::BeginPlay() {

	Push();


	m_IsPlaying = true;
	for (auto node : m_Nodes)
	{
		node->BeginPlay();
	}

}

void Node::Stop() {
	Pop();
	
	m_IsPlaying = false;

	for (auto node : m_Nodes) {
		node->Stop();
	}

}

void Node::Push() {
	m_PushedPosition = m_Position;
	m_PushedRotation = m_Rotation;
	m_PushedScale = m_Scale;
}

void Node::Pop() {
	m_Position = m_PushedPosition;
	m_Rotation = m_PushedRotation;
	m_Scale = m_PushedScale;

}

void Node::SetStatic(bool stat) {

	m_Static = stat;
	
}

bool Node::GetStatic() {

	return m_Static;

}

std::vector<VClass*> Node::GetScripts() {

	return m_Scripts;

}

std::string Node::GetFullName() {

	std::string root = "";

	if (m_Root != nullptr) {

		root = m_Root->GetFullName();
	}

	if (root != "")
	{
		return root + "." + m_Name;
	}
	return m_Name;

}

void Node::SetRoot(Node* node) {

	m_Root = node;

}

void Node::SetResourcePath(std::string path) {

	m_ResourcePath = path;

}

std::string Node::GetResourcePath() {

	return m_ResourcePath;

}

void Node::WriteNode(VFile* file) {

	file->WriteInt(1);
	file->WriteVec3(m_Position);
	file->WriteMatrix(m_Rotation);
	file->WriteVec3(m_Scale);
	file->WriteString(m_Name.c_str());

	WriteScripts(file);

	file->WriteInt(m_Nodes.size());

	for (auto node : m_Nodes) {

		node->WriteNode(file);

	}


}

void Node::ReadNode(VFile* file) {

	m_Position = file->ReadVec3();
	m_Rotation = file->ReadMatrix();
	m_Scale = file->ReadVec3();
	m_Name = file->ReadString();

}

void Node::RenderChildrenForcedMaterial(MaterialBase* material) {
	for (auto node : m_Nodes) {
		node->RenderForcedMaterial(material);
	}
}

void Node::RenderForcedMaterial(MaterialBase* material)
{
	RenderChildrenForcedMaterial(material);
}

VClass* Node::ReadClass(VFile* file) {

	std::string type = file->ReadString();

	auto ncls = ScriptHost::m_This->CreateInstance(type);


	VVar* node_v = ncls->FindVar("node");

	if (node_v != nullptr) {
		if (m_NodeClass == nullptr) {
			m_NodeClass = node_v->GetClassValue();
			auto obj = m_NodeClass->FindVar("C");
			obj->SetC((void*)this);

		}
	}

	if (type == "Node")
	{
		auto obj = ncls->FindVar("C");
		obj->SetC((void*)this);
	}


	//auto f1 = m_NodeClass->FindFunction("Node");

	//f1->Call(nullptr);


	//auto obj = m_NodeClass->FindVar("C");
//	auto pos = m_NodeClass->FindVar("Position");
//	auto scale = m_NodeClass->FindVar("Scale");
//	auto rot = m_NodeClass->FindVar("Rotation");
//
//	auto p_c = pos->GetClassValue()->FindVar("C");
//	auto s_c = scale->GetClassValue()->FindVar("C");
//	auto r_c = rot->GetClassValue()->FindVar("C");

//	p_c->SetC(&m_Position);
//	s_c->SetC(&m_Scale);
//	rot->SetC(&m_Rotation);


	int vc = file->ReadInt();
	for (int i = 0; i < vc; i++) {

		auto v = ncls->GetScope()->GetVars()[i];
		TokenType type = (TokenType)file->ReadInt();
		switch (type) {
		case T_Int:
		case T_Number:
			v->SetInt(file->ReadInt());

			break;
		case T_Float:
		case T_FloatNumber:
			v->SetFloat(file->ReadFloat());

			break;
		case T_Class:

			auto type = file->ReadString();
			int ctype = file->ReadInt();
			if (ctype == 0)
			{
				continue;
			}
			if (ctype == 1) {
				v->SetClassValue(ReadClass(file));
			}
			if (ctype == 2) {
				std::string data = file->ReadString();
				v->SetDataName(data);
			}

			break;
		}

	}
	int b = 5;
	
	return ncls;
}

void WriteClass(VClass* s, VFile* file) {

	file->WriteString(s->GetName().GetNames()[0].c_str());


	file->WriteInt(s->GetScope()->GetVars().size());
	for (auto v : s->GetScope()->GetVars()) {

		file->WriteInt(v->GetType());

		switch (v->GetType()) {
		case T_Int:
		case T_Number:


			file->WriteInt(v->ToInt());

			break;
		case T_Float:
		case T_FloatNumber:

			file->WriteFloat(v->ToFloat());
			break;
		case T_String:

			file->WriteString(v->ToString().c_str());
			break;
		case T_Class:

			file->WriteString(v->GetClassType().c_str());

			if (v->GetClassValue() == nullptr) {
				file->WriteInt(0);
			}
			else {
				auto cls = v->GetClassValue();
				if (cls->GetDataName() == "")
				{
					file->WriteInt(1);
					WriteClass(cls, file);
				}
				else {
					file->WriteInt(2);
					file->WriteString(cls->GetDataName().c_str());
				}

			}

			break;
		}

	}

}

void Node::WriteScripts(VFile* file) {


	file->WriteInt(m_Scripts.size());

	for (auto s : m_Scripts) {


		WriteClass(s, file);

		//s->WriteClass(file);


//
// 		file->WriteString(s->GetName().GetNames()[0].c_str());

	}

}

void Node::ReadScripts(VFile* file) {

	int sc = file->ReadInt();

	for (int i = 0; i < sc; i++) {

		auto s = ReadClass(file);
		m_Scripts.push_back(s);


	}

}

VClass* Node::FindClass(std::string name, int index)
{

	for (auto s : m_Scripts) {

		if (s->GetName().GetNames()[0] == name) {
			return s;
		}

		for (auto c : s->GetScope()->GetVars()) {

			if (c->GetClassType() == name)
			{
				return c->GetClassValue();
			}

		}

	}

	return nullptr;

}

float3 Cross(const float3& a, const float3& b)
{
	return float3(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	);
}
void Node::LookAt(float3 target) {

	float3 forward = normalize(target - m_Position);
	float3 right = normalize(Cross(float3(0,1,0), forward));
	float3 newUp = cross(forward, right);

	float4x4 rotation = float4x4(
		right.x, newUp.x, forward.x, 0.0f,
		right.y, newUp.y, forward.y, 0.0f,
		right.z, newUp.z, forward.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	m_Rotation = rotation.Inverse();
	//m_Position = 



}

void Node::Remove() {

	if (m_Root == nullptr) return;
	m_Root->RemoveNode(this);
	m_Root = nullptr;

}

void Node::RemoveNode(Node* node) {

	auto it = std::find(m_Nodes.begin(), m_Nodes.end(), node);

	// Check if the object was found and remove it
	if (it != m_Nodes.end()) {
		m_Nodes.erase(it);
		std::cout << "Object removed.\n";
	}
	else {
		std::cout << "Object not found.\n";
	}

}