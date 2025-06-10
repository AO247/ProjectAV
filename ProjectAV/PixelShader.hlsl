Texture2D diffuseTexture : register(t0);
SamplerState basicSampler : register(s0);
#include "PointLight.hlsl"

struct PS_INPUT
{
    float4 clipPosition : SV_Position;
    float3 viewNormal : NORMAL;
    float2 texCoord : TEXCOORD0;
    float3 viewPosition : TEXCOORD1;
};

float4 main(PS_INPUT input) : SV_Target
{
    float4 texSample = float4(1.0f, 0.0f, 1.0f, 1.0f); // default magenta
    float3 materialBaseColor = texSample.rgb;

    float3 N = normalize(input.viewNormal);
    float3 L_unnormalized = viewLightPos - input.viewPosition;
    float distanceToLight = length(L_unnormalized);
    float3 L = normalize(L_unnormalized);

    float3 ambientTerm = materialBaseColor * ambient;
    float NdotL = max(0.0f, dot(N, L));
    float3 diffuseTerm = materialBaseColor * diffuseColor * diffuseIntensity * NdotL;
    float attenuation = 1.0f / (attConst + attLin * distanceToLight + attQuad * distanceToLight * distanceToLight);

    float3 finalColor = ambientTerm + (diffuseTerm * attenuation);
    return float4(finalColor, texSample.a);
}
