// Plik: ShadowSkinned_VS.hlsl

// Bufor sta³y z macierz¹ transformacji œwiat³a
// Musi pasowaæ do tego, co ustawia twoja klasa ShadowCbuf
cbuffer ShadowTransform : register(b0)
{
    matrix lightTransform; // Zak³adam, ¿e to jest World * LightViewProj
};

// Bufor sta³y z macierzami koœci
// Jest identyczny jak w Skinned_VS.hlsl
cbuffer BoneTransforms : register(b3)
{
    matrix finalBoneMatrices[100];
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
    
    // --- Logika Skinningu (skopiowana i uproszczona z Skinned_VS.hlsl) ---
    // Pêtla po 4 koœciach wp³ywaj¹cych na wierzcho³ek
    for (int i = 0; i < 4; i++)
    {
        // Sprawdzamy, czy waga jest wiêksza od zera, aby unikn¹æ zbêdnych obliczeñ
        if (input.boneWeights[i] > 0.0f)
        {
            // Transformujemy pozycjê wierzcho³ka przez macierz koœci i mno¿ymy przez wagê
            totalLocalPos += mul(float4(input.position, 1.0f), finalBoneMatrices[input.boneIDs[i]]) * input.boneWeights[i];
        }
    }

    // Jeœli wagi nie sumuj¹ siê do 1 (lub nie ma wag), u¿yj oryginalnej pozycji
    // To zabezpieczenie przed "znikaj¹cymi" wierzcho³kami
    if (totalLocalPos.w == 0.0f)
    {
        totalLocalPos = float4(input.position, 1.0f);
    }
    
    // --- Transformacja do przestrzeni œwiat³a ---
    VS_OUTPUT output;
    // Mno¿ymy zdeformowan¹ pozycjê przez macierz transformacji œwiat³a
    output.clipPosition = mul(totalLocalPos, lightTransform);
    
    return output;
}