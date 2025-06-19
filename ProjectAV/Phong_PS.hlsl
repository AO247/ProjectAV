#include "ShaderOps.hlsl"
#include "LightVectorData.hlsl"
#include "PointLight.hlsl"
#include "DirectionalLight.hlsl"
#include "Shadow.hlsl"

cbuffer ObjectCBuf : register(b1)
{
    float3 materialColor;
    float3 specularColor;
    float specularWeight;
    float specularGloss;
};

float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float4 posLight : TEXCOORD1) : SV_Target
{
    viewNormal = normalize(viewNormal);
    float shadowFactor = CalculateShadowFactor(posLight);
    
    float3 total_ambient = dir_ambient;
    if (enabled)
    {
        total_ambient += ambient;
    }

    float3 total_diffuse = { 0.0f, 0.0f, 0.0f };
    float3 total_specular = { 0.0f, 0.0f, 0.0f };

    const float3 dir_to_light_dir = -normalize(viewLightDirection);
    total_diffuse += Diffuse(dir_diffuseColor, dir_diffuseIntensity, 1.0f, dir_to_light_dir, viewNormal) * materialColor;
    total_specular += Speculate(specularColor, specularWeight, viewNormal, dir_to_light_dir, viewFragPos, 1.0f, specularGloss) * dir_diffuseIntensity * dir_diffuseColor;

    if (enabled)
    {
        const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
        const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
        total_diffuse += Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, viewNormal) * materialColor;
        total_specular += Speculate(specularColor, specularWeight, viewNormal, lv.dirToL, viewFragPos, att, specularGloss) * diffuseIntensity * diffuseColor;
    }
    
    float3 final_color = total_ambient * materialColor + (total_diffuse + total_specular) * shadowFactor;
    return float4(saturate(final_color), 1.0f);
}