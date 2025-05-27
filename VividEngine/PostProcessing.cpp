#include "pch.h"
#include "PostProcessing.h"
#include "PostProcess.h"
#include "Draw2D.h"
#include "Texture2D.h"
#include "Engine.h"



PostProcessing::PostProcessing() {

	m_Draw = new Draw2D;

}

void PostProcessing::AddPostProcess(PostProcess* process) {

	m_Processes.push_back(process);

}

void PostProcessing::Process(Texture2D* frame) {

	for (auto pp : m_Processes) {

		frame = pp->Process(frame);

	}

	m_Draw->Rect(frame, float2(0,0), float2(Engine::GetFrameWidth(), Engine::GetFrameHeight()),float4(1,1,1,1));

}