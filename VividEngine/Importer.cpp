#include "pch.h"
#include "Importer.h"
#include "VFile.h"
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh3D.h"
#include "data.h"
#include "BasicMath.hpp"
#include "NodeEntity.h"
#include "MaterialMeshLight.h"
#include "MaterialDepth.h"
#include <filesystem>
#include "Texture2D.h"
#include "NodeActor.h"
#include "Animator.h"
#include "Animation.h"
#include "MaterialActorLight.h"
#include "MaterialActorDepth.h"
#include "MaterialMeshPBR.h"
#include <optional>
namespace fs = std::filesystem;
using namespace Diligent;
std::string getFileNameWithoutExtension(const std::string& fileName) {
    std::filesystem::path filePath(fileName);
    return filePath.stem().string();
}
std::string ExtractFilename(const std::string& path) {
    // Find the last occurrence of the directory separator
    size_t lastSlash = path.find_last_of("/\\");
    if (lastSlash == std::string::npos) {
        // No directory separator found, return the entire path
        return path;
    }
    else {
        // Extract the substring after the last directory separator
        return path.substr(lastSlash + 1);
    }
}

std::string getDirectoryPath(const std::string& fullPath) {
    size_t lastSlashPos = fullPath.find_last_of("/\\");
    if (lastSlashPos == std::string::npos) {
        // No slash found, return an empty string or handle appropriately
        return "";
    }
    return fullPath.substr(0, lastSlashPos + 1);
}


std::string extractFilenameAlone(const std::string& filePath) {
    // Find last occurrence of '/' or '\\' to handle both Unix and Windows paths
    size_t lastSlashIndex = filePath.find_last_of("/\\");

    // Extract substring starting right after the slash to the end
    std::string filename = (lastSlashIndex != std::string::npos)
        ? filePath.substr(lastSlashIndex + 1)
        : filePath;

    // Find last dot (.) to exclude extension
    size_t lastDotIndex = filename.find_last_of('.');

    // Extract substring up to the last dot if extension exists
    if (lastDotIndex != std::string::npos) {
        filename = filename.substr(0, lastDotIndex);
    }

    return filename;
}


std::string getFilename(const std::string& fullPath) {
    // Find the last occurrence of either '/' or '\\'
    size_t lastSlashPos = fullPath.find_last_of("/\\");

    if (lastSlashPos == std::string::npos) {
        // No slash found, the input is a filename itself
        return fullPath;
    }

    // Extract the filename from the full path
    return fullPath.substr(lastSlashPos + 1);
}

