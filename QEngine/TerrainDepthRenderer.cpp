#include "TerrainDepthRenderer.h"
#include "CameraComponent.h"
#include "SceneGraph.h"
#include "GraphNode.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "TerrainMeshComponent.h"
#include "TerrainMesh.h"
#include "TerrainMeshBuffer.h"
#include "TerrainLayer.h"
#include "MaterialTerrainDepth.h"

TerrainDepthRenderer::TerrainDepthRenderer() {


	m_TerrainDepth = new MaterialTerrainDepth;

}

void TerrainDepthRenderer::OnRenderDepth(GraphNode* camera) {

	auto mesh = m_Owner->GetComponent<TerrainMeshComponent>();
	int b = 5;


	bool add = false;
	auto light = SceneGraph::m_CurrentGraph->GetLights()[0];
	//	for (auto light : SceneGraph::m_CurrentGraph->GetLights()) {




		int ic = mesh->GetMesh()->GetTriangles().size() * 3;
		auto vbuf = mesh->GetMesh()->GetBuffer()->GetVertexBuffer();
		auto tbuf = mesh->GetMesh()->GetBuffer()->GetTriangleBuffer();

		auto mat = m_TerrainDepth;
		mat->SetIndexCount(ic);
		mat->SetBuffer(vbuf, 0);
		mat->SetBuffer(tbuf, 1);
		mat->SetMatrix(glm::inverse(camera->GetWorldMatrix()), 0);
		mat->SetMatrix(m_Owner->GetWorldMatrix(), 1);
		mat->SetMatrix(camera->GetComponent<CameraComponent>()->GetProjectionMatrix(), 2);
		mat->SetCameraPosition(camera->GetPosition());
		mat->SetCameraExt(camera->GetComponent<CameraComponent>()->GetExtents());
		//	mat->SetTexture(sub.m_Material->GetColorTexture(), 0);
		mat->SetLight(light);
		mat->Bind(add);
		mat->Render();


		//	}
			//add = true;

	//	}
	
}