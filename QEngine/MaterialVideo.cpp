#include "MaterialVideo.h"
#include "QEngine.h"
#include <Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <Graphics/GraphicsEngine/interface/DeviceContext.h>
#include <Graphics/GraphicsEngine/interface/Buffer.h>
#include <Graphics/GraphicsEngine/interface/SwapChain.h>
#include <Common/interface/RefCntAutoPtr.hpp>
#include <MapHelper.hpp> // Add this line
#include "Texture2D.h"
#include "PSOBuilder.h"

using namespace Diligent;

MaterialVideo::MaterialVideo() {

    m_UniformBuffer = CreateUniform(sizeof(float4x4), "Basic Uniform Buffer - MVP");

    {
        DepthStencilStateDesc DSDesc;
        DSDesc.DepthEnable = true;
        DSDesc.DepthFunc = COMPARISON_FUNC_LESS_EQUAL;
        DSDesc.DepthWriteEnable = true;

        PSOBuilder builder;
        m_Pipeline = builder
            .SetName("Material Video")
            .WithShaders(Q3D::Engine::QEngine::GetDevice(), Q3D::Engine::QEngine::GetShaderFactory(), "Engine/Shader/MaterialVideo/video.vsh", "Engine/Shader/MaterialVideo/video.psh")
            .WithLayout(VertexLayoutType::Normal2D)
            .WithResourceLayout(LayoutResourceType::MaterialYUV)
            .DefaultsForTransparent()
            .WithDepthStencilState(DSDesc)
            .WithNumRenderTargets(1)
            .WithRTVFormat(0, Q3D::Engine::QEngine::GetSwapChain()->GetDesc().ColorBufferFormat)
            .WithDSVFormat(Q3D::Engine::QEngine::GetSwapChain()->GetDesc().DepthBufferFormat)
            .Build(Q3D::Engine::QEngine::GetDevice());

        m_Pipeline->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Constants")->Set(m_UniformBuffer);
        //        m_Pipeline->GetStaticVariableByName(SHADER_TYPE_PIXEL, "Constants")->Set(m_UniformBuffer);
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
            .SetName("Material 2D")
            .WithShaders(Q3D::Engine::QEngine::GetDevice(), Q3D::Engine::QEngine::GetShaderFactory(), "Engine/Shader/Material2D/2d.vsh", "Engine/Shader/Material2D/2d.psh")
            .WithLayout(VertexLayoutType::Normal2D)
            .WithResourceLayout(LayoutResourceType::MaterialYUV)
            .DefaultsForTransparent()
            .WithDepthStencilState(DSDesc_Add)
            .WithBlendState(BSDesc_Add)

            .WithNumRenderTargets(1)
            .WithRTVFormat(0, Q3D::Engine::QEngine::GetSwapChain()->GetDesc().ColorBufferFormat)
            .WithDSVFormat(Q3D::Engine::QEngine::GetSwapChain()->GetDesc().DepthBufferFormat)
            .Build(Q3D::Engine::QEngine::GetDevice());

        m_PipelineAdd->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Constants")->Set(m_UniformBuffer);
        //      m_PipelineAdd->GetStaticVariableByName(SHADER_TYPE_PIXEL, "Constants")->Set(m_UniformBuffer);
        m_PipelineAdd->CreateShaderResourceBinding(&m_SRBAdd, true);
    }

    return;


}

void MaterialVideo::Bind(bool add) {

    if (!add) {
        m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_YTexture")->Set(m_Y->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
        m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_UTexture")->Set(m_U->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
        m_SRB->GetVariableByName(SHADER_TYPE_PIXEL, "v_VTexture")->Set(m_V->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
        //Engine::m_pImmediateContext->MapBuffer(BasicUniform, MAP_TYPE::MAP_WRITE, MAP_FLAGS::MAP_FLAG_DISCARD);
        {
            MapHelper<glm::mat4> map_data(Q3D::Engine::QEngine::GetContext(), m_UniformBuffer, MAP_WRITE, MAP_FLAG_DISCARD);
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

            map_data[0] = glm::transpose(mvp);
        }
        //map_data.Unmap();


        Uint64 offsets = 0;

        IBuffer* pBuffs[] = { m_Buffers[0] };

        RESOURCE_STATE_TRANSITION_MODE flags = RESOURCE_STATE_TRANSITION_MODE::RESOURCE_STATE_TRANSITION_MODE_TRANSITION;

        //     return;
        Q3D::Engine::QEngine::GetContext()->SetVertexBuffers(0, 1, pBuffs, &offsets, flags);
        Q3D::Engine::QEngine::GetContext()->SetIndexBuffer(m_Buffers[1], 0, flags);


        //   return;


        Q3D::Engine::QEngine::GetContext()->SetPipelineState(m_Pipeline);

        Q3D::Engine::QEngine::GetContext()->CommitShaderResources(m_SRB, flags);
        //Vivid::m_pImmediateContext->SetPipelineState(m_Pipeline);
    }
    else {
        m_SRBAdd->GetVariableByName(SHADER_TYPE_PIXEL, "v_Texture")->Set(m_Textures[0]->GetView(), SET_SHADER_RESOURCE_FLAG_NONE);
        //Engine::m_pImmediateContext->MapBuffer(BasicUniform, MAP_TYPE::MAP_WRITE, MAP_FLAGS::MAP_FLAG_DISCARD);
        {
            MapHelper<glm::mat4> map_data(Q3D::Engine::QEngine::GetContext(), m_UniformBuffer, MAP_WRITE, MAP_FLAG_DISCARD);
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

            map_data[0] = glm::transpose(mvp);
        }
        //map_data.Unmap();


        Uint64 offsets = 0;

        IBuffer* pBuffs[] = { m_Buffers[0] };

        RESOURCE_STATE_TRANSITION_MODE flags = RESOURCE_STATE_TRANSITION_MODE::RESOURCE_STATE_TRANSITION_MODE_TRANSITION;

        //     return;
        Q3D::Engine::QEngine::GetContext()->SetVertexBuffers(0, 1, pBuffs, &offsets, flags);
        Q3D::Engine::QEngine::GetContext()->SetIndexBuffer(m_Buffers[1], 0, flags);


        //   return;


        Q3D::Engine::QEngine::GetContext()->SetPipelineState(m_PipelineAdd);

        Q3D::Engine::QEngine::GetContext()->CommitShaderResources(m_SRBAdd, flags);

    }

}

void MaterialVideo::Render() {



    DrawIndexedAttribs attrib;
    attrib.IndexType = VALUE_TYPE::VT_UINT32;

    attrib.NumIndices = m_IndexCount;

    attrib.Flags = DRAW_FLAG_VERIFY_ALL;
    Q3D::Engine::QEngine::GetContext()->DrawIndexed(attrib);

}