Texture2D tex : register(t0);
SamplerState smp : register(s0);

struct PSInput
{
    float4 color : COLOR;
    float2 tc : TEXCOORD;
};

float4 main(PSInput psi) : SV_TARGET
{
    float4 texColor = tex.Sample(smp, psi.tc);
    
    // Combine texture alpha with particle color alpha for soft particles
    return texColor * psi.color;
}