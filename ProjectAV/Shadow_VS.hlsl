// Ten cbuffer bêdzie zawiera³ JEDN¥ macierz: Model * LightView * LightProjection
cbuffer TransformCBuf : register(b0)
{
    matrix lightTransform;
};

// Wejœcie jest bardzo proste - potrzebujemy tylko pozycji wierzcho³ka.
// Wyjœcie jest jeszcze prostsze - tylko pozycja na ekranie (w tym przypadku na teksturze cienia).
float4 main(float3 pos : Position) : SV_Position
{
    return mul(float4(pos, 1.0f), lightTransform);
}