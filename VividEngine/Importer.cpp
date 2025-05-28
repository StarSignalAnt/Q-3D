
#include "Importer.h"
#include "GraphNode.h"
#include "StaticMeshComponent.h"
#include "StaticRendererComponent.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <vector>
#include <string>
#include "RenderMaterial.h"
#include "MaterialBasic3D.h"

GraphNode* Importer::ImportEntity(std::string path) {

    GraphNode* node = new	GraphNode;
	node->SetName(path);

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_FlipUVs |
        aiProcess_CalcTangentSpace);

    if (!scene || !scene->HasMeshes()) {
        std::cerr << "Failed to load model: " << path << std::endl;
        return nullptr;
    }

    std::vector<RenderMaterial*> _mats;
    
    if (scene->HasMaterials()) {

        for (int i = 0;i < scene->mNumMaterials;i++) {

            aiMaterial * material = scene->mMaterials[i];
            std::string materialName = material->GetName().C_Str();
    
            RenderMaterial* mat = new MaterialBasic3D;

            _mats.push_back(mat);

        }

    }

    if (scene->HasMeshes()) {


        StaticMeshComponent* meshComponent = new StaticMeshComponent();

        for (int i = 0;i < scene->mNumMeshes;i++) {

            aiMesh* mesh = scene->mMeshes[i];

		

			SubMesh subMesh;


            for (int v = 0;v < mesh->mNumVertices;v++) {

                Vertex3 vertex;

				vertex.position = glm::vec3(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z);
				vertex.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); // Default color
				vertex.normal = glm::vec3(mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z);
				vertex.tangent = glm::vec3(mesh->mTangents[v].x, mesh->mTangents[v].y, mesh->mTangents[v].z);
				vertex.binormal = glm::vec3(mesh->mBitangents[v].x, mesh->mBitangents[v].y, mesh->mBitangents[v].z);
				vertex.uv = glm::vec3(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y, 0.0f); // Assuming UVs are in the first channel

				subMesh.m_Vertices.push_back(vertex);


            }

            for (int t = 0;t < mesh->mNumFaces;t++) {

                Tri3 tri;

				tri.v0 = mesh->mFaces[t].mIndices[0];
				tri.v1 = mesh->mFaces[t].mIndices[2];
				tri.v2 = mesh->mFaces[t].mIndices[1];

				subMesh.m_Triangles.push_back(tri);


            }

            subMesh.m_Material = new MaterialBasic3D;
			meshComponent->AddSubMesh(subMesh);

        }

        meshComponent->Finalize();

		node->AddComponent(meshComponent);

        auto renderer = new StaticRendererComponent;

        node->AddComponent(renderer);


    }

	return node;

}