
#include "Importer.h"
#include "GraphNode.h"
#include "StaticMeshComponent.h"
#include "StaticRendererComponent.h"
#include "StaticDepthRendererComponent.h"
#include "SkeletalMeshComponent.h"
#include "SkeletalRendererComponent.h"
#include "SkeletalDepthRendererComponent.h"
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
#include "Animation.h"
#include "Animator.h"
#include <random>
#include "QEngine.h"
#include "GameContent.h"
#include "ContentFile.h"
namespace fs = std::filesystem; // For C++17 compatibility
Q3D::Engine::Texture::Texture2D* LoadTexture(const std::string& path) {
    // Assume this loads and returns a new Texture2D*, or nullptr on failure
    return new Q3D::Engine::Texture::Texture2D(path); // Placeholder logic
}
Q3D::Engine::Texture::Texture2D* FindTexture(const aiMaterial* material, aiTextureType type, const std::string& modelDir) {
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

std::string GetFolderPath(const std::string& filePath) {
    // Copy and normalize to backslashes
    std::string normalizedPath = filePath;
    std::replace(normalizedPath.begin(), normalizedPath.end(), '/', '\\');

    // Find last backslash
    size_t lastSlash = normalizedPath.find_last_of('\\');
    if (lastSlash == std::string::npos)
        return ""; // No slash found, likely no folder

    std::string folderPath = normalizedPath.substr(0, lastSlash + 1);

    // Ensure it ends with a single backslash
    if (folderPath.back() != '\\') {
        folderPath += '\\';
    }

    return folderPath;
}
std::string GenerateRandomFileName(size_t length = 12) {
    static const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    static std::mt19937 rng(std::random_device{}());
    static std::uniform_int_distribution<> dist(0, sizeof(charset) - 2); // -2 to exclude null terminator

    std::string result;
    result.reserve(length);
    for (size_t i = 0; i < length; ++i) {
        result += charset[dist(rng)];
    }

    return result;
}
GraphNode* Importer::ImportEntity(std::string path,bool gen_lod) {
    std::string modelDir = fs::path(path).parent_path().string();
    Assimp::Importer importer;

    auto con = Q3D::Engine::QEngine::GetContent()->FindContent(path);

    aiScene* scene = nullptr;

  //  con = nullptr;
    if (con != nullptr) {

        void* data = con->LoadResource();
        std::string ext = con->GetExtension();
        const char* hint = ext.c_str() + (ext.rfind('.', 0) == 0 ? 1 : 0);

        scene = (aiScene*)importer.ReadFileFromMemory(data, (size_t)con->GetSize(),
            aiProcess_Triangulate |
            aiProcess_GenNormals |
            aiProcess_FlipUVs |
            aiProcess_CalcTangentSpace, hint);

    }
    else {
        scene = (aiScene*)importer.ReadFile(path,
            aiProcess_Triangulate |
            aiProcess_GenNormals |
            aiProcess_FlipUVs |
            aiProcess_CalcTangentSpace);
    }
    if (!scene || !scene->HasMeshes()) {
        std::cerr << "Failed to load model: " << path << std::endl;
        return nullptr;
    }

    // Materials (can be improved to extract properties)
    std::vector<RenderMaterial*> materials;
    if (scene->HasMaterials()) {
        for (int i = 0; i < scene->mNumMaterials; i++) {


            aiMaterial* aiMat = scene->mMaterials[i];
            
            auto m_name = aiMat->GetName();

            if (std::string(m_name.C_Str()) == "")
            {
                m_name = GenerateRandomFileName(8);

            }

            auto m_path = GetFolderPath(path);

            m_path = m_path + std::string(m_name.C_Str());

            auto fp = m_path + ".material";

            if (VFile::Exists(fp.c_str())) {



                bool found = false;
                for (auto m : Q3D::Engine::QEngine::GetActiveMaterials()) {

                    if (m->GetName() == m_name.C_Str())
                    {
                        materials.push_back(m);
                        found = true;
                        break;
                    }
                }
                if (found) continue;

                MaterialPBR* pbr = MaterialProducer::m_Instance->GetPBR();
                
                pbr->Load(m_path);
                pbr->SetName(m_name.C_Str());
                
                
                auto am = Q3D::Engine::QEngine::GetActiveMaterials();
                am.push_back(pbr);
                Q3D::Engine::QEngine::SetActiveMaterials(am);
                
                materials.push_back(pbr);
                continue;

            }

            auto material = MaterialProducer::m_Instance->GetPBR();
            material->SetName(m_name.C_Str());

            // Albedo / Base Color
            if (Q3D::Engine::Texture::Texture2D* tex = FindTexture(aiMat, aiTextureType_DIFFUSE, modelDir))
            {

           //     material->SetColorTexture(tex);
            //    material->SetTexture(tex, 0);
            }
            if (Q3D::Engine::Texture::Texture2D* tex = FindTexture(aiMat, aiTextureType_SPECULAR, modelDir))
            {


                //material->SetSpecularTexture(tex);
            }
            if (Q3D::Engine::Texture::Texture2D* tex = FindTexture(aiMat, aiTextureType_NORMALS, modelDir))
            {
              //  material->SetNormalTexture(tex);
            }
            //aterial->SetNormal(tex);

            if (Q3D::Engine::Texture::Texture2D* tex = FindTexture(aiMat, aiTextureType_METALNESS, modelDir))
            {

              //  material->SetMetallicTexture(tex);
            }
            //material->SetMetallic(tex);


            if (Q3D::Engine::Texture::Texture2D* tex = FindTexture(aiMat, aiTextureType_DIFFUSE_ROUGHNESS, modelDir))
            {
            //    material->SetRoughnessTexture(tex);
            }
            else {
                aiString path;
                if (aiMat->GetTexture(aiTextureType_UNKNOWN,0, &path) == AI_SUCCESS) {
                    // Attempt to load this texture as roughness or investigate its usage
                //    Texture2D* tex = FindTexture(aiMat, aiTextureType_UNKNOWN, modelDir);
                    if (tex) {
              //          material->SetRoughnessTexture(tex);
                        break;  // If you assume only one roughness map per material
                    }
                }
            }
            //  material->SetRoughness(tex);

            if (Q3D::Engine::Texture::Texture2D* tex = FindTexture(aiMat, aiTextureType_AMBIENT_OCCLUSION, modelDir))
                //    material->SetAO(tex);
            {
            }

            material->Save(m_path);
            materials.push_back(material);
            auto am = Q3D::Engine::QEngine::GetActiveMaterials();
            am.push_back(material);
            Q3D::Engine::QEngine::SetActiveMaterials(am);


            //QEngine::m_ActiveMaterials.push_back(material);
        }
    }

    // Mesh creation cache
    std::vector<SubMesh*> subMeshes;

    for (int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];
        SubMesh* subMesh = new SubMesh;
        subMeshes.push_back(subMesh);

        LODLevel* lod = new LODLevel;


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
            lod->m_Vertices.push_back(vertex);
        }

        for (int f = 0; f < mesh->mNumFaces; f++) {
            const aiFace& face = mesh->mFaces[f];
            if (face.mNumIndices == 3) {
                Tri3 tri{ face.mIndices[0], face.mIndices[2], face.mIndices[1] };
                lod->m_Triangles.push_back(tri);
            }
        }

        subMesh->m_LODs.push_back(lod);
        subMesh->m_Material = materials[mesh->mMaterialIndex];  // Use shared material
        subMesh->m_DepthMaterial = MaterialProducer::m_Instance->GetDepth();
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
            node->AddComponent(meshComponent);
            for (unsigned int i = 0; i < ainode->mNumMeshes; i++) {
                unsigned int meshIndex = ainode->mMeshes[i];
                meshComponent->AddSubMesh(subMeshes[meshIndex]);
            }
            
            for (auto sub : meshComponent->GetSubMeshes()) {

                if (gen_lod) {
                    sub->GenerateLod(4);
                }
            }
            meshComponent->Finalize();
         
            

            auto renderer = new StaticRendererComponent;
            node->AddComponent(renderer);
            auto depth_renderer = new StaticDepthRendererComponent;
			node->AddComponent(depth_renderer);
            node->SetResource("", ResourceType::Static);
            node->SetScale(glm::vec3(1, 1, 1));
        }

        // Recurse through children
        for (unsigned int i = 0; i < ainode->mNumChildren; i++) {
            GraphNode* childNode = ProcessNode(ainode->mChildren[i]);
            //childNode->SetResource("", ResourceType::SubData);
            node->AddNode(childNode);
            node->SetScale(glm::vec3(1, 1, 1));

        }

        return node;
        };

    GraphNode* root = ProcessNode(scene->mRootNode);
    root->SetResource(path, ResourceType::Static);
    return root;

}

GraphNode* Importer::ImportSkeletal(std::string path) {
    // This function would be similar to ImportEntity but handle skeletal meshes
    // For now, we can return nullptr or implement a similar logic as ImportEntity
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

    SkeletalMeshComponent* mesh_com = new SkeletalMeshComponent;

    for (int i = 0;i < scene->mNumMeshes;i++) {

        auto mesh = mesh_com->ProcessMesh(scene->mMeshes[i], (aiScene*)scene, false);;
        
    }

    mesh_com->Finalize();

    auto anim = new Animation((aiScene*)scene, scene->mAnimations[0],mesh_com);
    auto animator = new Animator(anim);
	mesh_com->SetAnimator(animator);

	GraphNode* node = new GraphNode;
    node->AddComponent(mesh_com);
    node->AddComponent(new SkeletalRendererComponent);
    node->AddComponent(new SkeletalDepthRendererComponent);



    return node;


}