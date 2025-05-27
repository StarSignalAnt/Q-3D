
#include "structures.fxh"
#include "RayUtils.fxh"





Texture2D    g_CubeTextures[128];
SamplerState g_SamLinearWrap;

[shader("closesthit")]
void main(inout PrimaryRayPayload payload, in BuiltInTriangleIntersectionAttributes attr)
{
    // Calculate triangle barycentrics.
    float3 barycentrics = float3(1.0 - attr.barycentrics.x - attr.barycentrics.y, attr.barycentrics.x, attr.barycentrics.y);


    uint real_i = PrimitiveIndex()+DynamicOffsets[InstanceID()].y;

    // Get vertex indices for primitive.
      uint3 primitive = DynamicTriangles[real_i].Indices;




    // Calculate texture coordinates.
    float2 uv = DynamicVertices[primitive.x].texture.xy * barycentrics.x +
                DynamicVertices[primitive.y].texture.xy * barycentrics.y +
                DynamicVertices[primitive.z].texture.xy * barycentrics.z;

    // Calculate and transform triangle normal.
    float3 normal = DynamicVertices[primitive.x].normal.xyz * barycentrics.x +
                    DynamicVertices[primitive.y].normal.xyz * barycentrics.y +
                    DynamicVertices[primitive.z].normal.xyz * barycentrics.z;
    normal = normalize(mul( ObjectToWorld3x4(), normal));

     float3 tang = DynamicVertices[primitive.x].tangent.xyz * barycentrics.x +
                    DynamicVertices[primitive.y].tangent.xyz * barycentrics.y +
                    DynamicVertices[primitive.z].tangent.xyz * barycentrics.z;
    tang = normalize(mul((float3x3) ObjectToWorld3x4(), tang));
  

  float3 v0 = DynamicVertices[primitive.x].position;
  float3 v1 = DynamicVertices[primitive.y].position;
  float3 v2 = DynamicVertices[primitive.z].position;


float3 fragPos = barycentrics.x * v0 + barycentrics.y * v1 + barycentrics.z * v2;
float3 worldPos = mul(ObjectToWorld3x4(), float4(fragPos, 1.0));
float3 fragPosWorld = worldPos.xyz;




    float3 T = normalize(mul((float3x3) ObjectToWorld3x4(),tang));
    float3 N =normal;


    T = normalize(T - dot(T, N) * N);

    float3 B = cross(N, T);
    
    float3x3 TBN = transpose(float3x3(T, B, N));



    
  // Apply lighting.
    float3 rayOrigin = WorldRayOrigin() + WorldRayDirection() * RayTCurrent();
    LightingPass(payload.Color, rayOrigin, normal, payload.Recursion + 1);

    payload.Color = payload.Color*0.001;

    float3 tex_Col = g_CubeTextures[NonUniformResourceIndex(InstanceID()*4)].SampleLevel(g_SamLinearWrap, uv, 0).rgb;
    float3 tex_Norm = g_CubeTextures[NonUniformResourceIndex(InstanceID()*4+1)].SampleLevel(g_SamLinearWrap, uv, 0).rgb;
    float tex_Rough = g_CubeTextures[NonUniformResourceIndex(InstanceID()*4+2)].SampleLevel(g_SamLinearWrap, uv, 0).r;
    float tex_Metal = g_CubeTextures[NonUniformResourceIndex(InstanceID()*4+3)].SampleLevel(g_SamLinearWrap, uv, 0).r;    


    // Sample texturing. Ray tracing shaders don't support LOD calculation, so we must specify LOD and apply filtering.
    //payload.Color += g_CubeTextures[NonUniformResourceIndex(InstanceID())].SampleLevel(g_SamLinearWrap, uv, 0).rgb;
    payload.Depth = RayTCurrent();
    
    
    float3 tNormal = float3(0.5, 0.0f, 1.0f);

      tNormal = tex_Norm;

      tNormal = -1.0+tNormal * 2.0;
      tNormal.y =-tNormal.y;

    
    for(int l=0;l<g_ConstantsCB.Lightcount.x;l++){

        int index=l*4;

        float3 light_Pos = LightsBuffer[index].xyz;
        float3 light_Diff = LightsBuffer[index+1].xyz;
        float3 light_Spec = LightsBuffer[index+2].xyz;
        float light_Range = LightsBuffer[index+3].x;

        float3 TLP = mul(light_Pos, TBN);
        float3 TVP = mul(g_ConstantsCB.CamPos.xyz, TBN);
        float3 TFP = mul(fragPos, TBN);

         float shadow = 1.0;

      RayDesc ray;
      ray.Origin = rayOrigin + N * SMALL_OFFSET;
      ray.TMin   = 0.0;
      ray.TMax = distance(light_Pos, rayOrigin) * 1.01;
      float3 rayDir = normalize(light_Pos - rayOrigin);
      ray.Direction = rayDir;

      shadow = CastShadow(ray, 1).Shading;
   
      shadow = 1.0;

      float3 viewDir = normalize(g_ConstantsCB.CamPos.xyz - fragPosWorld);
      float3 lightDir = normalize(light_Pos -fragPosWorld);

      
      float3 normalFromMap = tNormal;// g_TextureNorm.Sample(g_TextureNorm_sampler, input.Uv).xyz * 2.0 - 1.0;
      normalFromMap = normalize(mul(TBN, normalFromMap));


      float3 lightCol = CalculateLighting(fragPosWorld,tex_Col, normalFromMap, viewDir, lightDir,light_Diff, 1.0,tex_Rough,tex_Metal,payload.Recursion);

      //lightCol=lightCol+float3(0.3,0.3,0.3);

        payload.Color += (lightCol)*shadow;

       



    }

  

}
