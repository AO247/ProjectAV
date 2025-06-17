Texture2D tex : register(t0);
SamplerState smp : register(s0);

struct VSOut
{
    float4 pos : SV_Position;
    float4 color : Color;
    float2 tex : TexCoord;
};

float4 main(VSOut input) : SV_Target
{
    float4 texColor = tex.Sample(smp, input.tex);
    return texColor * input.color;
}