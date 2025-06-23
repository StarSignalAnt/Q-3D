#include "pch.h"
#include "CubeRenderer.h"
#include "SceneGraph.h"
#include "CameraComponent.h"

#include "RenderTargetCube.h"
//#include "NodeCamera.h"
//#include "Node.h"
#include "GraphNode.h"
#include <math.h>



GraphNode* new_cam;


CubeRenderer::CubeRenderer(SceneGraph* graph, RenderTargetCube* target)
{

	m_Graph = graph;
	m_Target = target;

}

void CubeRenderer::RenderDepth(glm::vec3 pos, float maxZ,RenderTargetCube* m_Target) {

	auto prev_cam = m_Graph->GetCamera();
	if (new_cam == nullptr) {
		new_cam = new GraphNode;
		new_cam->AddComponent(new CameraComponent);
	}
	else {

	}

	auto pc = prev_cam->GetComponent<CameraComponent>();
	auto nc = new_cam->GetComponent<CameraComponent>();


	m_Graph->SetCamera(new_cam);
	new_cam->SetPosition(pos);
	nc->SetExtents(pc->GetExtents().x,maxZ);
//	new_cam->SetFarZ(maxZ);
	nc->SetFOV(90.0f);
	for (int i = 0; i < 6; i++) {

		switch (i) {
		case 0:
			new_cam->SetRotation(float4x4::RotationY(PI_F / 2.0f));
			break;
		case 1:
			new_cam->SetRotation(float4x4::RotationY(-PI_F / 2.0f));
			break;
		case 2:
			new_cam->SetRotation(float4x4::RotationX(-PI_F / 2.0f));
			break;
		case 3:
			new_cam->SetRotation(float4x4::RotationX(PI_F / 2.0f));
			break;
		case 4:
			new_cam->SetRotation(float4x4::Identity());
			break;
		case 5:
			new_cam->SetRotation(float4x4::RotationY(PI_F));
			break;
		}

		m_Target->Bind(i);
		m_Graph->RenderDepth();
		m_Target->Release(i);


	}

	m_Graph->SetCamera(prev_cam);

}

void CubeRenderer::RenderEnvironment(glm::vec3 pos) {

	
	auto prev_cam = m_Graph->GetCamera();
	if (new_cam == nullptr) {
		new_cam = new GraphNode;
		new_cam->AddComponent(new CameraComponent);
	}
	else {

	}

	auto pc = prev_cam->GetComponent<CameraComponent>();
	auto nc = new_cam->GetComponent<CameraComponent>();


	m_Graph->SetCamera(new_cam);
	new_cam->SetPosition(pos);
	nc->SetExtents(pc->GetExtents().x, pc->GetExtents().y);
	//	new_cam->SetFarZ(maxZ);
	nc->SetFOV(90.0f);
	for (int i = 0; i < 6; i++) {

		switch (i) {
		case 0:
			new_cam->SetRotation(float4x4::RotationY(PI_F / 2.0f));
			break;
		case 1:
			new_cam->SetRotation(float4x4::RotationY(-PI_F / 2.0f));
			break;
		case 2:
			new_cam->SetRotation(float4x4::RotationX(-PI_F / 2.0f));
			break;
		case 3:
			new_cam->SetRotation(float4x4::RotationX(PI_F / 2.0f));
			break;
		case 4:
			new_cam->SetRotation(float4x4::Identity());
			break;
		case 5:
			new_cam->SetRotation(float4x4::RotationY(PI_F));
			break;
		}

		m_Target->Bind(i);
		m_Graph->RenderDepth();
		m_Target->Release(i);


	}

	m_Graph->SetCamera(prev_cam);
	
}