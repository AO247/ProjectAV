cbuffer ObjectVars : register(b2)
{
    float4x4 LocalToWorld4x4;
    float4x4 LocalToProjected4x4;
    float4x4 WorldToLocal4x4;
    float4x4 WorldToView4x4;
    float4x4 UVTransform4x4;
    float3 EyePosition;
};

struct A2V
{
    float4 pos : POSITION0;
    float3 normal : NORMAL0;
    float4 tangent : TANGENT0;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
};

struct V2P
{
    float4 pos : SV_POSITION;
    float4 diffuse : COLOR;
    float2 uv : TEXCOORD0;
    float3 worldNorm : TEXCOORD1;
    float3 worldPos : TEXCOORD2;
    float3 toEye : TEXCOORD3;
    float4 tangent : TEXCOORD4;
    float3 normal : TEXCOORD5;
};

V2P main(A2V input)
{
    V2P output;

    // transform position to clip space
    output.pos = mul(input.pos, LocalToProjected4x4);

    // transform to world space for lighting calculations
    float4 worldPos = mul(input.pos, LocalToWorld4x4);
    output.worldPos = worldPos.xyz;

    // pass UV (with optional transformation)
    float4 transformedUV = mul(float4(input.uv, 0.0f, 1.0f), UVTransform4x4);
    output.uv = transformedUV.xy;

    // transform normal and tangent to world space
    float3 worldNormal = normalize(mul(float4(input.normal, 0.0f), LocalToWorld4x4).xyz);
    float3 worldTangent = normalize(mul(float4(input.tangent.xyz, 0.0f), LocalToWorld4x4).xyz);
    float tangentW = input.tangent.w;

    output.worldNorm = worldNormal;
    output.normal = input.normal;
    output.tangent = float4(worldTangent, tangentW);

    // to eye vector (world space)
    output.toEye = normalize(EyePosition - worldPos.xyz);

    // pass vertex color if needed (here just assigning it, can be used in PS)
    output.diffuse = input.color;

    return output;
}
