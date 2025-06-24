#include "ShaderOps.hlsl"
#include "LightVectorData.hlsl"
#include "PointLight.hlsl"
#include "DirectionalLight.hlsl"
#include "Shadow.hlsl"

cbuffer ObjectCBuf : register(b1)
{
    bool useGlossAlpha;
    bool useSpecularMap;
    float3 specularColor;
    float specularWeight;
    float specularGloss;
    bool useNormalMap;
    float normalMapWeight;
};

Texture2D tex;
Texture2D spec;
Texture2D nmap;
SamplerState splr;

float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float3 viewTan : Tangent, float3 viewBitan : Bitangent, float2 tc : Texcoord, float4 posLight : TEXCOORD1) : SV_Target
{
    viewNormal = normalize(viewNormal);
    if (useNormalMap)
    {
        const float3 mappedNormal = MapNormal(normalize(viewTan), normalize(viewBitan), viewNormal, tc, nmap, splr);
        viewNormal = lerp(viewNormal, mappedNormal, normalMapWeight);
    }

    const float3 materialColor = tex.Sample(splr, tc).rgb;
    
    float specularPowerLoaded = specularGloss;
    const float4 specularSample = spec.Sample(splr, tc);
    float3 specularReflectionColor;
    if (useSpecularMap)
    {
        specularReflectionColor = specularSample.rgb;
    }
    else
    {
        specularReflectionColor = specularColor;
    }
    if (useGlossAlpha)
    {
        specularPowerLoaded = pow(2.0f, specularSample.a * 13.0f);
    }

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
    total_specular += Speculate(specularReflectionColor, specularWeight, viewNormal, dir_to_light_dir, viewFragPos, 1.0f, specularPowerLoaded) * dir_diffuseIntensity * dir_diffuseColor;

    if (enabled)
    {
        const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
        const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
        total_diffuse += Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, viewNormal) * materialColor;
        total_specular += Speculate(specularReflectionColor, specularWeight, viewNormal, lv.dirToL, viewFragPos, att, specularPowerLoaded) * diffuseIntensity * diffuseColor;
    }
    
    float3 final_color = total_ambient * materialColor + (total_diffuse + total_specular) * shadowFactor;
    return float4(saturate(final_color), 1.0f);
}