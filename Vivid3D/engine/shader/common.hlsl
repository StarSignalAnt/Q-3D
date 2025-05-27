
#define dx 1.0

Texture2D    v_Texture;
SamplerState v_Texture_sampler;
Texture2D v_TextureNorm;
SamplerState v_TextureNorm_sampler;
Texture2D v_TextureSpec;
SamplerState v_TextureSpec_sampler;
Texture2D v_TextureLayer;
SamplerState v_TextureLayer_sampler;
TextureCube v_Shadow;
SamplerState v_Shadow_sampler;


static const float3 sampleOffsetDirections[20] = {
   float3( dx,  dx,  dx), float3( dx, -dx,  dx), float3(-dx, -dx,  dx), float3(-dx,  dx,  dx), 
   float3( dx,  dx, -dx), float3( dx, -dx, -dx), float3(-dx, -dx, -dx), float3(-dx,  dx, -dx),
   float3( dx,  dx,  0), float3( dx, -dx,  0), float3(-dx, -dx,  0), float3(-dx,  dx,  0),
   float3( dx,  0,  dx), float3(-dx,  0,  dx), float3( dx,  0, -dx), float3(-dx,  0, -dx),
   float3( 0,  dx,  dx), float3( 0, -dx,  dx), float3( 0, -dx, -dx), float3( 0,  dx, -dx)
};

float3 light_GetNormal(float2 uv)
{

    float3 tNormal = float3(0.5, 0.0f, 1.0f);
                                                                                 
    tNormal = v_TextureNorm.Sample(v_TextureNorm_sampler, uv).rgb;

      tNormal = normalize(tNormal * 2.0 - 1.0);
    tNormal.y = -tNormal.y;

    return tNormal;

}

float light_GetDist(float3 lightPos,float3 fragPos,float lightRange)
{

  float xd = lightPos.x - fragPos.x;
    float yd = lightPos.y - fragPos.y;
    float zd = lightPos.z - fragPos.z;


    float dis = sqrt(xd * xd + yd * yd + zd * zd);

    if (dis < 0) {
        dis =0;
    }

    float dv = dis / lightRange;

    if (dv > 1.0) {
        dv = 1.0;
    }
    dv = 1.0 - dv;


    return dv;


}

float light_GetPointDiff(float3 TLP,float3 TFP,float3 TVP,float3 tNormal,float3 lightPos,float3 fragPos)
{


    float3 lightDir = normalize(TLP - TFP);
    return max(dot(lightDir, tNormal),0.0);


}

float light_GetPointSpec(float3 TLP, float3 TFP, float3 TVP, float3 tNormal, float shininess = 64.0f) {
    // Calculate the direction vectors
    float3 lightDir = normalize(TLP - TFP);  // Light direction from fragment to light
    float3 viewDir = normalize(TVP - TFP);   // View direction from fragment to viewer

    // Calculate the halfway vector between the light direction and view direction
    float3 halfwayDir = normalize(lightDir + viewDir);

    // Compute the dot product between the normal and the halfway vector
    float specAngle = max(dot(tNormal, halfwayDir), 0.0);

    // Compute the specular component using the shininess exponent
    return pow(specAngle, shininess)*2;
}


float light_GetDirectional(float3 lightDir,float3 TLP,float3 TFP,float3 tNormal){

    float3 lDir = normalize(lightDir);
    return max(dot(lDir, tNormal),0.0);

}

float light_GetSpotFalloff(float3 lightPos,float3 lightDir,float3 fragPos,float lightCone)
{
      float3 lightPixel = normalize(fragPos - lightPos);
      float factor = dot(lightPixel,-lightDir);
       factor *= pow(max(factor,0.0f),lightCone);
    return factor;
}

