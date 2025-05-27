// VelocityGen_PS.hlsl

Texture2D g_txDepth : register(t0); // Depth buffer
SamplerState g_samPointClamp : register(s0); // Point sampler for depth

cbuffer PerFrameConstants : register(b0)
{
    matrix g_mCurrentViewProjection;
    matrix g_mPreviousViewProjection;
    matrix g_mInverseCurrentViewProjection;
    float2 g_vRcpBufferDim;
    float blurStrength; // Note: blurStrength and numSamples are not used by this velocity shader
    int numSamples;     // They are part of the cbuffer for the motion blur apply shader
    float2 padding;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
};

// Output: XY screen-space velocity.
// Shader must return float4, even if RT is float2.
float4 main(PS_INPUT input) : SV_TARGET // Changed return type to float4
{
    float depth = g_txDepth.Sample(g_samPointClamp, input.tex).r;

    if (depth >= 1.0f) // or depth > 0.9999f to be safe with float precision
    {
        return float4(0.0f, 0.0f, 0.0f, 0.0f); // Return float4
    }

    float2 ndc_current;
    ndc_current.x = input.tex.x * 2.0f - 1.0f;
    ndc_current.y = (1.0f - input.tex.y) * 2.0f - 1.0f;

    float4 clip_current = float4(ndc_current.xy, depth, 1.0f);

    float4 world_current = mul(clip_current, g_mInverseCurrentViewProjection);
    world_current.xyz /= world_current.w;

    float4 clip_previous = mul(world_current, g_mPreviousViewProjection);
    float2 ndc_previous = clip_previous.xy / clip_previous.w;

    float2 screen_current = input.tex;
    float2 screen_previous;
    screen_previous.x = (ndc_previous.x + 1.0f) * 0.5f;
    screen_previous.y = (1.0f - ndc_previous.y) * 0.5f;

    float2 velocity = screen_current - screen_previous;

    // Return velocity in .xy, and 0 for .zw
    return float4(velocity.x, velocity.y, 0.0f, 0.0f); // Construct and return float4
}