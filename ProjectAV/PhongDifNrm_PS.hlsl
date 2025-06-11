// PhongDifNormal_PS.hlsl (WERSJA POPRAWIONA)

#include "ShaderOps.hlsl"
#include "LightVectorData.hlsl"

#include "PointLight.hlsl"
#include "DirectionalLight.hlsl"

cbuffer ObjectCBuf : register(b1)
{
    float3 specularColor;
    float specularWeight;
    float specularGloss;
    bool useNormalMap;
    float normalMapWeight;
};

Texture2D tex;
Texture2D nmap : register(t2);
SamplerState splr;

float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float3 viewTan : Tangent, float3 viewBitan : Bitangent, float2 tc : Texcoord) : SV_Target
{
    // Obliczenia normalnej
    viewNormal = normalize(viewNormal);
    if (useNormalMap)
    {
        const float3 mappedNormal = MapNormal(normalize(viewTan), normalize(viewBitan), viewNormal, tc, nmap, splr);
        viewNormal = lerp(viewNormal, mappedNormal, normalMapWeight);
    }
    
    const float3 materialColor = tex.Sample(splr, tc).rgb;

    // --- Inicjalizacja oœwietlenia ---
    float3 final_color = dir_ambient * materialColor;

    // --- Obliczenia dla Directional Light ---
    const float3 dir_to_light_dir = -normalize(viewLightDirection);
    final_color += Diffuse(dir_diffuseColor, dir_diffuseIntensity, 1.0f, dir_to_light_dir, viewNormal) * materialColor;
    final_color += Speculate(specularColor, specularWeight, viewNormal, dir_to_light_dir, viewFragPos, 1.0f, specularGloss) * dir_diffuseIntensity * dir_diffuseColor;

    // --- Obliczenia dla Point Light (tylko jeœli w³¹czone) ---
    if (enabled)
    {
        final_color += ambient * materialColor;
        
        const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
        const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
        
        final_color += Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, viewNormal) * materialColor;
        final_color += Speculate(specularColor, specularWeight, viewNormal, lv.dirToL, viewFragPos, att, specularGloss) * diffuseIntensity * diffuseColor;
    }
    
    return float4(saturate(final_color), 1.0f);
}