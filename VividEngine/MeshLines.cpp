#include "pch.h"
#include "MeshLines.h"
#include "MeshLinesBuffer.h"
#include "Engine.h"
#include "MaterialMeshLines.h"

MaterialMeshLines* m_LUTMat = nullptr;

MeshLines::MeshLines() {

	if (m_LUTMat == nullptr) {

		m_LUTMat = new MaterialMeshLines;

	}
	m_Material = m_LUTMat;

}

std::vector<LineVertex> MeshLines::GetVertices() {

	return m_Vertices;

}

std::vector<Line> MeshLines::GetLines() {

	return m_Lines;

}

void MeshLines::CreateBuffer() {

	m_Buffer = new MeshLinesBuffer;
	m_Buffer->SetMeshLines(this);

}

void MeshLines::Render() {

	Uint64 offsets = 0;

	m_Material->Bind(false);

	IBuffer* pBuffs[] = { m_Buffer->GetVertexBuffer() };

	Engine::m_pImmediateContext->SetVertexBuffers(0, 1, pBuffs, &offsets, RESOURCE_STATE_TRANSITION_MODE_TRANSITION, SET_VERTEX_BUFFERS_FLAG_RESET);
	Engine::m_pImmediateContext->SetIndexBuffer(m_Buffer->GetIndexBuffer(), 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

	Engine::m_pImmediateContext->CommitShaderResources(m_Material->GetSRB(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

	DrawIndexedAttribs attrib;
	attrib.IndexType = VALUE_TYPE::VT_UINT32;
	attrib.NumIndices = m_Lines.size() * 2;
	attrib.Flags = DRAW_FLAG_VERIFY_ALL;
	Engine::m_pImmediateContext->DrawIndexed(attrib);

}