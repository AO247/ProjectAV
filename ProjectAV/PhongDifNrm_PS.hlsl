// PhongDifNormal_PS.hlsl - NOWA WERSJA

#include "ShaderOps.hlsl"
#include "LightVectorData.hlsl"

#include "PointLight.hlsl"       // b0
#include "DirectionalLight.hlsl"  // b1

// Zmieniamy slot, aby nie kolidowa³ ze œwiat³ami
cbuffer ObjectCBuf : register(b1)
{
    float3 specularColor;
    float specularWeight;
    float specularGloss;
    bool useNormalMap;
    float normalMapWeight;
};

Texture2D tex;
Texture2D nmap : register(t2); // Domyœlnie u¿yje slotu t1, bo tex u¿ywa t0
SamplerState splr;

float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float3 viewTan : Tangent, float3 viewBitan : Bitangent, float2 tc : Texcoord) : SV_Target
{
    // Obliczenia normalnej (bez zmian)
    viewNormal = normalize(viewNormal);
    if (useNormalMap)
    {
        const float3 mappedNormal = MapNormal(normalize(viewTan), normalize(viewBitan), viewNormal, tc, nmap, splr);
        viewNormal = lerp(viewNormal, mappedNormal, normalMapWeight);
    }
    
    const float3 materialColor = tex.Sample(splr, tc).rgb;

    // --- Obliczenia dla Point Light (u¿ywa ju¿ poprawnej viewNormal) ---
    const LightVectorData lv_point = CalculateLightVectorData(viewLightPos, viewFragPos);
    const float att = Attenuate(attConst, attLin, attQuad, lv_point.distToL);
    const float3 diffuse_point = Diffuse(diffuseColor, diffuseIntensity, att, lv_point.dirToL, viewNormal);
    const float3 specular_point = Speculate(diffuseColor * diffuseIntensity * specularColor, specularWeight, viewNormal, lv_point.vToL, viewFragPos, att, specularGloss);

    // --- NOWE: Obliczenia dla Directional Light ---
    const float3 dir_to_light_dir = -normalize(viewLightDirection);
    const float3 diffuse_dir = Diffuse(dir_diffuseColor, dir_diffuseIntensity, 1.0f, dir_to_light_dir, viewNormal);
    const float3 specular_dir = Speculate(dir_diffuseColor * dir_diffuseIntensity * specularColor, specularWeight, viewNormal, dir_to_light_dir, viewFragPos, 1.0f, specularGloss);
    
    // --- ZMODYFIKOWANE: Kombinacja œwiate³ ---
    const float3 total_ambient = ambient + dir_ambient;
    const float3 final_color = saturate((total_ambient + diffuse_point + diffuse_dir) * materialColor + specular_point + specular_dir);

    return float4(final_color, 1.0f);
}