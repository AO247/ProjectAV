#include "ShaderOps.hlsl"
#include "LightVectorData.hlsl"
#include "PointLight.hlsl"

cbuffer ObjectCBuf
{
    float specularIntensity;
    float specularPower;
    float padding[2];
};

Texture2D tex;
SamplerState splr;

float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord) : SV_Target
{
    // renormalize interpolated normal
    viewNormal = normalize(viewNormal);
    
    // fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
    
    // attenuation
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
    
    // diffuse
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, viewNormal);
    
    // Blinn-Phong specular (modified from original Phong)
    float3 viewDir = normalize(-viewFragPos);
    float3 halfwayDir = normalize(lv.dirToL + viewDir);
    float specularAngle = max(dot(viewNormal, halfwayDir), 0.0);
    float3 specular = specularIntensity * diffuseColor * pow(specularAngle, specularPower) * att;

    // Sample texture color
    float3 texColor = tex.Sample(splr, tc).rgb;

    // Desaturate (grayscale conversion)
    float luminance = dot(texColor, float3(0.299, 0.587, 0.114));
    float3 desaturatedColor = float3(luminance, luminance, luminance);

    
    // rimlight - zakomentowane
    float rim = 1.0 - saturate(dot(viewDir, viewNormal));
    float3 rimColor = float3(0.5, 0.8, 1.0); // Light blue rim
    float rimIntensity = pow(rim, 2.0) * 2.0; // Power and multiplier for effect strength
    float3 rimLight = rimColor * rimIntensity;
    

    // Combine all lighting components
    return float4(saturate((diffuse + ambient  + rimLight ) * desaturatedColor + specular), 1.0f);

    //return float4(saturate((diffuse + ambient /* + rimLight */) * texColor + specular), 1.0f);
}