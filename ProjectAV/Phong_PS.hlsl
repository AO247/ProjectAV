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

    viewNormal = normalize(viewNormal);


    float3 final_color = dir_ambient * materialColor;


    const float3 dir_to_light_dir = -normalize(viewLightDirection);

    final_color += Diffuse(dir_diffuseColor, dir_diffuseIntensity, 1.0f, dir_to_light_dir, viewNormal) * materialColor;

    final_color += Speculate(
        specularColor,
        specularWeight,
        viewNormal,
        dir_to_light_dir,
        viewFragPos,
        1.0f, 
        specularGloss
    ) * dir_diffuseIntensity * dir_diffuseColor;

    
    if (enabled)
    {

        final_color += ambient * materialColor;

        const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
        const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
        

        final_color += Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, viewNormal) * materialColor;

        final_color += Speculate(
            specularColor,
            specularWeight,
            viewNormal,
            lv.dirToL, 
            viewFragPos,
            att,
            specularGloss
        ) * diffuseIntensity * diffuseColor; 
    }

    return float4(saturate(final_color), 1.0f);
}