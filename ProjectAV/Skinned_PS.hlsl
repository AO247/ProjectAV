#include "ShaderOps.hlsl"
#include "LightVectorData.hlsl"
#include "DirectionalLight.hlsl"
#include "PointLight.hlsl"

cbuffer ObjectCBuf : register(b1)
{ 
    float3 materialColor;  
    bool hasDiffuseMap; 
    float3 specularColor;  
    bool useNormalMap;  
    float normalMapWeight;  
    float specularWeight; 
    float specularGloss;  
};

Texture2D diffuseTexture ;
Texture2D normalTexture : register(t2);  
SamplerState defaultSampler ;

struct PS_INPUT
{
    float4 clipPosition : SV_Position;
    float3 viewPosition : TEXCOORD0;
    float3 viewNormal : NORMAL;
    float2 texCoord : TEXCOORD1;
    float3 viewTangent : TANGENT;
    float3 viewBitangent : BITANGENT;
};

float4 main(PS_INPUT input) : SV_Target
{
    float3 N = normalize(input.viewNormal);
    float3 T = normalize(input.viewTangent);
    float3 B = normalize(input.viewBitangent);

    if ( useNormalMap )  
    {
        const float3 mappedNormal = MapNormal(T, B, N, input.texCoord, normalTexture, defaultSampler);
        N = normalize(lerp(N, mappedNormal, normalMapWeight));
    }
    
    float3 albedo;
    float alpha = 1.0f;
    if ( hasDiffuseMap )
    {
        float4 diffuseSample = diffuseTexture.Sample(defaultSampler, input.texCoord);
        albedo = diffuseSample.rgb;
        alpha = diffuseSample.a;
    }

    float3 final_color = float3(0.0f, 0.0f, 0.0f);

    // Directional Light (from b2)
    final_color += dir_ambient * albedo;
    const float3 dir_to_light_dir_vs = -normalize(viewLightDirection);  
    final_color += Diffuse(dir_diffuseColor, dir_diffuseIntensity, 1.0f, dir_to_light_dir_vs, N) * albedo;
    final_color += Speculate(specularColor, specularWeight, N, dir_to_light_dir_vs, input.viewPosition, 1.0f, specularGloss) * dir_diffuseIntensity * dir_diffuseColor;

 
    return float4(saturate(final_color), alpha);
}