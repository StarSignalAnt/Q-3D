#include "pch.h"
#include "Draw2D.h"
#include "Texture2D.h"
#include "Mesh3D.h"
#include "Material2D.h"

Draw2D::Draw2D() {

	m_Material = new Material2D;
	m_Material->Create();

}

void Draw2D::Rect(Texture2D* image, float2 pos, float2 size, float4 color) {

	auto mesh = new Mesh3D();

	Vertex v1, v2, v3, v4;

	v1.position = float3(pos.x, pos.y,0.1f);
	v2.position = float3(pos.x + size.x, pos.y, 0.1f);
	v3.position = float3(pos.x + size.x, pos.y + size.y, 0.1f);
	v4.position = float3(pos.x, pos.y + size.y, 0.1f);
	v1.color = color;
	v2.color = color;
	v3.color = color;
	v4.color = color;
	v1.texture = float3(0, 0, 0);
	v2.texture = float3(1, 0, 0);
	v3.texture = float3(1, 1, 0);
	v4.texture = float3(0, 1, 0);

	mesh->AddVertex(v1, false);
	mesh->AddVertex(v2, false);
	mesh->AddVertex(v3, false);
	mesh->AddVertex(v4, false);

	Triangle t1, t2;

	t1.v0 = 0;
	t1.v1 = 1;
	t1.v2 = 2;

	t2.v0 = 2;
	t2.v1 = 3;
	t2.v2 = 0;

	mesh->AddTri(t1);
	mesh->AddTri(t2);

	mesh->Build();

	m_Material->SetDiffuse(image);
	m_Material->Bind(false);


	mesh->SetMaterial(m_Material);

	mesh->Render(false);


}