Node* Importer::ImportNode(std::string path) {

    Assimp::Importer importer;

    // Define import flags (e.g., to triangulate polygons)
    unsigned int flags = aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_GenSmoothNormals;



    // Load the scene from the file
    const aiScene* scene = importer.ReadFile(path, flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "Error: " << importer.GetErrorString() << std::endl;
        return nullptr;
    }

    NodeEntity* root = new NodeEntity;

    root->SetResourcePath(path);

    root->SetName(scene->mRootNode->mName.C_Str());

    root->SetName(extractFilenameAlone(path));

    std::string path_alone = getDirectoryPath(path);

    std::string mat_path = path_alone + "Material/";

    int b = 6;
    fs::create_directory(mat_path);


    std::vector<MaterialBase*> materials;

    for (int i = 0; i < scene->mNumMaterials; i++) {

        auto mat = scene->mMaterials[i];
        auto name = std::string(mat->GetName().C_Str());

        auto v_mat = new MaterialBase;

        std::string check = mat_path + name + ".material";

        auto lmat = Engine::FindActiveMaterial(check);

        if (lmat != nullptr) {
            materials.push_back(lmat);
            continue;
        }

        if (VFile::Exists(check.c_str())) {

            v_mat = MaterialBase::LoadMaterial(check);
            materials.push_back(v_mat);
            Engine::m_ActiveMaterials.push_back(v_mat);
            continue;

        }


        Engine::m_ActiveMaterials.push_back(v_mat);
        materials.push_back(v_mat);

        for (unsigned int j = 0; j < mat->GetTextureCount(aiTextureType_DIFFUSE); ++j) {
            aiString str;

            mat->GetTexture(aiTextureType_DIFFUSE, j, &str);
            if (VFile::Exists(std::string(path_alone + getFilename(std::string(str.C_Str()))).c_str()))
            {
                v_mat->SetDiffuse(new Texture2D(path_alone + getFilename(std::string(str.C_Str())),true));
            }
            else {
                auto raw_name = getFilename(str.C_Str());
                raw_name = getFileNameWithoutExtension(raw_name);
                auto res = Engine::FindResource(raw_name);
                if (res) {
                    v_mat->SetDiffuse(new Texture2D(res.value().c_str(),true));
                }

                int b = 5;
            }
            //GLuint textureID = LoadTextureFromFile(directory + '/' + str.C_Str());
            std::cout << "Loaded diffuse texture ID:  for texture " << str.C_Str() << std::endl;
        }

        // Load specular textures
        for (unsigned int j = 0; j < mat->GetTextureCount(aiTextureType_SPECULAR); ++j) {
            aiString str;
            mat->GetTexture(aiTextureType_SPECULAR, j, &str);
         //   v_mat->SetSpecular(new Texture2D(path_alone + std::string(str.C_Str())));
            if (VFile::Exists(std::string(path_alone + getFilename(std::string(str.C_Str()))).c_str()))
            {
                v_mat->SetMetal(new Texture2D(path_alone + getFilename(std::string(str.C_Str())),true));
            }
            else {
                auto raw_name = getFilename(str.C_Str());
                raw_name = getFileNameWithoutExtension(raw_name);
                auto res = Engine::FindResource(raw_name);
                if (res) {
                    v_mat->SetMetal(new Texture2D(res.value().c_str(),true));
                }

                int b = 5;
            }
            //GLuint textureID = LoadTextureFromFile(directory + '/' + str.C_Str());
            std::cout << "Loaded specular texture ID: for texture " << str.C_Str() << std::endl;
        }

        // Load normal textures (height maps in Assimp terminology)
        for (unsigned int j = 0; j < mat->GetTextureCount(aiTextureType_NORMALS); ++j) {
            aiString str;

            mat->GetTexture(aiTextureType_NORMALS, j, &str);
           // v_mat->SetNormals(new Texture2D(path_alone + std::string(str.C_Str())));
            if (VFile::Exists(std::string(path_alone + getFilename(std::string(str.C_Str()))).c_str()))
            {
                v_mat->SetNormals(new Texture2D(path_alone + getFilename(std::string(str.C_Str())),true));
            }
            else {
                auto raw_name = getFilename(str.C_Str());
                raw_name = getFileNameWithoutExtension(raw_name);
                auto res = Engine::FindResource(raw_name);
                if (res) {
                    v_mat->SetNormals(new Texture2D(res.value().c_str(),true));
                }

                int b = 5;
            }
            //GLuint textureID = LoadTextureFromFile(directory + '/' + str.C_Str());
            std::cout << "Loaded normal texture ID: for texture " << str.C_Str() << std::endl;
        }
        for (unsigned int j = 0; j < mat->GetTextureCount(aiTextureType_METALNESS); ++j) {
            aiString str;

            mat->GetTexture(aiTextureType_METALNESS, j, &str);
            //v_mat->SetMetal(new Texture2D(path_alone + std::string(str.C_Str())));
            if (VFile::Exists(std::string(path_alone + getFilename(std::string(str.C_Str()))).c_str()))
            {
                v_mat->SetMetal(new Texture2D(path_alone + getFilename(std::string(str.C_Str())),true));
            }
            else {
                auto raw_name = getFilename(str.C_Str());
                raw_name = getFileNameWithoutExtension(raw_name);
                auto res = Engine::FindResource(raw_name);
                if (res) {
                    v_mat->SetMetal(new Texture2D(res.value().c_str(),true));
                }

                int b = 5;
            }
            //GLuint textureID = LoadTextureFromFile(directory + '/' + str.C_Str());
            std::cout << "Loaded normal texture ID: for texture " << str.C_Str() << std::endl;
        }

        for (unsigned int j = 0; j < mat->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS); ++j) {
            aiString str;

            mat->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, j, &str);


            //v_mat->SetNormals(new Texture2D(path_alone + std::string(str.C_Str())));
            if (VFile::Exists(std::string(path_alone + getFilename(std::string(str.C_Str()))).c_str()))
            {
                v_mat->SetRough(new Texture2D(path_alone + getFilename(std::string(str.C_Str())),true));
            }
            else {
                auto raw_name = getFilename(str.C_Str());
                raw_name = getFileNameWithoutExtension(raw_name);
                auto res = Engine::FindResource(raw_name);
                if (res) {
                    v_mat->SetRough(new Texture2D(res.value().c_str(),true));
                }

                int b = 5;
            }
            //GLuint textureID = LoadTextureFromFile(directory + '/' + str.C_Str());
            std::cout << "Loaded normal texture ID: for texture " << str.C_Str() << std::endl;
        }

        int b = 5;
        v_mat->SaveMaterial(mat_path + name + ".material");

    }
    
    for (int i = 0; i < scene->mNumMeshes; i++) {

        Mesh3D* mesh = new Mesh3D;
        const aiMesh* aMesh = scene->mMeshes[i];

        for (int v = 0; v < aMesh->mNumVertices; v++) {

            auto aVertex = aMesh->mVertices[v];
            auto aTex = aMesh->mTextureCoords[0][v];
            auto aNorm = aMesh->mNormals[v];
            auto aBiNorm = aMesh->mBitangents[v];
            auto aTan = aMesh->mTangents[v];

            Vertex vertex;
            vertex.position = float3(-aVertex.x, aVertex.z, aVertex.y);
            vertex.color = float4(1, 1, 1, 1);
            vertex.texture = float3(aTex.x, 1.0-aTex.y, aTex.z);
            vertex.normal = float3(-aNorm.x, aNorm.z, aNorm.y);
            vertex.binormal = float3(-aBiNorm.x, aBiNorm.z, aBiNorm.y);
            vertex.tangent = float3(-aTan.x, aTan.z, aTan.y);

            mesh->AddVertex(vertex,true);

        }

        for (int t = 0; t < aMesh->mNumFaces; t++) {

            auto face = aMesh->mFaces[t];

            Triangle tri;

            tri.v0 = face.mIndices[0];
            tri.v1 = face.mIndices[1];
            tri.v2 = face.mIndices[2];

            mesh->AddTri(tri);

        }

        mesh->Build();
        
        mesh->SetMaterial(materials[aMesh->mMaterialIndex]);
        
      //  mesh->SetDepthMaterial(new MaterialDepth);


        root->AddMesh(mesh);
        //mesh->BuildGeo();


    }

