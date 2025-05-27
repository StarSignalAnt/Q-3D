
#include "structures.fxh"
#include "RayUtils.fxh"





Texture2D    g_CubeTextures[128];
Texture2D    g_TerrainCol[8];
Texture2D    g_TerrainNorm[8];
Texture2D    g_TerrainSpec[8];
Texture2D    g_Layers[8];

SamplerState g_SamLinearWrap;

[shader("closesthit")]
void main(inout PrimaryRayPayload payload, in BuiltInTriangleIntersectionAttributes attr)
{
    // Calculate triangle barycentrics.
    float3 barycentrics = float3(1.0 - attr.barycentrics.x - attr.barycentrics.y, attr.barycentrics.x, attr.barycentrics.y);


    uint real_i = PrimitiveIndex();

    // Get vertex indices for primitive.
      uint3 primitive = TerrainTriangles[real_i].Indices;



        
    // Calculate texture coordinates.
    float2 layer_coord = TerrainVertices[primitive.x].layercoord.xy * barycentrics.x +
                TerrainVertices[primitive.y].layercoord.xy * barycentrics.y +
                TerrainVertices[primitive.z].layercoord.xy * barycentrics.z;
    // Calculate texture coordinates.
    float2 uv = TerrainVertices[primitive.x].texture.xy * barycentrics.x +
                TerrainVertices[primitive.y].texture.xy * barycentrics.y +
                TerrainVertices[primitive.z].texture.xy * barycentrics.z;

    // Calculate and transform triangle normal.
    float3 normal = TerrainVertices[primitive.x].normal.xyz * barycentrics.x +
                    TerrainVertices[primitive.y].normal.xyz * barycentrics.y +
                    TerrainVertices[primitive.z].normal.xyz * barycentrics.z;
    normal = normalize(mul( ObjectToWorld3x4(), normal));

     float3 tang = TerrainVertices[primitive.x].tangent.xyz * barycentrics.x +
                    TerrainVertices[primitive.y].tangent.xyz * barycentrics.y +
                    TerrainVertices[primitive.z].tangent.xyz * barycentrics.z;
    tang = normalize(mul((float3x3) ObjectToWorld3x4(), tang));
  

  float3 v0 = TerrainVertices[primitive.x].position;
  float3 v1 = TerrainVertices[primitive.y].position;
  float3 v2 = TerrainVertices[primitive.z].position;


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

   


    // Sample texturing. Ray tracing shaders don't support LOD calculation, so we must specify LOD and apply filtering.
    //payload.Color += g_CubeTextures[NonUniformResourceIndex(InstanceID())].SampleLevel(g_SamLinearWrap, uv, 0).rgb;
    payload.Depth = RayTCurrent();
    
  
  
      float3 tcol = float3(0,0,0);

      for(int i=0;i<g_ConstantsCB.TerrainInfo.x;i++)
      {

          float3 tex_Col = ReadTex(g_TerrainCol[NonUniformResourceIndex(InstanceID()+i)],g_SamLinearWrap, uv, fragPosWorld).rgb;
          float3 tex_Norm = ReadTex(g_TerrainNorm[NonUniformResourceIndex(InstanceID()+i)],g_SamLinearWrap, uv, fragPosWorld).rgb;
          float tex_Spec = ReadTex(g_TerrainSpec[NonUniformResourceIndex(InstanceID()+i)],g_SamLinearWrap, uv, fragPosWorld).r;
          float l_val =  ReadTex(g_Layers[NonUniformResourceIndex(InstanceID()+i)],g_SamLinearWrap,layer_coord, fragPosWorld).r;    
   float3 tNormal = float3(0.5, 0.0f, 1.0f);

      tNormal = tex_Norm;

      tNormal = -1.0+tNormal * 2.0;
      tNormal.y =-tNormal.y;
        float3 lightCol = float3(0,0,0);

           for(int l=0;l<g_ConstantsCB.Lightcount.x;l++){

        int index=l*4;

        float3 light_Pos = LightsBuffer[index].xyz;
        float3 light_Diff = LightsBuffer[index+1].xyz;
        float3 light_Spec = LightsBuffer[index+2].xyz;
        float light_Range = LightsBuffer[index+3].x;

           float light = light_GetDist(light_Pos,fragPosWorld,light_Range);

        float3 TLP = mul(light_Pos, TBN);
        float3 TVP = mul(g_ConstantsCB.CamPos.xyz, TBN);
        float3 TFP = mul(fragPos, TBN);

         float shadow = 1.0;

      RayDesc ray;
      ray.Origin = fragPosWorld + N * SMALL_OFFSET;
      ray.TMin   = 0.0;
      ray.TMax = distance(light_Pos, rayOrigin) * 1;
      float3 rayDir = normalize(light_Pos - fragPosWorld);
      ray.Direction = rayDir;


      shadow = CastShadow(ray, 1).Shading;
   
 

      float3 viewDir = normalize(g_ConstantsCB.CamPos.xyz - fragPosWorld);
      float3 lightDir = normalize(light_Pos -fragPosWorld);

      
      float3 normalFromMap = tNormal;// g_TextureNorm.Sample(g_TextureNorm_sampler, input.Uv).xyz * 2.0 - 1.0;
      normalFromMap = normalize(mul(TBN, normalFromMap));


       lightCol+= CalculateLighting(fragPosWorld,float3(1,1,1), normalFromMap, viewDir, lightDir,light_Diff, 1.0,0.25,0.1,payload.Recursion)*shadow*light;
           
           }




          tcol = tcol + tex_Col *l_val*lightCol;
         


      }


        payload.Color += (tcol);

      
    

  

}
