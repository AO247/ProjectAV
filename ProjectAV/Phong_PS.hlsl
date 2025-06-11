
// Phong_PS.hlsl (NOWA WERSJA)

#include "ShaderOps.hlsl"
#include "LightVectorData.hlsl"

#include "PointLight.hlsl"       // Zawiera cbuffer na slocie b0
#include "DirectionalLight.hlsl"  // Zawiera cbuffer na slocie b1

cbuffer ObjectCBuf : register(b1) // Zmieniamy slot, aby nie kolidowa³
{
    float3 materialColor;
    float3 specularColor;
    float specularWeight;
    float specularGloss;
};


float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal) : SV_Target
{
    // Znormalizuj normaln¹ wektora powierzchni
    viewNormal = normalize(viewNormal);

    // --- Obliczenia dla Point Light ---
    const LightVectorData lv_point = CalculateLightVectorData(viewLightPos, viewFragPos);
    const float att = Attenuate(attConst, attLin, attQuad, lv_point.distToL);
    const float3 diffuse_point = Diffuse(diffuseColor, diffuseIntensity, att, lv_point.dirToL, viewNormal);
    const float3 specular_point = Speculate(
        diffuseColor * diffuseIntensity * specularColor, specularWeight, viewNormal,
        lv_point.vToL, viewFragPos, att, specularGloss
    );
    // --- Obliczenia dla Directional Light ---
    // Dla œwiat³a kierunkowego, kierunek do œwiat³a jest po prostu odwróconym kierunkiem œwiat³a.
    // T³umienie (attenuation) wynosi 1.0, bo œwiat³o jest nieskoñczenie daleko.
    const float3 dir_to_light_dir = -normalize(viewLightDirection);
    const float3 diffuse_dir = Diffuse(dir_diffuseColor, dir_diffuseIntensity, 1.0f, dir_to_light_dir, viewNormal);
    const float3 specular_dir = Speculate(
        dir_diffuseColor * dir_diffuseIntensity * specularColor, specularWeight, viewNormal,
        dir_to_light_dir, viewFragPos, 1.0f, specularGloss
    );
    
    // --- Kombinacja œwiate³ ---
    // Sumujemy sk³adowe ambient z obu Ÿróde³
    const float3 total_ambient = ambient + dir_ambient;
    // Sumujemy wszystkie sk³adowe i mno¿ymy przez kolor materia³u
    const float3 final_color = saturate((total_ambient + diffuse_point + diffuse_dir) * materialColor + specular_point + specular_dir);

    return float4(final_color, 1.0f);
}