//    root->SaveFastNode(path + ".ml");


    return (Node*)root;


}


Node* Importer::ImportAnims(NodeActor* load,std::string path) {

    Assimp::Importer importer;

    // Define import flags (e.g., to triangulate polygons)
    unsigned int flags = aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_GenNormals;

    // Load the scene from the file
    const aiScene* scene = importer.ReadFile(path, flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "Error: " << importer.GetErrorString() << std::endl;
        return nullptr;
    }

    NodeActor* res = load;
  //  auto anim = new Animation((aiScene*)scene, scene->mAnimations[0], res);
  //  auto animator = new Animator(anim);

    for (int i = 0; i < scene->mNumAnimations; i++) {

        auto new_anim = new Animation((aiScene*)scene, scene->mAnimations[i], res);
        res->GetAnimator()->AddAnimation(new_anim);

    }
    res->AddAnimFile(path);

//    res->GetAnimator()->ClearCopies();

    //res->SetAnimator(animator);
    return res;

}

Node* Importer::ImportActor(std::string path) {


    Assimp::Importer importer;

    // Define import flags (e.g., to triangulate polygons)
    unsigned int flags = aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_GenNormals;

    // Load the scene from the file
    const aiScene* scene = importer.ReadFile(path, flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "Error: " << importer.GetErrorString() << std::endl;
        return nullptr;
    }

    NodeActor* result = new NodeActor;

    result->SetResourcePath(path);

    std::string path_alone = getDirectoryPath(path);

    std::string mat_path = path_alone + "Material/";

    int b = 6;
    fs::create_directory(mat_path);


    std::vector<MaterialBase*> materials;

    for (int i = 0; i < scene->mNumMaterials; i++) {

        auto mat = scene->mMaterials[i];
        auto name = std::string(mat->GetName().C_Str());

        auto v_mat = new MaterialActorLight;

        std::string check = mat_path + name + ".material";

        auto lmat = Engine::FindActiveMaterial(check);

        if (lmat != nullptr) {
       //     materials.push_back(lmat);
       //     continue;
        }

        if (VFile::Exists(check.c_str())) {

         //   auto r = v_mat->LoadMaterial(check);
            
            //v_mat->SetDiffuse(r->GetDiffuse());
           // v_mat->SetNormals(r->GetNormal());
          //  v_mat->SetSpecular(r->GetSpecular());

          //  materials.push_back(v_mat);
           // Engine::m_ActiveMaterials.push_back(v_mat);
          //  continue;

        }

        //Engine::m_ActiveMaterials.push_back(v_mat);
        materials.push_back(v_mat);


        for (unsigned int j = 0; j < mat->GetTextureCount(aiTextureType_DIFFUSE); ++j) {
            aiString str;
           
            mat->GetTexture(aiTextureType_DIFFUSE, j, &str);
            str = ExtractFilename(str.C_Str()).c_str();
            v_mat->SetDiffuse(new Texture2D(path_alone + getFilename(std::string(str.C_Str())),true));
            //GLuint textureID = LoadTextureFromFile(directory + '/' + str.C_Str());
            std::cout << "Loaded diffuse texture ID:  for texture " << str.C_Str() << std::endl;
        }

        // Load specular textures
        for (unsigned int j = 0; j < mat->GetTextureCount(aiTextureType_SPECULAR); ++j) {
            aiString str;
            mat->GetTexture(aiTextureType_SPECULAR, j, &str);
            str = ExtractFilename(str.C_Str()).c_str();
            v_mat->SetSpecular(new Texture2D(path_alone + std::string(str.C_Str()),true));
            //GLuint textureID = LoadTextureFromFile(directory + '/' + str.C_Str());
            std::cout << "Loaded specular texture ID: for texture " << str.C_Str() << std::endl;
        }

        // Load normal textures (height maps in Assimp terminology)
        for (unsigned int j = 0; j < mat->GetTextureCount(aiTextureType_NORMALS); ++j) {
            aiString str;

            mat->GetTexture(aiTextureType_NORMALS, j, &str);
            str = ExtractFilename(str.C_Str()).c_str();
            v_mat->SetNormals(new Texture2D(path_alone + std::string(str.C_Str()),true));
            //GLuint textureID = LoadTextureFromFile(directory + '/' + str.C_Str());
            std::cout << "Loaded normal texture ID: for texture " << str.C_Str() << std::endl;
        }

        int b = 5;
        v_mat->SaveMaterial(mat_path + name + ".material");

    }


    for (int i = 0; i < scene->mNumMeshes;i++) {

        //Mesh3D* mesh = new Mesh3D((NodeEntity*)result);
        auto mesh = result->ProcessMesh(scene->mMeshes[i],(aiScene*)scene,false);

        mesh->Build();


        mesh->SetMaterial(materials[scene->mMeshes[i]->mMaterialIndex]);

        mesh->SetDepthMaterial(new MaterialActorDepth);

    }


    auto anim = new Animation((aiScene*)scene, scene->mAnimations[0], result);
    auto animator = new Animator(anim);
    
    for (int i = 1; i < scene->mNumAnimations; i++) {

        auto new_anim = new Animation((aiScene*)scene, scene->mAnimations[i], result);

    }
    
    result->SetAnimator(animator);


    auto input = path + ".anims";

    if (VFile::Exists(input.c_str())) {


        result->LoadAnimList();


    }
    result->LoadAnimNames();



    return result;
}