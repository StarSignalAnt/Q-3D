#include "pch.h"
#include "Mesh3D.h"
#include "MeshBuffer.h"
#include "Engine.h"
#include "MaterialBase.h"
#include "MaterialMeshLight.h"
#include "MaterialDepth.h"
#include "NodeEntity.h"
#include <immintrin.h>
#include <intrin.h>

Mesh3D::Mesh3D() {
	//m_Material = new MaterialBase;

}

void Mesh3D::AddVertex(Vertex vertex,bool reset) {


	if (reset) {
		//vertex.bone_ids = float4(-1, -1, -1, -1);
		//vertex.bone_weights = float4(0, 0, 0, 0);
		vertex.bone_ids = float4(-1, -1, -1, -1);
		vertex.bone_weights = float4(0, 0, 0, 0);
	}
	m_Vertices.push_back(vertex);
}

void Mesh3D::AddTri(Triangle tri) {

	m_Tris.push_back(tri);

}

std::vector<Vertex> Mesh3D::GetVertices() {

	return m_Vertices;

}

std::vector<Triangle> Mesh3D::GetTris() {

	return m_Tris;

}

void Mesh3D::Build() {

	m_Buffer = new MeshBuffer;
	m_Buffer->SetMesh(this);
	m_Buffer->Build();

}

void Mesh3D::BuildFast(float* v, Uint32* i,int verts,int tris) {

	m_Buffer = new MeshBuffer;
	m_Buffer->SetMeshFast(v, i,verts,tris);
	m_NumVerts = verts;
	m_NumTris = tris;
	m_VertsData = v;
	m_TrisData = i;


}

