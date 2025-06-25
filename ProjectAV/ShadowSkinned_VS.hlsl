// Plik: ShadowSkinned_VS.hlsl

// Bufor sta³y z macierz¹ transformacji œwiat³a
// Musi pasowaæ do tego, co ustawia twoja klasa ShadowCbuf
cbuffer ShadowCbuf : register(b0)
{
    matrix lightTransform; // Zak³adam, ¿e to jest World * LightViewProj
};

// Bufor sta³y z macierzami koœci
// Jest identyczny jak w Skinned_VS.hlsl
cbuffer BoneTransforms : register(b3)
{
    matrix finalBoneMatrices[150];
};

// Struktura wejœciowa musi pasowaæ do Vertex Layoutu animowanego modelu
// Skopiowana z Skinned_VS.hlsl
struct VS_INPUT
{
    float3 position : POSITION;
    float4 boneWeights : BLENDWEIGHTS0;
    int4 boneIDs : BLENDINDICES0;
};

// Struktura wyjœciowa jest bardzo prosta - tylko pozycja
struct VS_OUTPUT
{
    float4 clipPosition : SV_POSITION;
};

VS_OUTPUT main(VS_INPUT input)
{
    float4 totalLocalPos = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    // --- Logika Skinningu ---
    for (int i = 0; i < 4; i++)
    {
        if (input.boneWeights[i] > 0.0f)
        {
            totalLocalPos += mul(float4(input.position, 1.0f), finalBoneMatrices[input.boneIDs[i]]) * input.boneWeights[i];
        }
    }

    if (totalLocalPos.w == 0.0f)
    {
        totalLocalPos = float4(input.position, 1.0f);
    }
    
    // --- Transformacja do przestrzeni œwiat³a ---
    VS_OUTPUT output;
    output.clipPosition = mul(totalLocalPos, lightTransform);
    
    return output;
}