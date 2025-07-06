cbuffer Constants
{
    float4x4 g_MVP;
    float4x4 g_ModelMatrix;
    float4x4 g_ViewMatrix;
    float4x4 g_ProjectionMatrix;
    float4 m_CameraPos;
    float4 m_SunDir;
 float4 m_VolStart;
    float4 m_VolSize;
};

// Vertex shader takes two inputs: vertex position and uv coordinates.
// By convention, Diligent Engine expects vertex shader inputs to be 
// labeled 'ATTRIBn', where n is the attribute number.
struct VSInput
{
    float3 Pos : ATTRIB0;
    float4 Color : ATTRIB1;
    float3 Uv : ATTRIB2;
    float3 Norm : ATTRIB3;
    float3 BiNorm : ATTRIB4;
    float3 Tang : ATTRIB5;


};

struct PSInput
{
    float4 Pos : SV_POSITION;
    float3 Uv : TEX_COORD;
    float4 v_Color : COLOR2;
    float3 WorldPos    : TEXCOORD1;



};

// Note that if separate shader objects are not supported (this is only the case for old GLES3.0 devices), vertex
// shader output variable name must match exactly the name of the pixel shader input variable.
// If the variable has structure type (like in this example), the structure declarations must also be identical.
void main(in VSInput VSIn,
          out PSInput PSIn)
{
      PSIn.WorldPos = mul(float4(VSIn.Pos, 1.0), g_ModelMatrix).xyz;
   PSIn.Pos = mul(float4(VSIn.Pos, 1.0), g_MVP);
    PSIn.Uv = VSIn.Uv;
    PSIn.v_Color = VSIn.Color;
 PSIn.Pos.z = PSIn.Pos.w;
}