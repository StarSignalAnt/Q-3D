#include "PPSSR.h"
#include "Engine.h"
#include "SceneGraph.h"
#include "Draw2D.h"
#include "MaterialColorLimit.h"
#include "MaterialBlur.h"
#include "MaterialCombine.h"
#include "MaterialSSR.h"
#include "Texture2D.h"
#include "TextureDepth.h"

PPSSR::PPSSR() {

	CreateTargets(Engine::GetFrameWidth(), Engine::GetFrameHeight(), 4);
	m_SSR = new MaterialSSR;

	int b = 5;

}

void PPSSR::SetNormals(Texture2D* normals) {

	m_Normals = normals;

}

void PPSSR::SetDepth(TextureDepth* depth) {

	m_Depth = depth;

}

void PPSSR::SetPositions(Texture2D* positions) {

	m_Positions = positions;

}

Texture2D* PPSSR::Process(Texture2D* frame) {


	BindRT(0);

	m_Draw->SetMaterial(m_SSR);
	m_SSR->SetColor(m_Color);
	m_SSR->SetNormals(m_Normals);
	m_SSR->SetDepth(m_Depth);
	m_SSR->SetPositions(m_Positions);

	m_Draw->Rect(frame, float2(0, 0), float2(Engine::GetFrameWidth(), Engine::GetFrameHeight()), float4(1, 1, 1, 1));


	ReleaseRT(0);
	/*
	BindRT(1);

	m_Draw->SetMaterial((Material2D*)m_ColorLimit);
	m_Draw->Rect(frame, float2(0, 0), float2(Engine::GetFrameWidth(), Engine::GetFrameHeight()), float4(1, 1, 1, 1));

	ReleaseRT(1);

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
	return GetTexture(0);
}