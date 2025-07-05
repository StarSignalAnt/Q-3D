cbuffer Constants
{
    float4x4 g_MVP;
    float4x4 g_ModelMatrix;
    float4x4 g_ViewMatrix;
    float4x4 g_ProjectionMatrix;
    float4 g_SunDir;
    float4 g_CameraPos;
    float4 g_AtmosRadius;
    float4 g_LightDir;
    float4 g_PlanetRadius;
    float4 g_RayLeigh;
    float4 g_Mie;
    float4 g_SunIntense;
float4 g_DaylightFactor;
float4 g_Time;
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
    float3 WorldPos : TEXCOORD1;


};

// Note that if separate shader objects are not supported (this is only the case for old GLES3.0 devices), vertex
// shader output variable name must match exactly the name of the pixel shader input variable.
// If the variable has structure type (like in this example), the structure declarations must also be identical.
void main(in VSInput VSIn,
          out PSInput PSIn)
{
    PSIn.WorldPos = VSIn.Pos;

    // Create a view matrix with only rotation
    float4x4 viewRotationOnly = g_ViewMatrix;
    viewRotationOnly[3] = float4(0, 0, 0, 1);

    // Correct the handedness of the projection matrix from right-handed (GLM) to left-handed (HLSL)
    float4x4 leftHandedProj = g_ProjectionMatrix;
    leftHandedProj[2].z *= -1.0;

    // --- NEW MATH ORDER ---
    // Calculate final position using column-major order: P * V * M * v
    float4 clipPos = mul(leftHandedProj, mul(viewRotationOnly, mul(g_ModelMatrix, float4(VSIn.Pos, 1.0))));

    // Use the .xyww swizzle trick to force depth to the far plane
    PSIn.Pos = clipPos.xyww;

    // Pass through other data
    PSIn.Uv = VSIn.Uv;
    PSIn.v_Color = VSIn.Color;
}