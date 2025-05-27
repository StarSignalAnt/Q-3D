#include "vertexCommon.hlsl"

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
 float4 v_LightSpec;
 float4 v_RenderProps;
 int4 v_LightMode;
 float4 v_LightDir;
 float4 v_LightCone;
 float4 v_MatDiff;
 float4 v_MatSpec;
 float4 v_CameraExt;

};

// Vertex shader takes two inputs: vertex position and uv coordinates.
// By convention, Diligent Engine expects vertex shader inputs to be 
// labeled 'ATTRIBn', where n is the attribute number.
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
 float3 TLP : NORMAL1;
 float3 TVP : NORMAL2;
 float3 TFP : NORMAL3;
 float3 v_LightPos : POSITION1;
 float3 v_CameraPos : POSITION2;
 float4 v_LightProp : POSITION3;
 float3 fragPos : POSITION4;
 float3 v_LightDiff : POSITION5;
 float3 v_LightSpec : POSITION6;
 float3 localNormal : NORMAL4;
 float3 v_LightCone : POSITION7;
 int4 v_LightMode : POSITION8;
 float3 v_LightDir : NORMAL5;
 float3 v_MatDiff : COLOR1;
 float3 v_MatSpec : COLOR2;
 float4 v_CameraExt : POSITION9;
};



// Note that if separate shader objects are not supported (this is only the case for old GLES3.0 devices), vertex
// shader output variable name must match exactly the name of the pixel shader input variable.
// If the variable has structure type (like in this example), the structure declarations must also be identical.
void main(in  VSInput VSIn,
          out PSInput PSIn) 
{
    float4 worldPosition = mul(float4(VSIn.Pos,1.0), v_Model);
   float4 viewPosition = mul(worldPosition, v_View);
   PSIn.Pos = mul(float4(VSIn.Pos, 1.0), v_MVP);

   float3 fragPos = mul(float4(VSIn.Pos,1), v_Model).xyz;

   //vec3 T = normalize(normalMatrix * vTan);
   //vec3 N = normalize(normalMatrix * vNorm);
   
   TBNLighting lighting = CalculateTBN(VSIn.Norm,VSIn.Tang,fragPos,v_Model,v_LightPos,v_CameraPos);
   
    PSIn.localNormal = lighting.localNormal;

    PSIn.TLP = lighting.TLP;
    PSIn.TVP = lighting.TVP;
    PSIn.TFP = lighting.TFP;

   //float4x4 mvp = g_Proj;
   //PSIn.Pos = mul(float4(VSIn.Pos, 1.0),transpose(mvp));
   PSIn.Norm = mul(VSIn.Norm, transpose((float3x3)v_ModelInv));
  // PSIn.Norm = float3(0, 0, 0);
   PSIn.Color = VSIn.Color;
   PSIn.Uv = VSIn.Uv;
   PSIn.v_LightPos = v_LightPos.xyz;
   PSIn.v_CameraPos = v_CameraPos.xyz;
   PSIn.v_LightProp = v_LightProp;
   PSIn.fragPos = fragPos;
   PSIn.v_LightDiff = v_LightDiff;
   PSIn.v_LightSpec = v_LightSpec;
   PSIn.v_LightMode = v_LightMode;
   PSIn.v_LightDir = v_LightDir.xyz;
   PSIn.v_LightCone = v_LightCone.xyz;
   PSIn.v_MatDiff = v_MatDiff.rgb;
   PSIn.v_MatSpec = v_MatSpec.rgb;
   PSIn.LayerCoord = VSIn.LayerCoord;
   PSIn.v_CameraExt = v_CameraExt;
}
