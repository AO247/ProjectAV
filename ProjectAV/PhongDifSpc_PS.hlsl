// PhongSpec_PS.hlsl - NOWA WERSJA

#include "ShaderOps.hlsl"
#include "LightVectorData.hlsl"

#include "PointLight.hlsl"       // b0
#include "DirectionalLight.hlsl"  // b1

cbuffer ObjectCBuf : register(b1)
{
    bool useGlossAlpha;
    bool useSpecularMap;
    float3 specularColor;
    float specularWeight;
    float specularGloss;
};

Texture2D tex;
Texture2D spec;
SamplerState splr;

float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord) : SV_Target
{
    viewNormal = normalize(viewNormal);
    const float3 materialColor = tex.Sample(splr, tc).rgb;

    // Obliczenia parametrów specular (bez zmian)
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

    // --- Obliczenia dla Point Light (bez zmian) ---
    const LightVectorData lv_point = CalculateLightVectorData(viewLightPos, viewFragPos);
    const float att = Attenuate(attConst, attLin, attQuad, lv_point.distToL);
    const float3 diffuse_point = Diffuse(diffuseColor, diffuseIntensity, att, lv_point.dirToL, viewNormal);
    const float3 specular_point = Speculate(diffuseColor * diffuseIntensity * specularReflectionColor, specularWeight, viewNormal, lv_point.vToL, viewFragPos, att, specularPowerLoaded);

    // --- NOWE: Obliczenia dla Directional Light ---
    const float3 dir_to_light_dir = -normalize(viewLightDirection);
    const float3 diffuse_dir = Diffuse(dir_diffuseColor, dir_diffuseIntensity, 1.0f, dir_to_light_dir, viewNormal);
    // WA¯NE: U¿ywamy tych samych parametrów specular z mapy!
    const float3 specular_dir = Speculate(dir_diffuseColor * dir_diffuseIntensity * specularReflectionColor, specularWeight, viewNormal, dir_to_light_dir, viewFragPos, 1.0f, specularPowerLoaded);
    
    // --- ZMODYFIKOWANE: Kombinacja œwiate³ ---
    const float3 total_ambient = ambient + dir_ambient;
    const float3 final_color = saturate((total_ambient + diffuse_point + diffuse_dir) * materialColor + specular_point + specular_dir);

    return float4(final_color, 1.0f);
}