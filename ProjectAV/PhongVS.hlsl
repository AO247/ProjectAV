cbuffer CBuf
{
    matrix modelView;
    matrix modelViewProj;
};

struct VSIn
{
    float3 pos : Position;
    float3 n : Normal;
    float2 tc : Texcoord;
};

struct VSOut
{
    float3 viewPos : Position;
    float3 normal : Normal;
    float2 tc : Texcoord;
    float4 pos : SV_Position;
};

VSOut main(VSIn vsi)
{
    VSOut vso;
    vso.viewPos = (float3) mul(float4(vsi.pos, 1.0f), modelView);
    vso.normal = normalize(mul(vsi.n, (float3x3) modelView));
    vso.pos = mul(float4(vsi.pos, 1.0f), modelViewProj);
    vso.tc = vsi.tc;
    return vso;
}

