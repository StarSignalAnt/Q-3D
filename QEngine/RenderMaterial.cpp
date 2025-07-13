#include "RenderMaterial.h"
#include "QEngine.h"

void RenderMaterial::SetVertexShader(std::string path) {

    const char* filePath = path.c_str();

    // Allocate memory for the Diligent::Char* and copy the string
    size_t length = path.length();
    Char* diligentPath = new Char[length + 1]; // +1 for null terminator
    std::strcpy(diligentPath, filePath);

    ShaderDesc desc;

    desc.Name = diligentPath;
    desc.ShaderType = SHADER_TYPE_VERTEX;
    desc.UseCombinedTextureSamplers = true;


    ShaderCreateInfo info;
    info.FilePath = diligentPath;
    info.pShaderSourceStreamFactory = Q3D::Engine::QEngine::GetShaderFactory();
    info.Desc = desc;
    info.EntryPoint = "main";
    info.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;

    Q3D::Engine::QEngine::GetDevice()->CreateShader(info, &m_VS);




}

void RenderMaterial::SetPixelShader(std::string path) {

    const char* filePath = path.c_str();

    // Allocate memory for the Diligent::Char* and copy the string
    size_t length = path.length();
    Char* diligentPath = new Char[length + 1]; // +1 for null terminator
    std::strcpy(diligentPath, filePath);

    ShaderDesc desc;

    desc.Name = diligentPath;
    desc.ShaderType = SHADER_TYPE_PIXEL;
    desc.UseCombinedTextureSamplers = true;


    ShaderCreateInfo info;
    info.FilePath = diligentPath;
    info.pShaderSourceStreamFactory = Q3D::Engine::QEngine::GetShaderFactory();
    info.Desc = desc;
    info.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
    info.EntryPoint = "main";

    Q3D::Engine::QEngine::GetDevice()->CreateShader(info, &m_PS);


}

RefCntAutoPtr<IBuffer> RenderMaterial::CreateUniform(int size, std::string path) {

    BufferDesc desc;
    desc.Name = path.c_str();
    desc.Size = (Uint64)size;
    desc.Usage = USAGE_DYNAMIC;
    desc.BindFlags = BIND_UNIFORM_BUFFER;
    desc.CPUAccessFlags = CPU_ACCESS_WRITE;

    RefCntAutoPtr<IBuffer> buffer;

    Q3D::Engine::QEngine::GetDevice()->CreateBuffer(desc, nullptr, &buffer);


    return buffer;

}

void RenderMaterial::SetBuffer(RefCntAutoPtr<IBuffer> buffer,int index) {
    m_Buffers[index] = buffer;
}

void RenderMaterial::SetMatrix(glm::mat4 matrix, int index) {

    m_RenderMatrices[index] = matrix;

}


