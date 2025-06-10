// Transformation buffers
cbuffer PerObjectTransforms : register(b0)
{
    matrix modelView; // Transpose(World * View)
    matrix modelViewProj; // Transpose(World * View * Projection)
};

// Skinning (bone matrix array)
cbuffer BoneTransforms : register(b3)
{
    matrix finalBoneMatrices[128]; // Transposed bone transforms
};

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD0;
    int4 boneIDs : BLENDINDICES0;
    float4 boneWeights : BLENDWEIGHT0;
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
    PS_INPUT output = (PS_INPUT) 0;

    float4 modelPos = float4(input.position, 1.0f);
    float4 modelNormal = float4(input.normal, 0.0f);

    float4 skinnedPos = float4(0, 0, 0, 0);
    float4 skinnedNormal = float4(0, 0, 0, 0);

    // Skinning with up to 4 weights
    for (int i = 0; i < 4; ++i)
    {
        if (input.boneWeights[i] > 0.00001f)
        {
            int boneIndex = input.boneIDs[i];
            matrix boneMatrix = finalBoneMatrices[boneIndex];

            skinnedPos += input.boneWeights[i] * mul(modelPos, boneMatrix);
            skinnedNormal += input.boneWeights[i] * mul(modelNormal, boneMatrix);
        }
    }

    // Transform to view/clip space
    output.clipPosition = mul(skinnedPos, modelViewProj);
    output.viewPosition = mul(skinnedPos, modelView).xyz;
    output.viewNormal = normalize(mul(skinnedNormal.xyz, (float3x3) modelView));
    output.texCoord = input.texCoord;

    return output;
}
