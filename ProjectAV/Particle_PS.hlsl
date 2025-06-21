Texture2D tex : register(t0);
SamplerState smp : register(s0);

struct PSInput
{
    float4 color : COLOR; // The particle's interpolated lifetime color (e.g. starts white, fades to grey)
    float2 tc : TEXCOORD;
};

float4 main(PSInput psi) : SV_TARGET
{
    // Sample the color and alpha from the fire texture
    float4 textureColor = tex.Sample(smp, psi.tc);

    // Modulate the texture's color by the particle's lifetime color.
    // e.g., (Orange Texture) * (White Tint) = Orange Color
    float3 final_rgb = textureColor.rgb * psi.color.rgb;

    // The final alpha is the texture's alpha multiplied by the particle's lifetime fade.
    float final_alpha = textureColor.a * psi.color.a;

    return float4(final_rgb, final_alpha);
}