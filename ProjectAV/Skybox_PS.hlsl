TextureCube skyTexture : register(t0);
SamplerState smp : register(s0);

// Use the *exact same* struct definition as the VS output
struct VS_TO_PS
{
    float4 pos : SV_Position;
    float3 tex : TEXCOORD0; // Input semantic must match
};

float4 main(VS_TO_PS pin) : SV_Target
{
    return skyTexture.Sample(smp, pin.tex);
}