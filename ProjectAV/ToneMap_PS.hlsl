Texture2D tex : register(t0);
SamplerState smp : register(s0);

cbuffer PostProcessSettings : register(b0)
{
    float exposure;
    float sharpenAmount;
    float embossAmount;
    bool useSharpening;
    bool useEmboss;
    bool useOutline;
    float3 outlineColor;
    float outlineThreshold;
};

struct VSOut
{
    float2 uv : Texcoord;
    float4 pos : SV_Position;
};

float GetLuma(float3 color)
{
    return dot(color, float3(0.2126, 0.7152, 0.0822));
}

float4 main(VSOut i) : SV_Target
{
    float2 texelSize;
    tex.GetDimensions(texelSize.x, texelSize.y);
    texelSize = 1.0f / texelSize;

    float3 processedColor = tex.Sample(smp, i.uv).rgb * exposure;

    if (useSharpening)
    {
        float3 s0 = tex.Sample(smp, i.uv + texelSize * float2(-1, -1)).rgb * exposure;
        float3 s1 = tex.Sample(smp, i.uv + texelSize * float2(0, -1)).rgb * exposure;
        float3 s2 = tex.Sample(smp, i.uv + texelSize * float2(1, -1)).rgb * exposure;
        float3 s3 = tex.Sample(smp, i.uv + texelSize * float2(-1,  0)).rgb * exposure;
        float3 s4 = tex.Sample(smp, i.uv + texelSize * float2(1,  0)).rgb * exposure;
        float3 s5 = tex.Sample(smp, i.uv + texelSize * float2(-1,  1)).rgb * exposure;
        float3 s6 = tex.Sample(smp, i.uv + texelSize * float2(0,  1)).rgb * exposure;
        float3 s7 = tex.Sample(smp, i.uv + texelSize * float2(1,  1)).rgb * exposure;

        float3 sharpened = processedColor * 9.0f - (s0 + s1 + s2 + s3 + s4 + s5 + s6 + s7);
        processedColor = lerp(processedColor, sharpened, sharpenAmount);
    }

    if (useEmboss)
    {
        float3 topLeft = tex.Sample(smp, i.uv - texelSize).rgb * exposure;
        float3 bottomRight = tex.Sample(smp, i.uv + texelSize).rgb * exposure;

        float3 embossed = float3(0.5, 0.5, 0.5) + (topLeft - bottomRight);

        processedColor = lerp(processedColor, embossed, embossAmount);
    }

    if (useOutline)
    {
        float l0 = GetLuma(tex.Sample(smp, i.uv + texelSize * float2(-1, -1)).rgb);
        float l1 = GetLuma(tex.Sample(smp, i.uv + texelSize * float2(0, -1)).rgb);
        float l2 = GetLuma(tex.Sample(smp, i.uv + texelSize * float2(1, -1)).rgb);
        float l3 = GetLuma(tex.Sample(smp, i.uv + texelSize * float2(-1,  0)).rgb);
        float l5 = GetLuma(tex.Sample(smp, i.uv + texelSize * float2(1,  0)).rgb);
        float l6 = GetLuma(tex.Sample(smp, i.uv + texelSize * float2(-1,  1)).rgb);
        float l7 = GetLuma(tex.Sample(smp, i.uv + texelSize * float2(0,  1)).rgb);
        float l8 = GetLuma(tex.Sample(smp, i.uv + texelSize * float2(1,  1)).rgb);

        float Gx = l0 + 2.0f * l3 + l6 - (l2 + 2.0f * l5 + l8);
        float Gy = l0 + 2.0f * l1 + l2 - (l6 + 2.0f * l7 + l8);

        float edgeStrength = sqrt(Gx * Gx + Gy * Gy);

        float outlineFactor = smoothstep(outlineThreshold, outlineThreshold + 0.2f, edgeStrength);

        processedColor = lerp(processedColor, outlineColor, outlineFactor);
    }

    float3 finalColor = pow(processedColor, 1.0f / 2.2f);

    return float4(finalColor, 1.0f);
}