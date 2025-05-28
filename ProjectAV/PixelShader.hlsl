Texture2D diffuseTexture : register(t0);
SamplerState basicSampler : register(s0);

struct PS_INPUT // Output to Pixel Shader
{
    float4 clipPosition : SV_Position; // Vertex position in clip space
    float3 viewNormal : NORMAL; // Normal in view space (for view-space lighting)
    float2 texCoord : TEXCOORD0; // Texture coordinates
    float3 viewPosition : TEXCOORD1; // Vertex position in view space (for view-space lighting/effects)
};
float4 main(PS_INPUT input) : SV_Target
{
    return float4(1.0f, 0.0f, 1.0f, 1.0f); // Solid magenta
    //return diffuseTexture.Sample(basicSampler, input.texCoord); // If you have a texture
}