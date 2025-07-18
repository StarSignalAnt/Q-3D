cbuffer Constants
{
    float4x4 g_MVP;

};

// Vertex shader takes two inputs: vertex position and uv coordinates.
// By convention, Diligent Engine expects vertex shader inputs to be 
// labeled 'ATTRIBn', where n is the attribute number.
struct VSInput
{
    float3 Pos : ATTRIB0;
    float4 Color : ATTRIB1;
    float3 Uv : ATTRIB2;
    float4 View : ATTRIB3;


};

struct PSInput
{
    float4 Pos : SV_POSITION;
    float3 Uv : TEX_COORD;
    float4 v_Color : COLOR2;
    float4 v_View : COLOR3;

};

// Note that if separate shader objects are not supported (this is only the case for old GLES3.0 devices), vertex
// shader output variable name must match exactly the name of the pixel shader input variable.
// If the variable has structure type (like in this example), the structure declarations must also be identical.
void main(in VSInput VSIn,
          out PSInput PSIn)
{

    PSIn.Pos = mul(float4(VSIn.Pos, 1.0), g_MVP);
    PSIn.Uv = VSIn.Uv;
    PSIn.v_View = VSIn.View;
    PSIn.v_Color = VSIn.Color;
 
}