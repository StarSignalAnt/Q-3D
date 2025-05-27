#include "pch.h"
#include "PPEmissive.h"
#include "Engine.h"
#include "SceneGraph.h"
#include "MaterialEmissive.h"
#include "Draw2D.h"
#include "MaterialBlur.h"
#include "MaterialCombine.h"
PPEmissive::PPEmissive() {

	//	CreateTarget(Engine::GetFrameWidth(), Engine::GetFrameHeight());
	//	CreateTarget(Engine::GetFrameWidth() / 4, Engine::GetFrameHeight() / 4);



	CreateTargets(Engine::GetFrameWidth(), Engine::GetFrameHeight(), 4);
//	m_ColorLimit = new MaterialColorLimit;
//	m_Combine = new MaterialCombine;
//	m_Blur = new MaterialBlur;
	m_Emissive = new MaterialEmissive;
	//m_ColorLimit = new MaterialColorLimit;
	m_Combine = new MaterialCombine;
	m_Blur = new MaterialBlur;

}

Texture2D* PPEmissive::Process(Texture2D* frame) {


	BindRT(0);


	//GetGraph()->LinesOff();
	GetGraph()->RenderForcedMaterial(m_Emissive);
	//GetGraph()->LinesOn();

	ReleaseRT(0);

	BindRT(1);

	m_Draw->SetMaterial((Material2D*)m_Blur);
	m_Draw->Rect(GetTexture(0), float2(0, Engine::GetFrameHeight()), float2(Engine::GetFrameWidth(), -Engine::GetFrameHeight()), float4(1, 1, 1, 1));


	ReleaseRT(1);


	BindRT(2);

	m_Draw->SetMaterial((Material2D*)m_Combine);
	m_Combine->SetAux(frame);
	m_Draw->Rect(GetTexture(1), float2(0, 0), float2(Engine::GetFrameWidth(), Engine::GetFrameHeight()), float4(1, 1, 1, 1));


	ReleaseRT(2);


	return GetTexture(2);

}