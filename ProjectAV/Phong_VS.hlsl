#include "Transform.hlsl"

struct VSOut
{
    float3 viewPos : Position;
    float3 viewNormal : Normal;
    float4 posLight : TEXCOORD1;
    float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float3 n : Normal)
{
    VSOut vso;
    vso.viewPos = (float3) mul(float4(pos, 1.0f), modelView);
    vso.viewNormal = mul(n, (float3x3) modelView);
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    vso.posLight = mul(float4(pos, 1.0f), lightTransform);
    return vso;
}