void Mesh3D::Render(bool sp) {

	

	Uint64 offsets = 0;

	
	
	IBuffer* pBuffs[] = { m_Buffer->GetVertexBuffer() };

	Engine::m_pImmediateContext->SetVertexBuffers(0, 1, pBuffs, &offsets, RESOURCE_STATE_TRANSITION_MODE_TRANSITION, SET_VERTEX_BUFFERS_FLAG_RESET);
	Engine::m_pImmediateContext->SetIndexBuffer(m_Buffer->GetIndexBuffer(), 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
	m_Material->Bind(sp);
	if (sp) {
		Engine::m_pImmediateContext->CommitShaderResources(m_Material->GetSecondPassSRB(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
	}
	else {
		Engine::m_pImmediateContext->CommitShaderResources(m_Material->GetSRB(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
	}
	DrawIndexedAttribs attrib;
	attrib.IndexType = VALUE_TYPE::VT_UINT32;
	if (m_Tris.size() > 0) {
		attrib.NumIndices = m_Tris.size() * 3;
	}
	else {
		attrib.NumIndices = m_NumTris * 3;
	}
	attrib.Flags = DRAW_FLAG_VERIFY_ALL;
	Engine::m_pImmediateContext->DrawIndexed(attrib);


}

void Mesh3D::RenderDepth() {

	Uint64 offsets = 0;


	IBuffer* pBuffs[] = { m_Buffer->GetVertexBuffer() };

	Engine::m_pImmediateContext->SetVertexBuffers(0, 1, pBuffs, &offsets, RESOURCE_STATE_TRANSITION_MODE_TRANSITION, SET_VERTEX_BUFFERS_FLAG_RESET);
	Engine::m_pImmediateContext->SetIndexBuffer(m_Buffer->GetIndexBuffer(), 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

	Engine::m_pImmediateContext->CommitShaderResources(m_DepthMaterial->GetSRB(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

	DrawIndexedAttribs attrib;
	attrib.IndexType = VALUE_TYPE::VT_UINT32;
	attrib.NumIndices = m_Tris.size() * 3;
	attrib.Flags = DRAW_FLAG_VERIFY_ALL;
	Engine::m_pImmediateContext->DrawIndexed(attrib);


}

void Mesh3D::SetOwner(NodeEntity* entity) {

	m_Owner = entity;

}

NodeEntity* Mesh3D::GetOwner() {

	return m_Owner;

}


void transform_points_avx512(const std::vector<float3>& points, const float4x4& matrix, std::vector<float3>& transformed_points) {
	size_t size = points.size();
	transformed_points.resize(size);

	// Broadcast matrix rows once before the loop
	__m512 row0 = _mm512_set_ps(matrix.m03, matrix.m02, matrix.m01, matrix.m00, matrix.m03, matrix.m02, matrix.m01, matrix.m00, matrix.m03, matrix.m02, matrix.m01, matrix.m00, matrix.m03, matrix.m02, matrix.m01, matrix.m00);
	__m512 row1 = _mm512_set_ps(matrix.m13, matrix.m12, matrix.m11, matrix.m10, matrix.m13, matrix.m12, matrix.m11, matrix.m10, matrix.m13, matrix.m12, matrix.m11, matrix.m10, matrix.m13, matrix.m12, matrix.m11, matrix.m10);
	__m512 row2 = _mm512_set_ps(matrix.m23, matrix.m22, matrix.m21, matrix.m20, matrix.m23, matrix.m22, matrix.m21, matrix.m20, matrix.m23, matrix.m22, matrix.m21, matrix.m20, matrix.m23, matrix.m22, matrix.m21, matrix.m20);
	__m512 row3 = _mm512_set_ps(matrix.m33, matrix.m32, matrix.m31, matrix.m30, matrix.m33, matrix.m32, matrix.m31, matrix.m30, matrix.m33, matrix.m32, matrix.m31, matrix.m30, matrix.m33, matrix.m32, matrix.m31, matrix.m30);

	for (size_t i = 0; i + 4 <= size; i += 4) {
		// Load four points
		__m512 px = _mm512_set_ps(points[i + 3].x, points[i + 3].x, points[i + 3].x, points[i + 3].x,
			points[i + 2].x, points[i + 2].x, points[i + 2].x, points[i + 2].x,
			points[i + 1].x, points[i + 1].x, points[i + 1].x, points[i + 1].x,
			points[i].x, points[i].x, points[i].x, points[i].x);
		__m512 py = _mm512_set_ps(points[i + 3].y, points[i + 3].y, points[i + 3].y, points[i + 3].y,
			points[i + 2].y, points[i + 2].y, points[i + 2].y, points[i + 2].y,
			points[i + 1].y, points[i + 1].y, points[i + 1].y, points[i + 1].y,
			points[i].y, points[i].y, points[i].y, points[i].y);
		__m512 pz = _mm512_set_ps(points[i + 3].z, points[i + 3].z, points[i + 3].z, points[i + 3].z,
			points[i + 2].z, points[i + 2].z, points[i + 2].z, points[i + 2].z,
			points[i + 1].z, points[i + 1].z, points[i + 1].z, points[i + 1].z,
			points[i].z, points[i].z, points[i].z, points[i].z);
		__m512 pw = _mm512_set1_ps(1.0f);

		// Compute transformed x
		__m512 result_x = _mm512_fmadd_ps(px, _mm512_broadcastss_ps(_mm_load_ss(&matrix.m00)),
			_mm512_fmadd_ps(py, _mm512_broadcastss_ps(_mm_load_ss(&matrix.m01)),
				_mm512_fmadd_ps(pz, _mm512_broadcastss_ps(_mm_load_ss(&matrix.m02)),
					_mm512_mul_ps(pw, _mm512_broadcastss_ps(_mm_load_ss(&matrix.m03))))));

		// Compute transformed y
		__m512 result_y = _mm512_fmadd_ps(px, _mm512_broadcastss_ps(_mm_load_ss(&matrix.m10)),
			_mm512_fmadd_ps(py, _mm512_broadcastss_ps(_mm_load_ss(&matrix.m11)),
				_mm512_fmadd_ps(pz, _mm512_broadcastss_ps(_mm_load_ss(&matrix.m12)),
					_mm512_mul_ps(pw, _mm512_broadcastss_ps(_mm_load_ss(&matrix.m13))))));

		// Compute transformed z
		__m512 result_z = _mm512_fmadd_ps(px, _mm512_broadcastss_ps(_mm_load_ss(&matrix.m20)),
			_mm512_fmadd_ps(py, _mm512_broadcastss_ps(_mm_load_ss(&matrix.m21)),
				_mm512_fmadd_ps(pz, _mm512_broadcastss_ps(_mm_load_ss(&matrix.m22)),
					_mm512_mul_ps(pw, _mm512_broadcastss_ps(_mm_load_ss(&matrix.m23))))));

		// Store results
		float* xRes = reinterpret_cast<float*>(&result_x);
		float* yRes = reinterpret_cast<float*>(&result_y);
		float* zRes = reinterpret_cast<float*>(&result_z);

		transformed_points[i] = { xRes[0], yRes[0], zRes[0] };
		transformed_points[i + 1] = { xRes[4], yRes[4], zRes[4] };
		transformed_points[i + 2] = { xRes[8], yRes[8], zRes[8] };
		transformed_points[i + 3] = { xRes[12], yRes[12], zRes[12] };
	}

	// Handle the remaining points if size is not a multiple of 4
	for (size_t i = size - (size % 4); i < size; ++i) {
		__m512 px = _mm512_set1_ps(points[i].x);
		__m512 py = _mm512_set1_ps(points[i].y);
		__m512 pz = _mm512_set1_ps(points[i].z);
		__m512 pw = _mm512_set1_ps(1.0f);

		__m512 result_x = _mm512_fmadd_ps(px, _mm512_broadcastss_ps(_mm_load_ss(&matrix.m00)),
			_mm512_fmadd_ps(py, _mm512_broadcastss_ps(_mm_load_ss(&matrix.m01)),
				_mm512_fmadd_ps(pz, _mm512_broadcastss_ps(_mm_load_ss(&matrix.m02)),
					_mm512_mul_ps(pw, _mm512_broadcastss_ps(_mm_load_ss(&matrix.m03))))));

		__m512 result_y = _mm512_fmadd_ps(px, _mm512_broadcastss_ps(_mm_load_ss(&matrix.m10)),
			_mm512_fmadd_ps(py, _mm512_broadcastss_ps(_mm_load_ss(&matrix.m11)),
				_mm512_fmadd_ps(pz, _mm512_broadcastss_ps(_mm_load_ss(&matrix.m12)),
					_mm512_mul_ps(pw, _mm512_broadcastss_ps(_mm_load_ss(&matrix.m13))))));

		__m512 result_z = _mm512_fmadd_ps(px, _mm512_broadcastss_ps(_mm_load_ss(&matrix.m20)),
			_mm512_fmadd_ps(py, _mm512_broadcastss_ps(_mm_load_ss(&matrix.m21)),
				_mm512_fmadd_ps(pz, _mm512_broadcastss_ps(_mm_load_ss(&matrix.m22)),
					_mm512_mul_ps(pw, _mm512_broadcastss_ps(_mm_load_ss(&matrix.m23))))));

		float* xRes = reinterpret_cast<float*>(&result_x);
		float* yRes = reinterpret_cast<float*>(&result_y);
		float* zRes = reinterpret_cast<float*>(&result_z);

		transformed_points[i] = { xRes[0], yRes[0], zRes[0] };
	}
}

void transform_points_avx2(const std::vector<float3>& points, const float4x4& matrix, std::vector<float3>& transformed_points) {
	size_t size = points.size();
	transformed_points.resize(size);

	// Broadcast matrix rows once before the loop
	__m256 row0 = _mm256_set_ps(matrix.m03, matrix.m02, matrix.m01, matrix.m00, matrix.m03, matrix.m02, matrix.m01, matrix.m00);
	__m256 row1 = _mm256_set_ps(matrix.m13, matrix.m12, matrix.m11, matrix.m10, matrix.m13, matrix.m12, matrix.m11, matrix.m10);
	__m256 row2 = _mm256_set_ps(matrix.m23, matrix.m22, matrix.m21, matrix.m20, matrix.m23, matrix.m22, matrix.m21, matrix.m20);
	__m256 row3 = _mm256_set_ps(matrix.m33, matrix.m32, matrix.m31, matrix.m30, matrix.m33, matrix.m32, matrix.m31, matrix.m30);

	for (size_t i = 0; i + 4 <= size; i += 4) {
		// Load four points
		__m256 px = _mm256_set_ps(points[i + 3].x, points[i + 2].x, points[i + 1].x, points[i].x,
			points[i + 3].x, points[i + 2].x, points[i + 1].x, points[i].x);
		__m256 py = _mm256_set_ps(points[i + 3].y, points[i + 2].y, points[i + 1].y, points[i].y,
			points[i + 3].y, points[i + 2].y, points[i + 1].y, points[i].y);
		__m256 pz = _mm256_set_ps(points[i + 3].z, points[i + 2].z, points[i + 1].z, points[i].z,
			points[i + 3].z, points[i + 2].z, points[i + 1].z, points[i].z);
		__m256 pw = _mm256_set1_ps(1.0f);

		// Compute transformed x, y, z
		__m256 result_x = _mm256_fmadd_ps(px, _mm256_shuffle_ps(row0, row0, _MM_SHUFFLE(0, 0, 0, 0)),
			_mm256_fmadd_ps(py, _mm256_shuffle_ps(row0, row0, _MM_SHUFFLE(1, 1, 1, 1)),
				_mm256_fmadd_ps(pz, _mm256_shuffle_ps(row0, row0, _MM_SHUFFLE(2, 2, 2, 2)),
					_mm256_mul_ps(pw, _mm256_shuffle_ps(row0, row0, _MM_SHUFFLE(3, 3, 3, 3))))));

		__m256 result_y = _mm256_fmadd_ps(px, _mm256_shuffle_ps(row1, row1, _MM_SHUFFLE(0, 0, 0, 0)),
			_mm256_fmadd_ps(py, _mm256_shuffle_ps(row1, row1, _MM_SHUFFLE(1, 1, 1, 1)),
				_mm256_fmadd_ps(pz, _mm256_shuffle_ps(row1, row1, _MM_SHUFFLE(2, 2, 2, 2)),
					_mm256_mul_ps(pw, _mm256_shuffle_ps(row1, row1, _MM_SHUFFLE(3, 3, 3, 3))))));

		__m256 result_z = _mm256_fmadd_ps(px, _mm256_shuffle_ps(row2, row2, _MM_SHUFFLE(0, 0, 0, 0)),
			_mm256_fmadd_ps(py, _mm256_shuffle_ps(row2, row2, _MM_SHUFFLE(1, 1, 1, 1)),
				_mm256_fmadd_ps(pz, _mm256_shuffle_ps(row2, row2, _MM_SHUFFLE(2, 2, 2, 2)),
					_mm256_mul_ps(pw, _mm256_shuffle_ps(row2, row2, _MM_SHUFFLE(3, 3, 3, 3))))));

		// Store results
		float temp[24];
		_mm256_store_ps(temp, result_x);
		_mm256_store_ps(temp + 8, result_y);
		_mm256_store_ps(temp + 16, result_z);

		for (int j = 0; j < 4; ++j) {
			transformed_points[i + j] = { temp[j], temp[j + 8], temp[j + 16] };
		}
	}

	// Handle remaining points
	for (size_t i = size - (size % 4); i < size; ++i) {
		__m256 px = _mm256_set1_ps(points[i].x);
		__m256 py = _mm256_set1_ps(points[i].y);
		__m256 pz = _mm256_set1_ps(points[i].z);
		__m256 pw = _mm256_set1_ps(1.0f);

		__m256 result_x = _mm256_fmadd_ps(px, row0,
			_mm256_fmadd_ps(py, _mm256_permute_ps(row0, 0x55),
				_mm256_fmadd_ps(pz, _mm256_permute_ps(row0, 0xAA),
					_mm256_mul_ps(pw, _mm256_permute_ps(row0, 0xFF)))));

		__m256 result_y = _mm256_fmadd_ps(px, row1,
			_mm256_fmadd_ps(py, _mm256_permute_ps(row1, 0x55),
				_mm256_fmadd_ps(pz, _mm256_permute_ps(row1, 0xAA),
					_mm256_mul_ps(pw, _mm256_permute_ps(row1, 0xFF)))));

		__m256 result_z = _mm256_fmadd_ps(px, row2,
			_mm256_fmadd_ps(py, _mm256_permute_ps(row2, 0x55),
				_mm256_fmadd_ps(pz, _mm256_permute_ps(row2, 0xAA),
					_mm256_mul_ps(pw, _mm256_permute_ps(row2, 0xFF)))));

		float temp[24];
		_mm256_store_ps(temp, result_x);
		_mm256_store_ps(temp + 8, result_y);
		_mm256_store_ps(temp + 16, result_z);

		transformed_points[i] = { temp[0], temp[8], temp[16] };
	}
}
//

void transform_points_avx2_F(const float3* points, size_t size, const float4x4& matrix, float3* transformed_points) {
	// Broadcast matrix rows once before the loop
	__m256 row0 = _mm256_set_ps(matrix.m03, matrix.m02, matrix.m01, matrix.m00, matrix.m03, matrix.m02, matrix.m01, matrix.m00);
	__m256 row1 = _mm256_set_ps(matrix.m13, matrix.m12, matrix.m11, matrix.m10, matrix.m13, matrix.m12, matrix.m11, matrix.m10);
	__m256 row2 = _mm256_set_ps(matrix.m23, matrix.m22, matrix.m21, matrix.m20, matrix.m23, matrix.m22, matrix.m21, matrix.m20);
	__m256 row3 = _mm256_set_ps(matrix.m33, matrix.m32, matrix.m31, matrix.m30, matrix.m33, matrix.m32, matrix.m31, matrix.m30);

	for (size_t i = 0; i + 4 <= size; i += 4) {
		// Load four points
		__m256 px = _mm256_set_ps(points[i + 3].x, points[i + 2].x, points[i + 1].x, points[i].x,
			points[i + 3].x, points[i + 2].x, points[i + 1].x, points[i].x);
		__m256 py = _mm256_set_ps(points[i + 3].y, points[i + 2].y, points[i + 1].y, points[i].y,
			points[i + 3].y, points[i + 2].y, points[i + 1].y, points[i].y);
		__m256 pz = _mm256_set_ps(points[i + 3].z, points[i + 2].z, points[i + 1].z, points[i].z,
			points[i + 3].z, points[i + 2].z, points[i + 1].z, points[i].z);
		__m256 pw = _mm256_set1_ps(1.0f);

		// Compute transformed x, y, z
		__m256 result_x = _mm256_fmadd_ps(px, _mm256_shuffle_ps(row0, row0, _MM_SHUFFLE(0, 0, 0, 0)),
			_mm256_fmadd_ps(py, _mm256_shuffle_ps(row0, row0, _MM_SHUFFLE(1, 1, 1, 1)),
				_mm256_fmadd_ps(pz, _mm256_shuffle_ps(row0, row0, _MM_SHUFFLE(2, 2, 2, 2)),
					_mm256_mul_ps(pw, _mm256_shuffle_ps(row0, row0, _MM_SHUFFLE(3, 3, 3, 3))))));

		__m256 result_y = _mm256_fmadd_ps(px, _mm256_shuffle_ps(row1, row1, _MM_SHUFFLE(0, 0, 0, 0)),
			_mm256_fmadd_ps(py, _mm256_shuffle_ps(row1, row1, _MM_SHUFFLE(1, 1, 1, 1)),
				_mm256_fmadd_ps(pz, _mm256_shuffle_ps(row1, row1, _MM_SHUFFLE(2, 2, 2, 2)),
					_mm256_mul_ps(pw, _mm256_shuffle_ps(row1, row1, _MM_SHUFFLE(3, 3, 3, 3))))));

		__m256 result_z = _mm256_fmadd_ps(px, _mm256_shuffle_ps(row2, row2, _MM_SHUFFLE(0, 0, 0, 0)),
			_mm256_fmadd_ps(py, _mm256_shuffle_ps(row2, row2, _MM_SHUFFLE(1, 1, 1, 1)),
				_mm256_fmadd_ps(pz, _mm256_shuffle_ps(row2, row2, _MM_SHUFFLE(2, 2, 2, 2)),
					_mm256_mul_ps(pw, _mm256_shuffle_ps(row2, row2, _MM_SHUFFLE(3, 3, 3, 3))))));

		// Store results
		alignas(32) float temp[24]; // Ensure alignment for AVX2 store operations
		_mm256_store_ps(temp, result_x);
		_mm256_store_ps(temp + 8, result_y);
		_mm256_store_ps(temp + 16, result_z);

		for (int j = 0; j < 4; ++j) {
			transformed_points[i + j].x = temp[j];
			transformed_points[i + j].y = temp[j + 8];
			transformed_points[i + j].z = temp[j + 16];
		}
	}

	// Handle remaining points
	for (size_t i = size - (size % 4); i < size; ++i) {
		__m256 px = _mm256_set1_ps(points[i].x);
		__m256 py = _mm256_set1_ps(points[i].y);
		__m256 pz = _mm256_set1_ps(points[i].z);
		__m256 pw = _mm256_set1_ps(1.0f);

		__m256 result_x = _mm256_fmadd_ps(px, row0,
			_mm256_fmadd_ps(py, _mm256_permute_ps(row0, 0x55),
				_mm256_fmadd_ps(pz, _mm256_permute_ps(row0, 0xAA),
					_mm256_mul_ps(pw, _mm256_permute_ps(row0, 0xFF)))));

		__m256 result_y = _mm256_fmadd_ps(px, row1,
			_mm256_fmadd_ps(py, _mm256_permute_ps(row1, 0x55),
				_mm256_fmadd_ps(pz, _mm256_permute_ps(row1, 0xAA),
					_mm256_mul_ps(pw, _mm256_permute_ps(row1, 0xFF)))));

		__m256 result_z = _mm256_fmadd_ps(px, row2,
			_mm256_fmadd_ps(py, _mm256_permute_ps(row2, 0x55),
				_mm256_fmadd_ps(pz, _mm256_permute_ps(row2, 0xAA),
					_mm256_mul_ps(pw, _mm256_permute_ps(row2, 0xFF)))));

		alignas(32) float temp[24]; // Ensure alignment for AVX2 store operations
		_mm256_store_ps(temp, result_x);
		_mm256_store_ps(temp + 8, result_y);
		_mm256_store_ps(temp + 16, result_z);

		transformed_points[i].x = temp[0];
		transformed_points[i].y = temp[8];
		transformed_points[i].z = temp[16];
	}
}


//
void transform_points_avx2_OPTIMIZED(const std::vector<float3>& points, const float4x4& matrix, std::vector<float3>& transformed_points) {
	size_t size = points.size();
	transformed_points.resize(size);

	// Broadcast matrix rows once before the loop
	__m256 row0 = _mm256_set_ps(matrix.m03, matrix.m02, matrix.m01, matrix.m00,
		matrix.m03, matrix.m02, matrix.m01, matrix.m00);
	__m256 row1 = _mm256_set_ps(matrix.m13, matrix.m12, matrix.m11, matrix.m10,
		matrix.m13, matrix.m12, matrix.m11, matrix.m10);
	__m256 row2 = _mm256_set_ps(matrix.m23, matrix.m22, matrix.m21, matrix.m20,
		matrix.m23, matrix.m22, matrix.m21, matrix.m20);

	for (size_t i = 0; i < size; i += 8) {
		// Load eight points
		__m256 px = _mm256_set_ps(points[i + 7].x, points[i + 6].x, points[i + 5].x, points[i + 4].x,
			points[i + 3].x, points[i + 2].x, points[i + 1].x, points[i].x);
		__m256 py = _mm256_set_ps(points[i + 7].y, points[i + 6].y, points[i + 5].y, points[i + 4].y,
			points[i + 3].y, points[i + 2].y, points[i + 1].y, points[i].y);
		__m256 pz = _mm256_set_ps(points[i + 7].z, points[i + 6].z, points[i + 5].z, points[i + 4].z,
			points[i + 3].z, points[i + 2].z, points[i + 1].z, points[i].z);
		__m256 pw = _mm256_set1_ps(1.0f);

		// Compute transformed x, y, z using FMA
		__m256 result_x = _mm256_fmadd_ps(px, row0,
			_mm256_fmadd_ps(py, row1,
				_mm256_fmadd_ps(pz, row2,
					_mm256_broadcast_ss(&matrix.m03))));

		__m256 result_y = _mm256_fmadd_ps(px, _mm256_permute_ps(row0, 0x55),
			_mm256_fmadd_ps(py, _mm256_permute_ps(row1, 0x55),
				_mm256_fmadd_ps(pz, _mm256_permute_ps(row2, 0x55),
					_mm256_broadcast_ss(&matrix.m13))));

		__m256 result_z = _mm256_fmadd_ps(px, _mm256_permute_ps(row0, 0xAA),
			_mm256_fmadd_ps(py, _mm256_permute_ps(row1, 0xAA),
				_mm256_fmadd_ps(pz, _mm256_permute_ps(row2, 0xAA),
					_mm256_broadcast_ss(&matrix.m23))));

		// Store results
		_mm256_storeu_ps(&transformed_points[i].x, result_x);
		_mm256_storeu_ps(&transformed_points[i].y, result_y);
		_mm256_storeu_ps(&transformed_points[i].z, result_z);
	}
}
//fma3

//fma3


bool supports_avx512() {
	int cpuInfo[4];
	__cpuid(cpuInfo, 0);
	int numIds = cpuInfo[0];

	if (numIds >= 7) {
		__cpuidex(cpuInfo, 7, 0);
		return (cpuInfo[1] & (1 << 16)) != 0;  // AVX512F is bit 16 of EBX
	}

	return false;
}


void Mesh3D::BuildGeo() {

	if (n_NeedsRebuild == false) return;
	//printf("REBUILDING GEO: Tris:%d\n",GetTris().size());
	auto world = m_Owner->GetWorldMatrix().Transpose();

	int stime = clock();



	if (m_FBuild==nullptr) {
		auto tris = GetTris();
		auto verts = GetVertices();


		//m_Build.clear();


		m_FBuild = new float3[tris.size() * 3];
		int i = 0;


		for (auto t : tris) {

			float3 v0 = verts[t.v0].position;// *world;
			float3 v1 = verts[t.v1].position;// *world;
			float3 v2 = verts[t.v2].position;// *world;
		//	m_Build.push_back(v0);
		//	m_Build.push_back(v1);
			//m_Build.push_back(v2);
			m_FBuild[i++] = verts[t.v0].position;
			m_FBuild[i++] = verts[t.v1].position;
			m_FBuild[i++] = verts[t.v2].position;

		

		}

	}
	if (m_FUseBuild == nullptr)
	{
		//m_FUseBuild.clear();
		m_FUseBuild = new float3[m_Tris.size() * 3];
	
	}
	
	if (supports_avx512()) {
		transform_points_avx512(m_Build, world, m_UseBuild);
	}
	else {
		//transform_points_avx2_optimized(m_Build, world, m_UseBuild);

		transform_points_avx2_F(m_FBuild,m_Tris.size()*3, world, m_FUseBuild);
	}


	int et = clock();

	printf("TimeGEO:%d\n", et - stime);

	n_NeedsRebuild = false;

}


float* Mesh3D::GetVertexData() {

	float* data = new float[m_Vertices.size() * 27];
	int i = 0;
	for (auto v : m_Vertices) {

		data[i++] = v.position.x;
		data[i++] = v.position.y;
		data[i++] = v.position.z;
		data[i++] = v.color.x;
		data[i++] = v.color.y;
		data[i++] = v.color.z;
		data[i++] = v.color.w;
		data[i++] = v.texture.x;
		data[i++] = v.texture.y;
		data[i++] = v.texture.z;
		data[i++] = v.normal.x;
		data[i++] = v.normal.y;
		data[i++] = v.normal.z;
		data[i++] = v.binormal.x;
		data[i++] = v.binormal.y;
		data[i++] = v.binormal.z;
		data[i++] = v.tangent.x;
		data[i++] = v.tangent.y;
		data[i++] = v.tangent.z;
		data[i++] = v.bone_ids.x;
		data[i++] = v.bone_ids.y;
		data[i++] = v.bone_ids.z;
		data[i++] = v.bone_ids.w;
		data[i++] = v.bone_weights.x;
		data[i++] = v.bone_weights.y;
		data[i++] = v.bone_weights.z;
		data[i++] = v.bone_weights.w;

	}

	return data;

}

Uint32* Mesh3D::GetTriData() {

	Uint32* data = new Uint32[m_Tris.size() * 3];

	int i = 0;
	for (auto t : m_Tris) {

		data[i++] = t.v0;
		data[i++] = t.v1;
		data[i++] = t.v2;

	}

	return data;

}