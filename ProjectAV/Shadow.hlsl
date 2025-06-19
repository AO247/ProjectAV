Texture2D shadowMap : register(t3); // Tekstura cienia na slocie t3
SamplerComparisonState samShadow : register(s1); // Specjalny sampler na slocie s1

float CalculateShadowFactor(float4 posLight)
{
// 1. Perspective Divide: przekszta³æ z homogenous clip space na NDC
    float3 projCoords = posLight.xyz / posLight.w;

// 2. Przeskaluj z [-1, 1] na [0, 1] dla wspó³rzêdnych tekstury
    projCoords.x = projCoords.x / 2.0f + 0.5f;
    projCoords.y = -projCoords.y / 2.0f + 0.5f; // Y jest odwrócone

// Jeœli piksel jest poza "widokiem" œwiat³a, nie jest w cieniu
    if (saturate(projCoords.x) != projCoords.x || saturate(projCoords.y) != projCoords.y)
    {
        return 1.0f; // Nie w cieniu
    }

// 3. Dodaj "bias" (przesuniêcie), aby unikn¹æ "shadow acne"
    float bias = 0.00008f;
    projCoords.z -= bias;

// 4. Próbkuj mapê cienia i porównaj g³êbie
// SampleCmpLevelZero zwraca 1.0 jeœli test przejdzie (piksel oœwietlony), 0.0 jeœli nie (w cieniu)
    return shadowMap.SampleCmpLevelZero(samShadow, projCoords.xy, projCoords.z);
}