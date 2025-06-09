// ToneMap_PS.hlsl

Texture2D tex : register(t0);
SamplerState smp : register(s0);

struct VSOut
{
    float2 uv : Texcoord;
    float4 pos : SV_Position;
};

// ACES Filmic Tone Mapping Curve
// Applies a cinematic contrast curve to an LDR image.
float3 aces(float3 x)
{
    const float a = 2.51f;
    const float b = 0.03f;
    const float c = 2.43f;
    const float d = 0.59f;
    const float e = 0.14f;
    return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

float4 main(VSOut i) : SV_Target
{
    // Sample the standard LDR color from the input texture
    float3 ldrColor = tex.Sample(smp, i.uv).rgb;

// Apply the ACES curve to adjust contrast and color
float3 toneMappedColor = aces(ldrColor);

// Apply gamma correction (approximate)
toneMappedColor = pow(toneMappedColor, 1.0f / 2.2f);

return float4(toneMappedColor, 1.0f);
}