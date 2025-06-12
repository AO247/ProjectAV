TextureCube skyTexture : register(t0);
SamplerState smp : register(s0);


struct VS_TO_PS
{
    float4 pos : SV_Position;
    float3 tex : TEXCOORD0;
};

float4 main(VS_TO_PS pin) : SV_Target
{
    return skyTexture.Sample(smp, pin.tex);
}