float light_GetShadow(float3 fragPos,float3 lightPos,float lightRange){

    float3 fragToLight = fragPos - lightPos;
    float currentDepth = abs(distance(fragPos,lightPos));

    int samples = 20;
    float diskRadius = 0.0025f;
    float bias = 0.005;
    float shadowVal = 0;
    currentDepth = currentDepth / lightRange;

    for(int i=0;i<samples;i++){

        float closestDepth = v_Shadow.Sample(v_Shadow_sampler,normalize(fragToLight)+sampleOffsetDirections[i] * diskRadius).r;


        if( (currentDepth-0.005)>closestDepth)
        {

         shadowVal +=1.0f;
        

        }
    
    }

    shadowVal = shadowVal / (float)samples;
   
 //   shadowVal = shadowVal * v_Shadow.Sample(v_Shadow_sampler,normalize(fragToLight)).g;
   


    shadowVal = 1.0 - shadowVal;
    
    //shadowVal =shadowVal * v_Shadow.Sample(v_Shadow_sampler,normalize(fragToLight)).g;

    

    return shadowVal;

}




struct LightValues{

    float light;
    float spec;
    float shadow;

};

LightValues CalculateLighting(int LightMode,float3 tNormal,float3 TLP,float3 TFP,float3 TVP,float3 LightPos,float3 FragPos,float3 LightDir,float3 LightCone,float LocalNormal)
{

    LightValues info;
   

  if(LightMode==0){
  

        info.light = light_GetPointDiff(TLP,TFP,TVP,tNormal,LightPos,FragPos);
        info.spec = light_GetPointSpec(TLP,TFP,TVP,tNormal);


    }

    
    if(LightMode==1)
    {


         info.light = light_GetPointDiff(TLP,TFP,TVP,tNormal,LightPos,FragPos);
         info.spec = light_GetPointSpec(TLP,TFP,TVP,tNormal); 
         float ff = light_GetSpotFalloff(LightPos,LightDir,FragPos,LightCone.x);
        
         info.light = info.light * ff;
         info.spec = info.spec * ff;

    };

    //Directional
  
   if(LightMode==2)
    {
        
        info.light = light_GetDirectional(LightDir,TLP,TFP,LocalNormal);
        info.spec = light_GetPointSpec(TLP,TFP,TVP,tNormal); 



    }

    info.spec = info.spec * 8.0f;

    return info;

}


// Function to calculate the normalized direction from the fragment to the light source
float3 CalculateLightDirection(float3 FragPosition, float3 LightPosition)
{
    return normalize(LightPosition - FragPosition);
}

// Function to calculate the diffuse lighting component
float CalculateDiffuseLight(float3 Normal, float3 LightDirection)
{
    return max(0.0, dot(Normal, LightDirection));
}

// Function to calculate the specular lighting component
float CalculateSpecularLight(float3 Normal, float3 LightDirection, float3 ViewPosition, float Shininess,float3 FragPosition)
{
    float3 ReflectDirection = reflect(-LightDirection, Normal);
    float specular = pow(max(0.0, dot(ReflectDirection, normalize(ViewPosition - FragPosition))), Shininess);
    return specular;
}

// Function to calculate the final lighting value
float3 CalculatePointLighting(float3 ViewPosition, float3 FragPosition, float3 Normal, float3 LightPosition, float3 LightColor, float3 MaterialDiffuse, float3 MaterialSpecular, float Shininess, float3 LightSpecular,float3 UV)
{
    float3 LightDirection = CalculateLightDirection(FragPosition, LightPosition);
    float Diffuse = CalculateDiffuseLight(Normal, LightDirection);
    float Specular = light_GetPointSpec(LightPosition,FragPosition,ViewPosition,Normal);//  CalculateSpecularLight(Normal, LightDirection, ViewPosition, Shininess,FragPosition);
    
    float3 spec_tex =v_TextureSpec.Sample(v_TextureSpec_sampler,UV).rgb;

     float3 SpecularLighting = (Specular * LightSpecular)*spec_tex;
     float3 DiffuseLighting = Diffuse *  LightColor;
    float3 Lighting = DiffuseLighting + SpecularLighting;

    // Final lighting value
    //float3 Lighting = (Diffuse * MaterialDiffuse + Specular * MaterialSpecular) * LightColor * LightSpecular;//


    return Lighting;
}
