#include "ShaderOps.hlsl"
#include "LightVectorData.hlsl"

#include "PointLight.hlsl"
#include "DirectionalLight.hlsl"

cbuffer ObjectCBuf : register(b1)
{
    float3 materialColor;
    float3 specularColor;
    float specularWeight;
    float specularGloss;
};


float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal) : SV_Target
{
    // Znormalizuj normaln¹ wektora powierzchni na pocz¹tku
    viewNormal = normalize(viewNormal);

    // --- Inicjalizacja ca³kowitego oœwietlenia ---
    // Zaczynamy od œwiat³a otoczenia ze œwiat³a kierunkowego
    float3 final_color = dir_ambient * materialColor;

    // --- Obliczenia dla Directional Light ---
    const float3 dir_to_light_dir = -normalize(viewLightDirection);
    // Diffuse
    final_color += Diffuse(dir_diffuseColor, dir_diffuseIntensity, 1.0f, dir_to_light_dir, viewNormal) * materialColor;
    // Specular
    final_color += Speculate(
        specularColor, // Kolor odbicia z materia³u
        specularWeight,
        viewNormal,
        dir_to_light_dir,
        viewFragPos,
        1.0f, // T³umienie (attenuation)
        specularGloss
    ) * dir_diffuseIntensity * dir_diffuseColor; // Mno¿ymy przez intensywnoœæ i kolor œwiat³a kierunkowego

    
    // --- Obliczenia dla Point Light (tylko jeœli w³¹czone) ---
    if (enabled)
    {
        // Najpierw dodajemy wk³ad ambient z tego œwiat³a
        final_color += ambient * materialColor;

        const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
        const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
        
        // Diffuse
        final_color += Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, viewNormal) * materialColor;
        // Specular
        final_color += Speculate(
            specularColor,
            specularWeight,
            viewNormal,
            lv.dirToL, // U¿ywamy znormalizowanego wektora
            viewFragPos,
            att,
            specularGloss
        ) * diffuseIntensity * diffuseColor; // Mno¿ymy przez intensywnoœæ i kolor œwiat³a punktowego
    }
    
    // Na koniec nasycamy kolor, aby upewniæ siê, ¿e jest w zakresie [0, 1]
    return float4(saturate(final_color), 1.0f);
}