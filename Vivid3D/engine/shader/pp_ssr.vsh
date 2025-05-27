cbuffer Constants
{
   float4x4 g_MVP;
 float4x4 v_View;
 float4x4 v_Proj;
float4x4 v_InvProj;
float4x4 v_InvView;
 float4 v_Pars;
 int4 v_IPars;
 float4 v_ViewDir;
 float4 v_CamPos;
 

};

struct VSInput
{
     float3 Pos   : ATTRIB0;
    float4 Color : ATTRIB1;
    float3 Uv : ATTRIB2;
    float3 Norm : ATTRIB3;
    float3 BiNorm : ATTRIB4;
    float3 Tang : ATTRIB5;
    float4 m_BoneIds : ATTRIB6;
    float4 m_Weights : ATTRIB7;

};

struct PSInput 
{ 
 float4 Pos   : SV_POSITION;
 float3 Uv : TEX_COORD;
 float4 v_Color : COLOR2;
 float4x4 v_View: MATRIX1;
 float4x4 v_Proj : MATRIX5;
 float4x4 v_InvProj : MATRIX10;
 float4x4 v_InvView : MATRIX15;
   float4 v_Pars : TEXCOORD3;
  int4  v_IPars : TEXCOORD8;
  float4 v_ViewDir : VIEWDIR;
  float4 v_CamPos : CAMPOS;

};

void main(in  VSInput VSIn,
          out PSInput PSIn) 
{

   PSIn.Pos = mul(float4(VSIn.Pos, 1.0), g_MVP);
   PSIn.Uv = VSIn.Uv; 
   PSIn.v_Color = VSIn.Color;

    PSIn.v_View = v_View;
    PSIn.v_Proj = v_Proj;
    PSIn.v_InvProj = v_InvProj;
    PSIn.v_Pars = v_Pars;
    PSIn.v_IPars = v_IPars;
    PSIn.v_ViewDir = v_ViewDir;
    PSIn.v_CamPos = v_CamPos;
    PSIn.v_InvView = v_InvView;
}
