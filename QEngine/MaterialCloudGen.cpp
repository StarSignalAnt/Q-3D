#include "MaterialCloudGen.h"
#include "MaterialCloudGen.h"
#include "QEngine.h"
#include "Texture3D.h"
#include <MapHelper.hpp>

struct CloudGenConstants {
    float g_Time;
    float g_Coverage;
    float padding1, padding2;
    uint32_t g_TexDimensions[4];
};

MaterialCloudGen::MaterialCloudGen() {
    ShaderCreateInfo ShaderCI;
    ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
    ShaderCI.pShaderSourceStreamFactory = QEngine::GetShaderFactory();
    ShaderVersion ShaderModel;
    ShaderModel.Major = 5;
    ShaderModel.Minor = 0;
    ShaderCI.HLSLVersion = ShaderModel;
    RefCntAutoPtr<IShader> pCS;
    ShaderCI.Desc.ShaderType = SHADER_TYPE_COMPUTE;
    ShaderCI.EntryPoint = "main";
    ShaderCI.Desc.Name = "Cloud Gen CS";
    ShaderCI.FilePath = "Engine/Compute/CloudGen/cloud_gen.csh";
    QEngine::GetDevice()->CreateShader(ShaderCI, &pCS);

    m_UniformBuffer = CreateUniform(sizeof(CloudGenConstants), "Cloud Gen Constants");

    PipelineResourceLayoutDesc ResourceLayout;
    ShaderResourceVariableDesc Variables[] =
    {
        // Tell the engine that g_OutputTexture is a DYNAMIC variable
        { SHADER_TYPE_COMPUTE, "g_OutputTexture", SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC }
    };
    ResourceLayout.Variables = Variables;
    ResourceLayout.NumVariables = _countof(Variables);

    ComputePipelineStateCreateInfo PSOCreateInfo;
    PSOCreateInfo.PSODesc.Name = "Cloud Gen PSO";
    PSOCreateInfo.PSODesc.ResourceLayout = ResourceLayout; // Assign the new layout here
    PSOCreateInfo.pCS = pCS;
    QEngine::GetDevice()->CreateComputePipelineState(PSOCreateInfo, &m_Pipeline);

    // This part remains the same
    m_Pipeline->GetStaticVariableByName(SHADER_TYPE_COMPUTE, "Constants")->Set(m_UniformBuffer);
    m_Pipeline->CreateShaderResourceBinding(&m_SRB, true);
}

void MaterialCloudGen::Dispatch(Texture3D* target, float time, float coverage) {
    m_SRB->GetVariableByName(SHADER_TYPE_COMPUTE, "g_OutputTexture")->Set(target->GetUAV());

    {
        MapHelper<CloudGenConstants> CBConstants(QEngine::GetContext(), m_UniformBuffer, MAP_WRITE, MAP_FLAG_DISCARD);
        CBConstants->g_Time = time;
        CBConstants->g_Coverage = coverage;
        CBConstants->g_TexDimensions[0] = target->GetWidth();
        CBConstants->g_TexDimensions[1] = target->GetHeight();
        CBConstants->g_TexDimensions[2] = target->GetDepth();
    }

    QEngine::GetContext()->SetPipelineState(m_Pipeline);
    QEngine::GetContext()->CommitShaderResources(m_SRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    // Tell the GPU how many thread groups to launch
    DispatchComputeAttribs DspAttribs;
    DspAttribs.ThreadGroupCountX = (target->GetWidth() + 7) / 8;
    DspAttribs.ThreadGroupCountY = (target->GetHeight() + 7) / 8;
    DspAttribs.ThreadGroupCountZ = (target->GetDepth() + 7) / 8;
    QEngine::GetContext()->DispatchCompute(DspAttribs);
}

void MaterialCloudGen::Bind(bool add) {

}

void MaterialCloudGen::Render() {

}