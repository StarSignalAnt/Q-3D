#include "SkeletalMeshComponent.h"
#include "Animator.h"
#include "QEngine.h"
#include "MaterialSkeletal.h"
#include "MaterialDepth.h"
#include "MaterialSkeletalDepth.h"

SkeletalMeshComponent::SkeletalMeshComponent()
{
	// Constructor logic here
	for (int i = 0; i < 100; i++) {

		m_Bones.push_back(float4x4::Identity());

	}
}

void SkeletalMeshComponent::UpdateAnim(float delta) {

	m_Animator->UpdateAnimation(delta);

}


Diligent::float4x4 atov2(const aiMatrix4x4& assimpMat) {
	Diligent::float4x4 diligentMat;

	diligentMat.m00 = assimpMat.a1; diligentMat.m01 = assimpMat.a2; diligentMat.m02 = assimpMat.a3; diligentMat.m03 = assimpMat.a4;
	diligentMat.m10 = assimpMat.b1; diligentMat.m11 = assimpMat.b2; diligentMat.m12 = assimpMat.b3; diligentMat.m13 = assimpMat.b4;
	diligentMat.m20 = assimpMat.c1; diligentMat.m21 = assimpMat.c2; diligentMat.m22 = assimpMat.c3; diligentMat.m23 = assimpMat.c4;
	diligentMat.m30 = assimpMat.d1; diligentMat.m31 = assimpMat.d2; diligentMat.m32 = assimpMat.d3; diligentMat.m33 = assimpMat.d4;

	return diligentMat.Transpose();
}


glm::vec3 atov(aiVector3D v)
{

	return glm::vec3(v.x, v.y, v.z);

}

glm::mat4 atov(const aiMatrix4x4& aiMat) {
	glm::mat4 glmMat;

	glmMat[0][0] = aiMat.a1; glmMat[1][0] = aiMat.a2; glmMat[2][0] = aiMat.a3; glmMat[3][0] = aiMat.a4;
	glmMat[0][1] = aiMat.b1; glmMat[1][1] = aiMat.b2; glmMat[2][1] = aiMat.b3; glmMat[3][1] = aiMat.b4;
	glmMat[0][2] = aiMat.c1; glmMat[1][2] = aiMat.c2; glmMat[2][2] = aiMat.c3; glmMat[3][2] = aiMat.c4;
	glmMat[0][3] = aiMat.d1; glmMat[1][3] = aiMat.d2; glmMat[2][3] = aiMat.d3; glmMat[3][3] = aiMat.d4;

	return glmMat;
}

SkSubMesh SkeletalMeshComponent::ProcessMesh(aiMesh* mesh, aiScene* scene, bool buf_cre)
{

	SkSubMesh res;

	res.m_Material = new MaterialSkeletal;
	res.m_DepthMaterial = new MaterialSkeletalDepth;

	//res->SetOwner(this);

	for (int i = 0; i < mesh->mNumVertices; i++) {

		SkVertex3 v;

		v.position = atov(mesh->mVertices[i]);
		v.normal = atov(mesh->mNormals[i]);
		v.tangent = atov(mesh->mTangents[i]);
		v.binormal = atov(mesh->mBitangents[i]);
		v.uv = atov(mesh->mTextureCoords[0][i]);
		v.uv.y = 1.0f - v.uv.y;
		v.color = glm::vec4(1, 1, 1, 1);
		res.AddVertex(v, true);


	}

	ExtractBoneWeights(res, mesh, scene);

	//m_Meshes.push_back(res);


	for (int i = 0; i < mesh->mNumFaces; i++) {
		SkTri3 t;
		t.v0 = mesh->mFaces[i].mIndices[0];
		t.v1 = mesh->mFaces[i].mIndices[2];
		t.v2 = mesh->mFaces[i].mIndices[1];

		res.m_Triangles.push_back(t);

		//Triangle t2;

		//t2.v0 = t.v0;
		//t2.v1 = t.v2;

		//t2.v2 = t.v1;


	}
	m_SubMeshes.push_back(res);
	if (buf_cre) {
		Finalize();
	}


	return res;
}

