#include "ShaderOps.hlsl"
#include "LightVectorData.hlsl"
#include "DirectionalLight.hlsl"

struct PS_INPUT
{
    float4 clipPosition : SV_Position;
    float3 viewNormal : NORMAL;
    float2 texCoord : TEXCOORD0;
    float3 viewPosition : TEXCOORD1;
};

float4 main(PS_INPUT input) : SV_Target
{
    // Normalize the interpolated normal vector
    const float3 normal = normalize(input.viewNormal);

    // ======== HARDCODED MATERIAL PROPERTIES ========
    // As requested, no textures or material cbuffer. Just a solid color.
    const float3 solidColor = float3(0.9f, 0.75f, 0.65f); // A simple flesh/stone tone
    const float3 specularColor = float3(1.0f, 1.0f, 1.0f); // White specular highlights
    const float specularWeight = 0.8f;
    const float specularGloss = 30.0f;
    // ===============================================

    // Start with the ambient light contribution
    float3 final_color = dir_ambient * solidColor;

    // The direction vector FROM the light source is given. We need the vector TO the light source.
    const float3 dirToLight = -normalize(viewLightDirection);

    // Add the diffuse lighting component
    final_color += Diffuse(dir_diffuseColor, dir_diffuseIntensity, 1.0f, dirToLight, normal) * solidColor;

    // Add the specular highlight component
    // The highlight's color and intensity is modulated by the light's color and intensity
    final_color += Speculate(specularColor, specularWeight, normal, dirToLight, input.viewPosition, 1.0f, specularGloss) * dir_diffuseIntensity * dir_diffuseColor;

    // Return the final saturated color with full alpha
    return float4(saturate(final_color), 1.0f);
}