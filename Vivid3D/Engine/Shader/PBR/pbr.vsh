cbuffer Constants
{

       // === TRANSFORMATION MATRICES ===
    float4x4 g_ModelMatrix;        // World transform
    float4x4 g_ViewMatrix;         // Camera view
    float4x4 g_ProjectionMatrix;   // Projection
    float4x4 g_MVPMatrix;          // Combined Model-View-Projection
    float4x4 g_NormalMatrix;       // Inverse transpose of model matrix for normals

    // === CAMERA ===
    float4 g_CameraPosition;       // .xyz = world-space camera position, .w unused

    // === LIGHTING: Point Light ===
    float4 g_LightPosition;        // .xyz = position, .w unused
    float4 g_LightColor;           // .rgb = color, .a unused
    float4 g_LightIntensity;       // .x = intensity multiplier, .yzw unused
    float4 g_LightRange;           // .x = effective range, .yzw unused

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
    float3 WorldPos : WORLD_POS;    // Added: world position for lighting calculations
    float3 Normal : NORMAL;         // Added: world-space normal
    float3 Tangent : TANGENT;       // Added: world-space tangent
    float3 Binormal : BINORMAL;  

};

// Note that if separate shader objects are not supported (this is only the case for old GLES3.0 devices), vertex
// shader output variable name must match exactly the name of the pixel shader input variable.
// If the variable has structure type (like in this example), the structure declarations must also be identical.
void main(in VSInput VSIn,
          out PSInput PSIn)
{

     PSIn.Pos = mul(float4(VSIn.Pos, 1.0), g_MVPMatrix);
    PSIn.Uv = VSIn.Uv;
    PSIn.v_Color = VSIn.Color;
    
    // Added: Calculate world-space position for lighting
    PSIn.WorldPos = mul(float4(VSIn.Pos, 1.0), g_ModelMatrix).xyz;
    
    // Added: Transform normals, tangents, and binormals to world space
    PSIn.Normal = normalize(mul(VSIn.Norm, (float3x3)g_NormalMatrix));
    PSIn.Tangent = normalize(mul(VSIn.Tang, (float3x3)g_ModelMatrix));
    PSIn.Binormal = normalize(mul(VSIn.BiNorm, (float3x3)g_ModelMatrix));
}