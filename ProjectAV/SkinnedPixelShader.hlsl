// SkinnedPixelShader.hlsl

// Texture and Sampler (assuming texture on slot t0, sampler on s0)
Texture2D ColorTexture : register(t0);
SamplerState SamplerAniso : register(s0); // Or SamplerLinear, SamplerPoint, etc.

// Constant buffer for pixel shader specific data (like lighting)
cbuffer PixelShaderConstants : register(b2) // Use a different slot than VS (b0, b1)
{
    float3 LightDirection; // Direction TO the light source (e.g., (0, 1, -1))
    float Padding1; // HLSL packing rules
    float4 LightColor; // Color and Intensity (e.g., (1,1,1,1))
    float4 AmbientColor; // Ambient light (e.g., (0.2, 0.2, 0.2, 1.0))
};

struct PixelInputType
{
    float4 position : SV_POSITION; // System Value: Pixel position in screen space
    float3 normal : NORMAL; // Interpolated world-space normal
    float2 texCoord : TEXCOORD0; // Interpolated texture coordinates
    float3 worldPos : WORLDPOS; // Interpolated world-space position
};

// Pixel Shader entry point
float4 main(PixelInputType input) : SV_TARGET // SV_TARGET is the output render target
{
    // Normalize the interpolated normal vector (important!)
    float3 N = normalize(input.normal);

    // Sample the texture
    float4 textureColor = ColorTexture.Sample(SamplerAniso, input.texCoord);

    // Calculate diffuse lighting component
    // LightDirection in cbuffer is direction TO the light. For dot product, we need vector FROM surface TO light.
    float3 lightVec = normalize(LightDirection); // Or -LightDirection if LightDirection is from light source
    float diffuseIntensity = saturate(dot(N, lightVec)); // saturate clamps between 0 and 1
    float3 diffuseColor = textureColor.rgb * LightColor.rgb * LightColor.a * diffuseIntensity; // LightColor.a can be intensity

    // Combine with ambient light
    float3 finalColor = AmbientColor.rgb * textureColor.rgb + diffuseColor;

    // Output the final color (ensure alpha from texture or set explicitly)
    return float4(finalColor, textureColor.a);
}