// MotionBlurApply_PS.hlsl

Texture2D g_txSceneColor : register(t0); // Full resolution scene
Texture2D g_txVelocity : register(t1); // Velocity buffer
SamplerState g_samLinearClamp : register(s0); // For scene color
SamplerState g_samPointClamp : register(s1); // For velocity

cbuffer PerFrameConstants : register(b0)
{
    matrix g_mCurrentViewProjection;
    matrix g_mPreviousViewProjection;
    matrix g_mInverseCurrentViewProjection;
    float2 g_vRcpBufferDim;
    float  g_fBlurStrength;
    int    g_iNumSamples;
    float2 padding;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float2 velocity = g_txVelocity.Sample(g_samPointClamp, input.tex).xy;

    if (abs(velocity.x) < 0.00001f && abs(velocity.y) < 0.00001f)
    {
        return g_txSceneColor.Sample(g_samLinearClamp, input.tex);
    }

    float4 blurredColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float totalWeight = 0.0f;

    int numSamples = max(1, g_iNumSamples); // Ensure at least one sample

    // Ensure denominator is not zero if numSamples is 1
    float invNumSamplesMinusOne = 0.0f;
    if (numSamples > 1) {
        invNumSamplesMinusOne = 1.0f / (float)(numSamples - 1);
    }

    [loop] // <--- ADD THIS
    for (int i = 0; i < numSamples; ++i)
    {
        // If numSamples is 1, t should be 0. Otherwise, t ranges from 0 to 1.
        float t = (numSamples == 1) ? 0.0f : (float)i * invNumSamplesMinusOne;

        float2 sampleTexCoord = input.tex - velocity * t * g_fBlurStrength;

        blurredColor += g_txSceneColor.Sample(g_samLinearClamp, sampleTexCoord);
        // totalWeight += 1.0f; // If all samples have equal weight
    }

    // If numSamples is 0 (due to max(1,g_iNumSamples) it won't be), this would divide by zero.
    // Since numSamples is at least 1, this is safe.
    return blurredColor / (float)numSamples; // Average the samples
}