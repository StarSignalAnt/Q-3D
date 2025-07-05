#include "PPOutline.h"
#include "QEngine.h"
#include "SceneGraph.h"
#include "Draw2D.h"
#include "MaterialOutline.h"
#include "GraphNode.h"

//#include "MaterialColorLimit.h"
//#include "MaterialBlur.h"
//#include "MaterialCombine.h"

PPOutline::PPOutline(){

	//	CreateTarget(Engine::GetFrameWidth(), Engine::GetFrameHeight());
	//	CreateTarget(Engine::GetFrameWidth() / 4, Engine::GetFrameHeight() / 4);
	

	CreateTargets(QEngine::GetFrameWidth(), QEngine::GetFrameHeight(), 2);
//	m_ColorLimit = new MaterialColorLimit;
	//m_Combine = new MaterialCombine;
	//m_Blur = new MaterialBlur;

}

Texture2D* PPOutline::Process(Texture2D* frame) {




	BindRT(1);



	if (m_Node) {
		//m_Node->RenderDepth(m_Camera);
	}
	//m_Draw->SetMaterial((Material2D*)m_ColorLimit);
	//m_Draw->Rect(frame, float2(0, 0), float2(Engine::GetFrameWidth(), Engine::GetFrameHeight()), float4(1, 1, 1, 1));

	ReleaseRT(1);


	m_Draw->Rect(GetTexture(1), glm::vec2(0,0), glm::vec2(QEngine::GetFrameWidth(),QEngine::GetFrameHeight()), glm::vec4(1, 1, 1, 1));


	/*

	BindRT(2);

	m_Draw->SetMaterial((Material2D*)m_Blur);
	m_Draw->Rect(GetTexture(1), float2(0, 0), float2(Engine::GetFrameWidth(), Engine::GetFrameHeight()), float4(1, 1, 1, 1));


	ReleaseRT(2);

	BindRT(3);

	m_Draw->SetMaterial((Material2D*)m_Combine);
	m_Combine->SetAux(frame);
	m_Draw->Rect(GetTexture(2), float2(0, 0), float2(Engine::GetFrameWidth(), Engine::GetFrameHeight()), float4(1, 1, 1, 1));


	ReleaseRT(3);


	return GetTexture(3);
	*/
	return nullptr;
}

void PPOutline::SetNode(GraphNode* node) {

	m_Node = node;

}

void PPOutline::SetCam(GraphNode* cam) {

	m_Camera = cam;
	CreateDraw(cam);
	m_Draw->SetOverride(new MaterialOutline);
}