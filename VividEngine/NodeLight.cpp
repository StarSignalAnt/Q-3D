#include "pch.h"
#include "NodeLight.h"
#include "RenderTargetCube.h"
#include "VFile.h"

NodeLight::NodeLight() {

	m_Diffuse = float3(1, 1, 1);
	m_Specular = float3(1, 1, 1);
	m_Range = 30;
	m_ShadowMap = new RenderTargetCube(2048,2048,true);
	m_Name = "Light";
}

float3 NodeLight::GetDiffuse()
{

	return m_Diffuse;

}

float3 NodeLight::GetSpecular() {

	return m_Specular;

}

float NodeLight::GetRange() {

	return m_Range;

}

void NodeLight::WriteNode(VFile* file) {

	file->WriteInt(3);
	file->WriteVec3(m_Position);
	file->WriteMatrix(m_Rotation);
	file->WriteVec3(m_Scale);
	file->WriteString(m_Name.c_str());
	file->WriteFloat(m_Range);
	file->WriteVec3(m_Diffuse);
	file->WriteVec3(m_Specular);

	WriteScripts(file);


	file->WriteInt(m_Nodes.size());

	for (auto node : m_Nodes) {

		node->WriteNode(file);

	}

}

void NodeLight::ReadNode(VFile* file) {

	m_Position = file->ReadVec3();
	m_Rotation = file->ReadMatrix();
	m_Scale = file->ReadVec3();
	m_Name = file->ReadString();
	m_Range = file->ReadFloat();
	m_Diffuse = file->ReadVec3();
	m_Specular = file->ReadVec3();


}