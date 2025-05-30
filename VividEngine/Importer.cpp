
#include "Importer.h"
#include "GraphNode.h"
#include "StaticMeshComponent.h"
#include "StaticRendererComponent.h"
#include "StaticDepthRendererComponent.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <vector>
#include <string>
#include "RenderMaterial.h"
#include "MaterialBasic3D.h"
#include "MaterialDepth.h"
#include "Texture2D.h"
#include "MaterialPBR.h"
#include <filesystem> // C++17 or later

namespace fs = std::filesystem; // For C++17 compatibility
Texture2D* LoadTexture(const std::string& path) {
    // Assume this loads and returns a new Texture2D*, or nullptr on failure
    return new Texture2D(path); // Placeholder logic
}
Texture2D* FindTexture(const aiMaterial* material, aiTextureType type, const std::string& modelDir) {
    if (material->GetTextureCount(type) == 0)
        return nullptr;

    aiString texPath;
    if (material->GetTexture(type, 0, &texPath) != AI_SUCCESS)
        return nullptr;

    std::string texStr = texPath.C_Str();

    // Try full path from Assimp
    if (fs::exists(texStr))
        return LoadTexture(texStr);

    // Try filename in working directory
    fs::path filename = fs::path(texStr).filename();
    if (fs::exists(filename))
        return LoadTexture(filename.string());

    // Try filename in model's directory
    fs::path localPath = fs::path(modelDir) / filename;
    if (fs::exists(localPath))
        return LoadTexture(localPath.string());

    return nullptr;
}

GraphNode* Importer::ImportEntity(std::string path) {
    std::string modelDir = fs::path(path).parent_path().string();
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

    // Materials (can be improved to extract properties)
    std::vector<RenderMaterial*> materials;
    if (scene->HasMaterials()) {
        for (int i = 0; i < scene->mNumMaterials; i++) {


            aiMaterial* aiMat = scene->mMaterials[i];
            auto* material = new MaterialPBR;

            // Albedo / Base Color
            if (Texture2D* tex = FindTexture(aiMat, aiTextureType_DIFFUSE, modelDir))
            {

                material->SetColorTexture(tex);
            }
            if (Texture2D* tex = FindTexture(aiMat, aiTextureType_SPECULAR, modelDir))
            {


                //material->SetSpecularTexture(tex);
            }
            if (Texture2D* tex = FindTexture(aiMat, aiTextureType_NORMALS, modelDir))
            {
                material->SetNormalTexture(tex);
            }
            //aterial->SetNormal(tex);

            if (Texture2D* tex = FindTexture(aiMat, aiTextureType_METALNESS, modelDir))
            {
                material->SetMetallicTexture(tex);
            }
            //material->SetMetallic(tex);


            if (Texture2D* tex = FindTexture(aiMat, aiTextureType_DIFFUSE_ROUGHNESS, modelDir))
            {
                material->SetRoughnessTexture(tex);
            }
            else {
                aiString path;
                if (aiMat->GetTexture(aiTextureType_UNKNOWN,0, &path) == AI_SUCCESS) {
                    // Attempt to load this texture as roughness or investigate its usage
                    Texture2D* tex = FindTexture(aiMat, aiTextureType_UNKNOWN, modelDir);
                    if (tex) {
                        material->SetRoughnessTexture(tex);
                        break;  // If you assume only one roughness map per material
                    }
                }
            }
            //  material->SetRoughness(tex);

            if (Texture2D* tex = FindTexture(aiMat, aiTextureType_AMBIENT_OCCLUSION, modelDir))
                //    material->SetAO(tex);
            {
            }


            materials.push_back(material);
        }
    }

    // Mesh creation cache
    std::vector<SubMesh> subMeshes(scene->mNumMeshes);

    for (int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];
        SubMesh& subMesh = subMeshes[i];

        for (int v = 0; v < mesh->mNumVertices; v++) {
            Vertex3 vertex;
            vertex.position = glm::vec3(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z);
            vertex.color = glm::vec4(1.0f);
            vertex.normal = glm::vec3(mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z);
            vertex.tangent = glm::vec3(mesh->mTangents[v].x, mesh->mTangents[v].y, mesh->mTangents[v].z);
            vertex.binormal = glm::vec3(mesh->mBitangents[v].x, mesh->mBitangents[v].y, mesh->mBitangents[v].z);
            vertex.uv = glm::vec3(mesh->mTextureCoords[0] ? mesh->mTextureCoords[0][v].x : 0.0f,
                mesh->mTextureCoords[0] ? mesh->mTextureCoords[0][v].y : 0.0f,
                0.0f);
            subMesh.m_Vertices.push_back(vertex);
        }

        for (int f = 0; f < mesh->mNumFaces; f++) {
            const aiFace& face = mesh->mFaces[f];
            if (face.mNumIndices == 3) {
                Tri3 tri{ face.mIndices[0], face.mIndices[2], face.mIndices[1] };
                subMesh.m_Triangles.push_back(tri);
            }
        }

        subMesh.m_Material = materials[mesh->mMaterialIndex];  // Use shared material
        subMesh.m_DepthMaterial = new MaterialDepth;
    }

    // Recursive function to process scene nodes
    std::function<GraphNode* (aiNode*)> ProcessNode = [&](aiNode* ainode) -> GraphNode* {
        GraphNode* node = new GraphNode;
        node->SetName(ainode->mName.C_Str());

        // Convert aiMatrix4x4 to glm::mat4
        aiVector3D pos, scale;
        aiQuaternion rot;
        ainode->mTransformation.Decompose(scale, rot, pos);
        glm::vec3 position(pos.x, pos.y, pos.z);
        glm::quat rotation(rot.w, rot.x, rot.y, rot.z);
        glm::vec3 scaling(scale.x, scale.y, scale.z);

        node->SetPosition(position);

        node->SetScale(scaling);
        // Convert quaternion to euler angles (in degrees)
        glm::quat q(rot.w, rot.x, rot.y, rot.z);
        glm::vec3 eulerRadians = glm::eulerAngles(q);
        glm::vec3 eulerDegrees = glm::degrees(eulerRadians);
        node->SetRotation(eulerDegrees);


        // Meshes for this node
        if (ainode->mNumMeshes > 0) {
            StaticMeshComponent* meshComponent = new StaticMeshComponent;
            for (unsigned int i = 0; i < ainode->mNumMeshes; i++) {
                unsigned int meshIndex = ainode->mMeshes[i];
                meshComponent->AddSubMesh(subMeshes[meshIndex]);
            }
            meshComponent->Finalize();
            node->AddComponent(meshComponent);
            

            auto renderer = new StaticRendererComponent;
            node->AddComponent(renderer);
            auto depth_renderer = new StaticDepthRendererComponent;
			node->AddComponent(depth_renderer);
        }

        // Recurse through children
        for (unsigned int i = 0; i < ainode->mNumChildren; i++) {
            GraphNode* childNode = ProcessNode(ainode->mChildren[i]);
            node->AddNode(childNode);
        }

        return node;
        };

    GraphNode* root = ProcessNode(scene->mRootNode);
    return root;

}