#include "Transform.hlsl"

cbuffer BoneTransforms : register(b3)
{
    matrix finalBoneMatrices[100];
};

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD0;
    int4 boneIDs : BLENDINDICES0;
    float4 boneWeights : BLENDWEIGHTS0;
};

struct PS_INPUT
{
    float4 clipPosition : SV_Position;
    float3 viewNormal : NORMAL;
    float2 texCoord : TEXCOORD0;
    float3 viewPosition : TEXCOORD1;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    
    float4 totalPosition = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 totalNormal = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    // ZAKLADAM, ZE MAX 4 KOSCI MAJA WPLYW
    // ORAZ MOZEMY MIEC MAX 100 KOSCI
    
    for (int i = 0; i < 4; i++)
    {
        if (input.boneIDs[i] == -1)
        {
            continue;
        }
        if (input.boneIDs[i] >= 100)
        {
            totalPosition = float4(input.position, 1.0f);
            break;
        }

        float4 localPosition = mul(float4(input.position, 1.0f), finalBoneMatrices[input.boneIDs[i]]);
        totalPosition += localPosition * input.boneWeights[i];
        float4 localNormal = mul(float4(input.normal, 0.0f), finalBoneMatrices[input.boneIDs[i]]);
        totalNormal += localNormal * input.boneWeights[i];
    }

    output.clipPosition = mul(totalPosition, modelViewProj);
    output.viewPosition = mul(totalPosition, modelView).xyz;
    output.viewNormal = normalize(mul(totalNormal.xyz, (float3x3) modelView));
    output.texCoord = input.texCoord;
    
    return output;
}