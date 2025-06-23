// Shadow.hlsl

Texture2D shadowMap : register(t3);
SamplerComparisonState samShadow : register(s1);

// Get the dimensions of the shadow map (needed for texel size)
// You'll need to pass this in a constant buffer or use GetDimensions intrinsic
// For simplicity now, let's assume you know it (e.g. 2048x2048)
static const float2 SHADOW_MAP_SIZE = float2(8192.0f, 8192.0f); // Or pass via CBuffer

float CalculateShadowFactor(float4 posLight)
{
    // 1. Perspective Divide
    float3 projCoords = posLight.xyz / posLight.w;

    // 2. Transform to [0,1] texture coordinates
    projCoords.x = projCoords.x * 0.5f + 0.5f;
    projCoords.y = -projCoords.y * 0.5f + 0.5f; // Y is often flipped

    // If pixel is outside light's frustum (screen bounds for shadow map)
    if (saturate(projCoords.x) != projCoords.x || saturate(projCoords.y) != projCoords.y || projCoords.z < 0.0f || projCoords.z > 1.0f)
    {
        return 1.0f; // Not in shadow
    }

    // 3. Depth Bias
    float bias = 0.00008f; // Adjust as needed
    // For orthographic projection, a constant bias is often okay.
    // For perspective projection, a slope-scaled bias is better:
    // float bias = max(0.00005 * (1.0 - dot(normal, lightDir)), 0.000005);

    float shadowFactor = 0.0f;
    float texelSizeX = 1.0f / SHADOW_MAP_SIZE.x;
    float texelSizeY = 1.0f / SHADOW_MAP_SIZE.y;

    // 2x2 PCF (4 samples)
    // You can use a loop for more samples (e.g., 3x3, 4x4)
    // The offsets define the neighborhood around the projected coordinate.
    // These are half-texel offsets for a 2x2 grid centered on the original sample point.
    float2 offsets[4] =
    {
        float2(-0.5f * texelSizeX, -0.5f * texelSizeY),
        float2(0.5f * texelSizeX, -0.5f * texelSizeY),
        float2(-0.5f * texelSizeX, 0.5f * texelSizeY),
        float2(0.5f * texelSizeX, 0.5f * texelSizeY)
    };

    // If you want a more centered 2x2 or a "rotated grid" for better distribution:
    // float2 offsets[4] =
    // {
    //     float2(-1.5f * texelSizeX, -0.5f * texelSizeY),
    //     float2( 0.5f * texelSizeX, -1.5f * texelSizeY),
    //     float2(-0.5f * texelSizeX,  1.5f * texelSizeY),
    //     float2( 1.5f * texelSizeX,  0.5f * texelSizeY)
    // };
    // Or simpler, just sample at pixel corners relative to the main projCoords.xy:
    // float2 offsets[4] =
    // {
    //     float2(0,0), // Center (or slightly offset if desired)
    //     float2(texelSizeX, 0),
    //     float2(0, texelSizeY),
    //     float2(texelSizeX, texelSizeY)
    // };
    // More commonly, for a 2x2 block aligned with texels:
    // float2 baseCoord = floor(projCoords.xy * SHADOW_MAP_SIZE) / SHADOW_MAP_SIZE; // Align to texel grid
    // // Then offsets from baseCoord. This can make results more stable.

    for (int i = 0; i < 4; ++i)
    {
        // SampleCmpLevelZero returns 1.0 if not in shadow (comparison passes), 0.0 if in shadow.
        shadowFactor += shadowMap.SampleCmpLevelZero(
            samShadow,
            projCoords.xy + offsets[i],
            projCoords.z - bias // Apply bias to the current fragment's depth
        );
    }
    shadowFactor /= 4.0f; // Average the results

    return shadowFactor;
}