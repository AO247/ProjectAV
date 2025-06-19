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
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    int4 boneIDs : BLENDINDICES0;
    float4 boneWeights : BLENDWEIGHTS0;
};

struct PS_INPUT
{
    float4 clipPosition : SV_Position;
    float3 viewPosition : TEXCOORD0;
    float3 viewNormal : NORMAL;
    float2 texCoord : TEXCOORD1;
    float3 viewTangent : TANGENT;
    float3 viewBitangent : BITANGENT;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    
    float4 totalLocalPos = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 totalLocalNormal = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 totalLocalTangent = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 totalLocalBitangent = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
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
            totalLocalPos = float4(input.position, 1.0f);
            totalLocalNormal = float4(input.normal, 0.0f);
            totalLocalTangent = float4(input.tangent, 0.0f);
            totalLocalBitangent = float4(input.bitangent, 0.0f);
            break;
        }

        
        matrix boneTransform = finalBoneMatrices[input.boneIDs[i]];
        float weight = input.boneWeights[i];

        totalLocalPos += mul(float4(input.position, 1.0f), boneTransform) * weight;
        totalLocalNormal += mul(float4(input.normal, 0.0f), boneTransform) * weight; // Normals are vectors
        totalLocalTangent += mul(float4(input.tangent, 0.0f), boneTransform) * weight; // Tangents are vectors
        totalLocalBitangent += mul(float4(input.bitangent, 0.0f), boneTransform) * weight;
        //float4 localPosition = mul(float4(input.position, 1.0f), finalBoneMatrices[input.boneIDs[i]]);
        //totalPosition += localPosition * input.boneWeights[i];
        //float4 localNormal = mul(float4(input.normal, 0.0f), finalBoneMatrices[input.boneIDs[i]]);
        //totalNormal += localNormal * input.boneWeights[i];
    }

    // Transform skinned local attributes to view/clip space
    output.clipPosition = mul(totalLocalPos, modelViewProj);
    output.viewPosition = mul(totalLocalPos, modelView).xyz;
    
    // For normals and TBN vectors, use the 3x3 part of modelView (avoids translation)
    // and normalize as skinning and non-uniform scaling can change length.
    output.viewNormal = normalize(mul(totalLocalNormal.xyz, (float3x3) modelView));
    output.viewTangent = normalize(mul(totalLocalTangent.xyz, (float3x3) modelView));
    output.viewBitangent = normalize(mul(totalLocalBitangent.xyz, (float3x3) modelView));
    
    output.texCoord = input.texCoord;
    
    return output;
}