void SkeletalMeshComponent::Finalize() {


	for (auto& sub : m_SubMeshes) {


		float* data = new float[sub.m_Vertices.size() * 27];

		int i = 0;
		for (auto v : sub.m_Vertices) {

			data[i++] = v.position.x;
			data[i++] = v.position.y;
			data[i++] = v.position.z;
			data[i++] = v.color.x;
			data[i++] = v.color.y;
			data[i++] = v.color.z;
			data[i++] = v.color.w;
			data[i++] = v.uv.x;
			data[i++] = v.uv.y;
			data[i++] = v.uv.z;
			data[i++] = v.normal.x;
			data[i++] = v.normal.y;
			data[i++] = v.normal.z;
			data[i++] = v.binormal.x;
			data[i++] = v.binormal.y;
			data[i++] = v.binormal.z;
			data[i++] = v.tangent.x;
			data[i++] = v.tangent.y;
			data[i++] = v.tangent.z;
			data[i++] = v.boneIndices.x;
			data[i++] = v.boneIndices.y;
			data[i++] = v.boneIndices.z;
			data[i++] = v.boneIndices.w;
			data[i++] = v.boneWeights.x;
			data[i++] = v.boneWeights.y;
			data[i++] = v.boneWeights.z;
			data[i++] = v.boneWeights.w;


		}

		BufferDesc vbuf;
		vbuf.Name = "SkMesh Buffer VB";
		vbuf.Usage = USAGE_IMMUTABLE;
		vbuf.BindFlags = BIND_VERTEX_BUFFER;
		vbuf.Size = (Uint64)(sizeof(SkVertex3) * sub.m_Vertices.size());

		BufferData vdata;
		vdata.DataSize = vbuf.Size;
		vdata.pData = data;

		QEngine::GetDevice()->CreateBuffer(vbuf, &vdata, &sub.VertexBuffer);

		BufferDesc ibuf;

		ibuf.Name = "Mesh Buffer IB";
		ibuf.Usage = USAGE_IMMUTABLE;
		ibuf.BindFlags = BIND_INDEX_BUFFER;
		ibuf.Size = (Uint64)(sizeof(Uint32) * sub.m_Triangles.size() * 3);

		Uint32* indices = new Uint32[sub.m_Triangles.size() * 3];

		i = 0;
		for (auto t : sub.m_Triangles)
		{
			indices[i++] = t.v0;
			indices[i++] = t.v1;
			indices[i++] = t.v2;
		}


		BufferData idata;
		idata.DataSize = ibuf.Size;
		idata.pData = indices;

		QEngine::GetDevice()->CreateBuffer(ibuf, &idata, &sub.IndexBuffer);

	}


}


void SkeletalMeshComponent::ExtractBoneWeights(SkSubMesh& mesh, aiMesh* amesh, aiScene* scene) {

	for (int i = 0; i < amesh->mNumBones; i++) {

		int boneID = -1;
		std::string name = amesh->mBones[i]->mName.C_Str();
		if (m_BoneInfoMap.count(name) == 0) {

			BoneInfo* bone = new BoneInfo;
			bone->ID = m_BoneCounter;
			bone->Offset = atov2(amesh->mBones[i]->mOffsetMatrix);
			m_BoneInfoMap[name] = bone;
			boneID = m_BoneCounter;
			m_BoneCounter++;

		}
		else {
			boneID = m_BoneInfoMap[name]->ID;
		}

		auto weights = amesh->mBones[i]->mWeights;
		int num_weights = amesh->mBones[i]->mNumWeights;

		for (int weightIndex = 0; weightIndex < num_weights; weightIndex++)
		{
			int vert_id = weights[weightIndex].mVertexId;
			float weight = weights[weightIndex].mWeight;
			auto v = SetVertexBoneData(mesh.GetVertex(vert_id), i, weight);
			mesh.SetVertex(vert_id, v);
		}


	}


}


SkVertex3 SkeletalMeshComponent::SetVertexBoneData(SkVertex3 vertex, int boneID, float weight) {

	if (vertex.boneIndices.x < 0)
	{
		vertex.boneIndices.x = boneID;
		vertex.boneWeights.x = weight;
		return vertex;
	}
	if (vertex.boneIndices.y < 0)
	{
		vertex.boneIndices.y = boneID;
		vertex.boneWeights.y = weight;
		return vertex;
	}
	if (vertex.boneIndices.z < 0)
	{
		vertex.boneIndices.z = boneID;
		vertex.boneWeights.z = weight;
		return vertex;
	}
	if (vertex.boneIndices.w < 0)
	{
		vertex.boneIndices.w = boneID;
		vertex.boneWeights.w = weight;
		return vertex;
	}
	return vertex;
}

void SkeletalMeshComponent::OnUpdate(float dt) {

	UpdateAnim(dt);
	m_Bones = m_Animator->GetBones();

}