cbuffer Constants
{
    float4x4 v_MVP;
    float4x4 v_Proj;
    float4x4 v_Model;
    float4x4 v_View;
    float4x4 v_ModelInv;
    float4 v_CameraPos;
    float4 v_LightPos;
    float4 v_LightProp;
    float4 v_LightDiff;
    float4 v_CameraExt;
    int4 v_Layers;
};

struct VSInput
{
    float3 Pos   : ATTRIB0;
    float4 Color : ATTRIB1;
    float3 Uv : ATTRIB2;
    float3 Norm : ATTRIB3;
    float3 BiNorm : ATTRIB4;
    float3 Tang : ATTRIB5;
    float3 LayerCoord : ATTRIB6;
};

struct PSInput 
{ 
    float4 Pos   : SV_POSITION;
    float4 Color : COLOR0;
    float3 Norm : NORMAL0;
    float3 Uv : TEX_COORD;
    float3 LayerCoord : NORMAL6;
    float3 WorldPos : NORMAL1;
    float3 Tangent : NORMAL2;    // World space tangent
    float3 Bitangent : NORMAL3;  // World space bitangent
};

void main(in VSInput VSIn, out PSInput PSIn) 
{
    // Transform vertex position to world space
    float4 worldPosition = mul(float4(VSIn.Pos, 1.0), v_Model);
    PSIn.WorldPos = worldPosition.xyz;
    
    // Transform to clip space
    PSIn.Pos = mul(float4(VSIn.Pos, 1.0), v_MVP);
    
    // Transform normal, tangent, and bitangent to world space
    float3x3 normalMatrix = (float3x3)transpose(v_ModelInv);
    PSIn.Norm = normalize(mul(VSIn.Norm, normalMatrix));
    PSIn.Tangent = normalize(mul(VSIn.Tang, normalMatrix));
    PSIn.Bitangent = normalize(mul(VSIn.BiNorm, normalMatrix));
    
    // Pass through other attributes
    PSIn.Color = VSIn.Color;
    PSIn.Uv = VSIn.Uv;
    PSIn.LayerCoord = VSIn.LayerCoord;
}