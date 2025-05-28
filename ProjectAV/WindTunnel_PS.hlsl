cbuffer TimeBuffer : register(b0) {
    float time;
    float3 padding; // tylko 1 float3, nie tablica!
};
Texture2D tex : register(t0);
SamplerState sam : register(s0);

float4 main(float4 pos : SV_POSITION, float2 uv : TEXCOORD0) : SV_Target
{
    // Okreúl rozmiar obszaru efektu (np. 0.4 szerokoúci i 0.4 wysokoúci)
    float2 effectArea = float2(0.4, 0.4);

// Sprawdü, czy uv jest w lewym dolnym rogu
if (uv.x <= effectArea.x && uv.y >= 1.0 - effectArea.y)
{
    // Im bliøej rogu, tym silniejszy efekt
    float strength = 1.0 - (uv.x / effectArea.x) * ((uv.y - (1.0 - effectArea.y)) / effectArea.y);
    float offset = sin(uv.y * 20 + time * 2) * 0.02 * strength;
    float2 newUV = uv + float2(offset, 0);
    return tex.Sample(sam, newUV);
}
else
{
    // Poza obszarem efektu - normalny sampling
    return tex.Sample(sam, uv);
}
}