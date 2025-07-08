#include "MaterialPBR.h"
#include "QEngine.h"
#include <Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <Graphics/GraphicsEngine/interface/DeviceContext.h>
#include <Graphics/GraphicsEngine/interface/Buffer.h>
#include <Graphics/GraphicsEngine/interface/SwapChain.h>
#include <Common/interface/RefCntAutoPtr.hpp>
#include <MapHelper.hpp> // Add this line
#include "Texture2D.h"
#include "TextureCube.h"
#include "SceneGraph.h"
#include "LightComponent.h"
#include "GraphNode.h"
#include "RenderTargetCube.h"
#include "VFile.h"
#include "RenderTarget2D.h"
#include "PSOBuilder.h"

using namespace Diligent;

struct PBRConstant {

    glm::mat4 g_ModelMatrix;
    glm::mat4 g_ViewMatrix;
    glm::mat4 g_ProjectionMatrix;
    glm::mat4 g_MVPMatrix;
    glm::mat4 g_NormalMatrix;

    glm::vec4 g_CameraPosition;
    glm::vec4 g_LightPosition;    // Position for point/spot lights
    glm::vec4 g_LightDirection;   // Direction for directional/spot lights
    glm::vec4 g_LightColor;
    glm::vec4 g_LightIntensity;
    glm::vec4 g_LightRange;

    glm::vec4 g_ToneMapParams;  // x: Exposure, y: Gamma, z: AmbientStrength, w: EnvStrength
    glm::vec4 g_AmbientColor;   // Ambient light color (w component unused)
    glm::vec4 g_LightType;      // x: LightType (0=point, 1=directional, 2=spot), y-w: unused
    glm::vec4 g_SpotLightCone;  // x: inner cone angle (cos), y: outer cone angle (cos), z-w: unused
    glm::mat4 g_LightSpaceMatrix;
};


MaterialPBR::MaterialPBR(bool clone) {



}

MaterialPBR::MaterialPBR() {

    m_ColorTexture = new Texture2D("Engine/Maps/White.png");
	m_MetallicTexture = new Texture2D("Engine/Maps/White.png");
	m_RoughnessTexture = new Texture2D("Engine/Maps/White.png");
	m_NormalTexture = new Texture2D("Engine/Maps/Normal.png");
	m_EnvironmentMap = new TextureCube("Engine/Maps/blackcm.tex");
	m_HeightTexture = new Texture2D("Engine/Maps/Black.png");
	//m_BRDF = new Texture2D("Engine/Maps/BRDF.png");



    m_UniformBuffer = CreateUniform(sizeof(PBRConstant), "Basic Uniform Buffer - MVP");

    {
      
        DepthStencilStateDesc DSDesc;
        DSDesc.DepthEnable = true;
        DSDesc.DepthFunc = COMPARISON_FUNC_LESS_EQUAL;
        DSDesc.DepthWriteEnable = true; 

        PSOBuilder builder;
        m_Pipeline = builder
            .SetName("Material PBR")
            .WithShaders(QEngine::m_pDevice, QEngine::m_pShaderFactory, "Engine/Shader/PBR/pbr.vsh", "Engine/Shader/PBR/pbr.psh")
            .WithLayout(VertexLayoutType::Normal3D)
            .WithResourceLayout(LayoutResourceType::PBRMaterial)
            .DefaultsForTransparent() 
            .WithDepthStencilState(DSDesc)
            .WithNumRenderTargets(1)
            .WithRTVFormat(0, QEngine::m_pSwapChain->GetDesc().ColorBufferFormat)
            .WithDSVFormat(QEngine::m_pSwapChain->GetDesc().DepthBufferFormat)
            .Build(QEngine::m_pDevice);

        m_Pipeline->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Constants")->Set(m_UniformBuffer);
        m_Pipeline->GetStaticVariableByName(SHADER_TYPE_PIXEL, "Constants")->Set(m_UniformBuffer);
        m_Pipeline->CreateShaderResourceBinding(&m_SRB, true);
    }


    {
        DepthStencilStateDesc DSDesc_Add;
        DSDesc_Add.DepthEnable = true;
        DSDesc_Add.DepthFunc = COMPARISON_FUNC_EQUAL;
        DSDesc_Add.DepthWriteEnable = true;

        BlendStateDesc BSDesc_Add;
        BSDesc_Add.RenderTargets[0].BlendEnable = true;
        BSDesc_Add.RenderTargets[0].SrcBlend = BLEND_FACTOR_ONE;
        BSDesc_Add.RenderTargets[0].DestBlend = BLEND_FACTOR_ONE;

        PSOBuilder builder;
        m_PipelineAdd = builder
            .SetName("Material PBR (Additive)")
            .WithShaders(QEngine::m_pDevice, QEngine::m_pShaderFactory, "Engine/Shader/PBR/pbr.vsh", "Engine/Shader/PBR/pbr.psh")
            .WithLayout(VertexLayoutType::Normal3D)
            .WithResourceLayout(LayoutResourceType::PBRMaterial)
            .WithDepthStencilState(DSDesc_Add)
            .WithBlendState(BSDesc_Add)
            .WithNumRenderTargets(1) 
            .WithRTVFormat(0, QEngine::m_pSwapChain->GetDesc().ColorBufferFormat)
            .WithDSVFormat(QEngine::m_pSwapChain->GetDesc().DepthBufferFormat)
            .Build(QEngine::m_pDevice);

        m_PipelineAdd->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Constants")->Set(m_UniformBuffer);
        m_PipelineAdd->GetStaticVariableByName(SHADER_TYPE_PIXEL, "Constants")->Set(m_UniformBuffer);
        m_PipelineAdd->CreateShaderResourceBinding(&m_SRBAdd, true);
    }

}

