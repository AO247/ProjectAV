// Plik: ShadowSkinned_VS.hlsl

// Bufor sta�y z macierz� transformacji �wiat�a
// Musi pasowa� do tego, co ustawia twoja klasa ShadowCbuf
cbuffer ShadowTransform : register(b0)
{
    matrix lightTransform; // Zak�adam, �e to jest World * LightViewProj
};

// Bufor sta�y z macierzami ko�ci
// Jest identyczny jak w Skinned_VS.hlsl
cbuffer BoneTransforms : register(b3)
{
    matrix finalBoneMatrices[100];
};

// Struktura wej�ciowa musi pasowa� do Vertex Layoutu animowanego modelu
// Skopiowana z Skinned_VS.hlsl
struct VS_INPUT
{
    float3 position : POSITION;
    float4 boneWeights : BLENDWEIGHTS0;
    int4 boneIDs : BLENDINDICES0;
};

// Struktura wyj�ciowa jest bardzo prosta - tylko pozycja
struct VS_OUTPUT
{
    float4 clipPosition : SV_POSITION;
};

VS_OUTPUT main(VS_INPUT input)
{
    float4 totalLocalPos = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    // --- Logika Skinningu (skopiowana i uproszczona z Skinned_VS.hlsl) ---
    // P�tla po 4 ko�ciach wp�ywaj�cych na wierzcho�ek
    for (int i = 0; i < 4; i++)
    {
        // Sprawdzamy, czy waga jest wi�ksza od zera, aby unikn�� zb�dnych oblicze�
        if (input.boneWeights[i] > 0.0f)
        {
            // Transformujemy pozycj� wierzcho�ka przez macierz ko�ci i mno�ymy przez wag�
            totalLocalPos += mul(float4(input.position, 1.0f), finalBoneMatrices[input.boneIDs[i]]) * input.boneWeights[i];
        }
    }

    // Je�li wagi nie sumuj� si� do 1 (lub nie ma wag), u�yj oryginalnej pozycji
    // To zabezpieczenie przed "znikaj�cymi" wierzcho�kami
    if (totalLocalPos.w == 0.0f)
    {
        totalLocalPos = float4(input.position, 1.0f);
    }
    
    // --- Transformacja do przestrzeni �wiat�a ---
    VS_OUTPUT output;
    // Mno�ymy zdeformowan� pozycj� przez macierz transformacji �wiat�a
    output.clipPosition = mul(totalLocalPos, lightTransform);
    
    return output;
}