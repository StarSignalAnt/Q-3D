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
    float4x4 bones[100];
};

struct VSInput
{
    float3 Pos : ATTRIB0;
    float4 Color : ATTRIB1;
    float3 Uv : ATTRIB2;
    float3 Norm : ATTRIB3;
    float3 BiNorm : ATTRIB4;
    float3 Tang : ATTRIB5;
    float4 boneIndices : ATTRIB6;
    float4 boneWeights : ATTRIB7;
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
    float3 ViewDirTS : TEXCOORD6; // View direction in tangent space
};


float4x4 inverse(float4x4 m) {
    float n11 = m[0][0], n12 = m[1][0], n13 = m[2][0], n14 = m[3][0];
    float n21 = m[0][1], n22 = m[1][1], n23 = m[2][1], n24 = m[3][1];
    float n31 = m[0][2], n32 = m[1][2], n33 = m[2][2], n34 = m[3][2];
    float n41 = m[0][3], n42 = m[1][3], n43 = m[2][3], n44 = m[3][3];

    float t11 = n23 * n34 * n42 - n24 * n33 * n42 + n24 * n32 * n43 - n22 * n34 * n43 - n23 * n32 * n44 + n22 * n33 * n44;
    float t12 = n14 * n33 * n42 - n13 * n34 * n42 - n14 * n32 * n43 + n12 * n34 * n43 + n13 * n32 * n44 - n12 * n33 * n44;
    float t13 = n13 * n24 * n42 - n14 * n23 * n42 + n14 * n22 * n43 - n12 * n24 * n43 - n13 * n22 * n44 + n12 * n23 * n44;
    float t14 = n14 * n23 * n32 - n13 * n24 * n32 - n14 * n22 * n33 + n12 * n24 * n33 + n13 * n22 * n34 - n12 * n23 * n34;

    float det = n11 * t11 + n21 * t12 + n31 * t13 + n41 * t14;
    float idet = 1.0f / det;

    float4x4 ret;


    ret[0][0] = t11 * idet;
    ret[0][1] = (n24 * n33 * n41 - n23 * n34 * n41 - n24 * n31 * n43 + n21 * n34 * n43 + n23 * n31 * n44 - n21 * n33 * n44) * idet;
    ret[0][2] = (n22 * n34 * n41 - n24 * n32 * n41 + n24 * n31 * n42 - n21 * n34 * n42 - n22 * n31 * n44 + n21 * n32 * n44) * idet;
    ret[0][3] = (n23 * n32 * n41 - n22 * n33 * n41 - n23 * n31 * n42 + n21 * n33 * n42 + n22 * n31 * n43 - n21 * n32 * n43) * idet;

    ret[1][0] = t12 * idet;
    ret[1][1] = (n13 * n34 * n41 - n14 * n33 * n41 + n14 * n31 * n43 - n11 * n34 * n43 - n13 * n31 * n44 + n11 * n33 * n44) * idet;
    ret[1][2] = (n14 * n32 * n41 - n12 * n34 * n41 - n14 * n31 * n42 + n11 * n34 * n42 + n12 * n31 * n44 - n11 * n32 * n44) * idet;
    ret[1][3] = (n12 * n33 * n41 - n13 * n32 * n41 + n13 * n31 * n42 - n11 * n33 * n42 - n12 * n31 * n43 + n11 * n32 * n43) * idet;

    ret[2][0] = t13 * idet;
    ret[2][1] = (n14 * n23 * n41 - n13 * n24 * n41 - n14 * n21 * n43 + n11 * n24 * n43 + n13 * n21 * n44 - n11 * n23 * n44) * idet;
    ret[2][2] = (n12 * n24 * n41 - n14 * n22 * n41 + n14 * n21 * n42 - n11 * n24 * n42 - n12 * n21 * n44 + n11 * n22 * n44) * idet;
    ret[2][3] = (n13 * n22 * n41 - n12 * n23 * n41 - n13 * n21 * n42 + n11 * n23 * n42 + n12 * n21 * n43 - n11 * n22 * n43) * idet;

    ret[3][0] = t14 * idet;
    ret[3][1] = (n13 * n24 * n31 - n14 * n23 * n31 + n14 * n21 * n33 - n11 * n24 * n33 - n13 * n21 * n34 + n11 * n23 * n34) * idet;
    ret[3][2] = (n14 * n22 * n31 - n12 * n24 * n31 - n14 * n21 * n32 + n11 * n24 * n32 + n12 * n21 * n34 - n11 * n22 * n34) * idet;
    ret[3][3] = (n12 * n23 * n31 - n13 * n22 * n31 + n13 * n21 * n32 - n11 * n23 * n32 - n12 * n21 * n33 + n11 * n22 * n33) * idet;

    return ret;

}

#define IDENTITY_MATRIX float4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)
#define IDENTITY_MATRIX2 float4x4(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)

void main(in VSInput VSIn, out PSInput PSIn)
{

  float4x4 S = IDENTITY_MATRIX2;
    for (int i = 0; i < 4; ++i)
    {
        if ((int)VSIn.boneIndices[i] >= 0)
        {
            S += (bones[(int)VSIn.boneIndices[i]]* VSIn.boneWeights[i]);
        }
    }

    float3x3 S_ = transpose((float3x3)inverse(S));

    float4 a_pos =mul(float4(VSIn.Pos, 1.0),S);
    PSIn.Pos =mul(a_pos,g_MVPMatrix);


    float3 worldPos = mul(float4(a_pos.xyz, 1.0), g_ModelMatrix).xyz;
    float3 normal = normalize(mul(VSIn.Norm, (float3x3)g_NormalMatrix));
    float3 tangent = normalize(mul(VSIn.Tang, (float3x3)g_ModelMatrix));
    float3 binormal = normalize(mul(VSIn.BiNorm, (float3x3)g_ModelMatrix));

    float3 viewDirWS = normalize(g_CameraPosition.xyz - worldPos);
    float3x3 TBN = float3x3(tangent, binormal, normal);
    float3 viewDirTS = mul(viewDirWS, TBN);


//    PSIn.Pos = mul(float4(VSIn.Pos, 1.0), g_MVPMatrix);
    PSIn.Uv = VSIn.Uv;
    PSIn.v_Color = VSIn.Color;
    PSIn.WorldPos = worldPos;
    PSIn.Normal = normal;
    PSIn.Tangent = tangent;
    PSIn.Binormal = binormal;
    PSIn.ViewDirTS = viewDirTS;
}