void MaterialPBR::Bind(bool add) {

    //auto l1 = SceneGraph::m_CurrentGraph->GetLights()[0];

	//auto lc = l1->GetComponent<LightComponent>();

    if (add) {

        auto l1 = m_Light;
        auto lc = l1->GetComponent<LightComponent>();

        m_SRBAdd->GetVariableByName(SHADER_TYPE_PIXEL, "v_Texture")->Set(m_ColorTexture->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
        m_SRBAdd->GetVariableByName(SHADER_TYPE_PIXEL, "v_TextureNormal")->Set(m_NormalTexture->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
        m_SRBAdd->GetVariableByName(SHADER_TYPE_PIXEL, "v_TextureMetal")->Set(m_MetallicTexture->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
        m_SRBAdd->GetVariableByName(SHADER_TYPE_PIXEL, "v_TextureRough")->Set(m_RoughnessTexture->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
        m_SRBAdd->GetVariableByName(SHADER_TYPE_PIXEL, "v_TextureEnvironment")->Set(m_EnvironmentMap->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
        if (lc->GetLightType() == LightType::Directional) {
            m_SRBAdd->GetVariableByName(SHADER_TYPE_PIXEL, "v_TextureDirShadow")->Set(m_Light->GetComponent<LightComponent>()->GetDirectionalShadowMap()->GetDepthView(), SET_SHADER_RESOURCE_FLAG_NONE);

        }
        else {
            m_SRBAdd->GetVariableByName(SHADER_TYPE_PIXEL, "v_TextureShadow")->Set(m_Light->GetComponent<LightComponent>()->GetShadowMap()->GetTexView(), SET_SHADER_RESOURCE_FLAG_NONE);
            //m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_TextureHeight")->Set(m_HeightTexture->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
        }


        //Engine::m_pImmediateContext->MapBuffer(BasicUniform, MAP_TYPE::MAP_WRITE, MAP_FLAGS::MAP_FLAG_DISCARD);
        {
            MapHelper<PBRConstant> map_data(QEngine::m_pImmediateContext, m_UniformBuffer, MAP_WRITE, MAP_FLAG_DISCARD);
            float FOVRadians = 45.0f * (3.14159265358979323846f / 180.0f);



            //float4x4 mvp = Engine::m_Camera->GetProjection(); //float4x4::Projection(FOVRadians, 1024.0f / 760.0f,0.01,1001,false);


            //float4x4 view = Engine::m_Camera->GetWorldMatrix();  //float4x4::Translation(float3(0,1.0f,-5)).Inverse();

           // float4x4 model = Engine::m_Node->GetWorldMatrix();

            //float4x4 id = float4x4::Identity().Inverse();

            //mvp = mvp*id;

            //mvp.Transpose();


            //mvp = model * view * mvp;

            glm::mat4 proj = m_RenderMatrices[2];
            glm::mat4 view = m_RenderMatrices[0];
            glm::mat4 model = m_RenderMatrices[1];

            glm::mat4 mvp = proj * view * model;

            //    map_data[0].g_MVPMatrix = glm::transpose(mvp);
            //	map_data[0].g_ModelMatrix = glm::transpose(model);
            map_data[0].g_ProjectionMatrix = glm::transpose(proj);
            map_data[0].g_ViewMatrix = glm::transpose(view);
            //    map_data[0].g_NormalMatrix = glm::transpose(glm::inverse(model)); // Inverse transpose for normals
            map_data[0].g_CameraPosition = glm::vec4(m_CameraPosition, 1.0f);
            auto mat = l1->GetWorldMatrix();
            glm::vec3 position = glm::vec3(mat[3]);


            map_data[0].g_LightPosition = glm::vec4(position, 1.0f); // Li // Light position
            map_data[0].g_LightColor = glm::vec4(lc->GetColor(), 1.0f); // White light color
            map_data[0].g_LightIntensity = glm::vec4(lc->GetIntensity(), 1.0f, 1.0f, 1.0f); // Was 10000!

            glm::mat3 normalMatrix = glm::inverse(glm::mat3(model));
            // Convert to 4x4 for the shader
            glm::mat4 normalMatrix4x4 = glm::mat4(normalMatrix);

            // Also try removing transposes (test one at a time):
            map_data[0].g_MVPMatrix = glm::transpose(mvp); // Remove transpose
            map_data[0].g_ModelMatrix = glm::transpose(model); // Remove transpose 
            map_data[0].g_NormalMatrix = normalMatrix4x4; // Remove transpose
            map_data[0].g_LightRange = glm::vec4(lc->GetRange(), 100.f, 100.f, 1.f); // Light range
            map_data[0].g_ToneMapParams = glm::vec4(1.0f, 2.2f, 0.05f, 1.0f); // Tone mapping parameters (example values)
            map_data[0].g_AmbientColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); // Ambient color
			map_data[0].g_LightType = glm::vec4((int)lc->GetLightType(), 0.0f, 0.0f, 0.0f); // Light type (0=point, 1=directional, 2=spot)
            map_data[0].g_LightDirection = glm::vec4(m_Light->TransformVector(glm::vec3(0, 0, 1)), 1.0f);
            map_data[0].g_SpotLightCone = glm::vec4(0.94f, 0.707, 0, 0);
            //	map_data[0].g_ParallaxParams = glm::vec4(m_ParalaxScale, 0.05f, 0.05f, 0.05f); // Parallax parameters (example values)
        }





        //map_data.Unmap();


        Uint64 offsets = 0;

        IBuffer* pBuffs[] = { m_Buffers[0] };

        RESOURCE_STATE_TRANSITION_MODE flags = RESOURCE_STATE_TRANSITION_MODE::RESOURCE_STATE_TRANSITION_MODE_TRANSITION;

        //     return;
        QEngine::m_pImmediateContext->SetVertexBuffers(0, 1, pBuffs, &offsets, flags);
        QEngine::m_pImmediateContext->SetIndexBuffer(m_Buffers[1], 0, flags);


        //   return;


        QEngine::m_pImmediateContext->SetPipelineState(m_PipelineAdd);

        QEngine::m_pImmediateContext->CommitShaderResources(m_SRBAdd, flags);
    }
    else {
        auto l1 = m_Light;
        auto lc = l1->GetComponent<LightComponent>();

        m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_Texture")->Set(m_ColorTexture->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
        m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_TextureNormal")->Set(m_NormalTexture->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
        m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_TextureMetal")->Set(m_MetallicTexture->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
        m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_TextureRough")->Set(m_RoughnessTexture->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
        m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_TextureEnvironment")->Set(m_EnvironmentMap->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
        if (lc->GetLightType() == LightType::Directional) {
            m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_TextureDirShadow")->Set(m_Light->GetComponent<LightComponent>()->GetDirectionalShadowMap()->GetDepthView(), SET_SHADER_RESOURCE_FLAG_NONE);

        }
        else {
            m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_TextureShadow")->Set(m_Light->GetComponent<LightComponent>()->GetShadowMap()->GetTexView(), SET_SHADER_RESOURCE_FLAG_NONE);
            //m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_TextureHeight")->Set(m_HeightTexture->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
        }


        //Engine::m_pImmediateContext->MapBuffer(BasicUniform, MAP_TYPE::MAP_WRITE, MAP_FLAGS::MAP_FLAG_DISCARD);
        {
            MapHelper<PBRConstant> map_data(QEngine::m_pImmediateContext, m_UniformBuffer, MAP_WRITE, MAP_FLAG_DISCARD);
            float FOVRadians = 45.0f * (3.14159265358979323846f / 180.0f);



            //float4x4 mvp = Engine::m_Camera->GetProjection(); //float4x4::Projection(FOVRadians, 1024.0f / 760.0f,0.01,1001,false);


            //float4x4 view = Engine::m_Camera->GetWorldMatrix();  //float4x4::Translation(float3(0,1.0f,-5)).Inverse();

           // float4x4 model = Engine::m_Node->GetWorldMatrix();

            //float4x4 id = float4x4::Identity().Inverse();

            //mvp = mvp*id;

            //mvp.Transpose();


            //mvp = model * view * mvp;

            glm::mat4 proj = m_RenderMatrices[2];
            glm::mat4 view = m_RenderMatrices[0];
            glm::mat4 model = m_RenderMatrices[1];

            glm::mat4 mvp = proj * view * model;

            //    map_data[0].g_MVPMatrix = glm::transpose(mvp);
            //	map_data[0].g_ModelMatrix = glm::transpose(model);
            map_data[0].g_ProjectionMatrix = glm::transpose(proj);
            map_data[0].g_ViewMatrix = glm::transpose(view);
            //    map_data[0].g_NormalMatrix = glm::transpose(glm::inverse(model)); // Inverse transpose for normals
            map_data[0].g_CameraPosition = glm::vec4(m_CameraPosition, 1.0f);

            auto mat = l1->GetWorldMatrix();
            glm::vec3 position = glm::vec3(mat[3]);


            map_data[0].g_LightPosition = glm::vec4(position, 1.0f); // Light position
            map_data[0].g_LightColor = glm::vec4(lc->GetColor(), 1.0f); // White light color
            map_data[0].g_LightIntensity = glm::vec4(lc->GetIntensity(), 1.0f, 1.0f, 1.0f); // Was 10000!

            glm::mat3 normalMatrix = glm::inverse(glm::mat3(model));
            // Convert to 4x4 for the shader
            glm::mat4 normalMatrix4x4 = glm::mat4(normalMatrix);
            // Also try removing transposes (test one at a time):
            map_data[0].g_MVPMatrix = glm::transpose(mvp); // Remove transpose
            map_data[0].g_ModelMatrix = glm::transpose(model); // Remove transpose 
            map_data[0].g_NormalMatrix = normalMatrix4x4; // Remove transpose
            map_data[0].g_LightRange = glm::vec4(lc->GetRange(), 100.f, 100.f, 1.f); // Light range
            map_data[0].g_ToneMapParams = glm::vec4(1.0f, 2.2f, 0.05f, 1.0f); // Tone mapping parameters (example values)
            map_data[0].g_AmbientColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); // Ambient color
            //map_data[0].g_ParallaxParams = glm::vec4(m_ParalaxScale, 0.05f, 0.05f, 0.05f); // Parallax parameters (example values)
            map_data[0].g_LightType = glm::vec4((int)lc->GetLightType(), 0.0f, 0.0f, 0.0f); // Light type (0=point, 1=directional, 2=spot)
            map_data[0].g_LightDirection = glm::vec4(-lc->GetDirection(), 1.0f);
            map_data[0].g_SpotLightCone = glm::vec4(0.94f, 0.707, 0, 0);
            map_data[0].g_LightSpaceMatrix = glm::transpose(lc->GetLightSpaceMatrix());

        }



        //map_data.Unmap();


        Uint64 offsets = 0;

        IBuffer* pBuffs[] = { m_Buffers[0] };

        RESOURCE_STATE_TRANSITION_MODE flags = RESOURCE_STATE_TRANSITION_MODE::RESOURCE_STATE_TRANSITION_MODE_TRANSITION;

        //     return;
        QEngine::m_pImmediateContext->SetVertexBuffers(0, 1, pBuffs, &offsets, flags);
        QEngine::m_pImmediateContext->SetIndexBuffer(m_Buffers[1], 0, flags);


        //   return;


        QEngine::m_pImmediateContext->SetPipelineState(m_Pipeline);

        QEngine::m_pImmediateContext->CommitShaderResources(m_SRB, flags);
        //Vivid::m_pImmediateContext->SetPipelineState(m_Pipeline);
    }

}

void MaterialPBR::Render() {

    DrawIndexedAttribs attrib;
    attrib.IndexType = VALUE_TYPE::VT_UINT32;

    attrib.NumIndices = m_IndexCount;

    attrib.Flags = DRAW_FLAG_VERIFY_ALL;
    QEngine::m_pImmediateContext->DrawIndexed(attrib);

}


bool HasExtension(const std::string& path, const std::string& extension) {
    // Find the last dot in the path
    size_t dotPos = path.find_last_of('.');
    if (dotPos == std::string::npos) return false; // No extension

    std::string fileExt = path.substr(dotPos + 1);

    // Normalize to lowercase for case-insensitive comparison
    std::string extLower = extension;
    std::string fileExtLower = fileExt;
    std::transform(extLower.begin(), extLower.end(), extLower.begin(), ::tolower);
    std::transform(fileExtLower.begin(), fileExtLower.end(), fileExtLower.begin(), ::tolower);

    return fileExtLower == extLower;
}


void MaterialPBR::Save(std::string path) {




    if (!HasExtension(path, "material"))
    {
        path = path + ".material";
    }
    VFile* f = new VFile(path.c_str(), FileMode::Write);

    f->WriteString(m_ColorTexture->GetPath().c_str());
    f->WriteString(m_NormalTexture->GetPath().c_str());
    f->WriteString(m_MetallicTexture->GetPath().c_str());
    f->WriteString(m_RoughnessTexture->GetPath().c_str());

    f->Close();
    m_Path = path;

}

void MaterialPBR::Load(std::string path) {

    if (!HasExtension(path, "material"))
    {
        path = path + ".material";
    }
    m_Path = path;
    VFile* f = new VFile(path.c_str(), FileMode::Read);

    m_ColorTexture = new Texture2D(f->ReadString());
    m_NormalTexture = new Texture2D(f->ReadString());
    m_MetallicTexture = new Texture2D(f->ReadString());
    m_RoughnessTexture = new Texture2D(f->ReadString());

    f->Close();
}