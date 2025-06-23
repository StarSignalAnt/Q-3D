cbuffer Constants
{
    float4x4 g_ModelMatrix;
    float4x4 g_ViewMatrix;
    float4x4 g_ProjectionMatrix;
    float4x4 g_MVPMatrix;
    float4x4 g_NormalMatrix;
    float4 g_CameraPosition;
    float4 g_LightPosition;
    float4 g_LightColor;
    float4 g_LightIntensity;
    float4 g_LightRange;
    float4 g_ToneMapParams;
    float4 g_AmbientColor;
    float4 g_Parallax;
};

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
    float3 Uv : TEXCOORD0;
    float4 v_Color : TEXCOORD1;
    float3 WorldPos : TEXCOORD2;
    float3 Normal : TEXCOORD3;
    float3 Tangent : TEXCOORD4;
    float3 Binormal : TEXCOORD5;
};

void main(in VSInput VSIn, out PSInput PSIn)
{
    // Transform position to world space
    float3 worldPos = mul(float4(VSIn.Pos, 1.0), g_ModelMatrix).xyz;
    
    // Extract the upper-left 3x3 matrix for normal transformations
    float3x3 normalMatrix = (float3x3)g_NormalMatrix;
    
    // Transform normal, tangent, and binormal to world space
    float3 normal = normalize(mul(VSIn.Norm, normalMatrix));
    float3 tangent = normalize(mul(VSIn.Tang, normalMatrix));
    float3 binormal = normalize(mul(VSIn.BiNorm, normalMatrix));
    
    // Ensure orthogonality of the TBN basis
    tangent = normalize(tangent - dot(tangent, normal) * normal);
    
    // Calculate binormal from cross product to ensure right-handed coordinate system
    float3 calculatedBinormal = cross(normal, tangent);
    float binormalSign = sign(dot(binormal, calculatedBinormal));
    binormal = calculatedBinormal * binormalSign;
    
    // Output values
    PSIn.Pos = mul(float4(VSIn.Pos, 1.0), g_MVPMatrix);
    PSIn.Uv = VSIn.Uv;
    PSIn.v_Color = VSIn.Color;
    PSIn.WorldPos = worldPos;
    PSIn.Normal = normal;
    PSIn.Tangent = tangent;
    PSIn.Binormal = binormal;
}