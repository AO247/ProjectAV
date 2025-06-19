Texture2D shadowMap : register(t3); // Tekstura cienia na slocie t3
SamplerComparisonState samShadow : register(s1); // Specjalny sampler na slocie s1

float CalculateShadowFactor(float4 posLight)
{
// 1. Perspective Divide: przekszta�� z homogenous clip space na NDC
    float3 projCoords = posLight.xyz / posLight.w;

// 2. Przeskaluj z [-1, 1] na [0, 1] dla wsp�rz�dnych tekstury
    projCoords.x = projCoords.x / 2.0f + 0.5f;
    projCoords.y = -projCoords.y / 2.0f + 0.5f; // Y jest odwr�cone

// Je�li piksel jest poza "widokiem" �wiat�a, nie jest w cieniu
    if (saturate(projCoords.x) != projCoords.x || saturate(projCoords.y) != projCoords.y)
    {
        return 1.0f; // Nie w cieniu
    }

// 3. Dodaj "bias" (przesuni�cie), aby unikn�� "shadow acne"
    float bias = 0.00008f;
    projCoords.z -= bias;

// 4. Pr�bkuj map� cienia i por�wnaj g��bie
// SampleCmpLevelZero zwraca 1.0 je�li test przejdzie (piksel o�wietlony), 0.0 je�li nie (w cieniu)
    return shadowMap.SampleCmpLevelZero(samShadow, projCoords.xy, projCoords.z);
}