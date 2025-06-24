// Ten cbuffer b�dzie zawiera� JEDN� macierz: Model * LightView * LightProjection
cbuffer TransformCBuf : register(b0)
{
    matrix lightTransform;
};

// Wej�cie jest bardzo proste - potrzebujemy tylko pozycji wierzcho�ka.
// Wyj�cie jest jeszcze prostsze - tylko pozycja na ekranie (w tym przypadku na teksturze cienia).
float4 main(float3 pos : Position) : SV_Position
{
    return mul(float4(pos, 1.0f), lightTransform);
}