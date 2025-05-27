
struct TBNLighting
{
    float3 T;
    float3 B;
    float3 N;
    float3 localNormal;
    float3 TLP;
    float3 TVP;
    float3 TFP;
    float3x3 TBN;
};

TBNLighting CalculateTBN(float3 Norm,float3 Tang,float3 FragPos,float4x4 Model,float3 LightPos,float3 CamPos){

    TBNLighting info;
 
    float3x3 normalMatrix = (float3x3)Model;

    info.localNormal = normalize(mul(Norm, normalMatrix));
   
    info.T = normalize(mul(Tang, normalMatrix));
    info.N = normalize(mul(Norm, normalMatrix));

    info.T = normalize(info.T - dot(info.T, info.N) * info.N);

    info.B = cross(info.N, info.T);

    info.TBN = transpose(float3x3(info.T, info.B, info.N));

    info.TLP = mul(LightPos.xyz, info.TBN);
    info.TVP = mul(CamPos.xyz, info.TBN);
    info.TFP = mul(FragPos, info.TBN);

    return info;

}