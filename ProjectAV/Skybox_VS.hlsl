cbuffer CBuf
{
    matrix view;
    matrix projection;
};

struct VS_INPUT
{
    float3 pos : POSITION;
};

struct VS_TO_PS
{
    float4 pos : SV_Position;
    float3 tex : TEXCOORD0;
};

VS_TO_PS main(VS_INPUT vin)
{
    VS_TO_PS vout;

    matrix view_no_trans = view;
    view_no_trans[3] = float4(0.0f, 0.0f, 0.0f, 1.0f);

    float4 pos = float4(vin.pos, 1.0f);
    pos = mul(pos, view_no_trans);
    pos = mul(pos, projection);

    vout.pos = pos.xyww;
    vout.tex = vin.pos;

    return vout;
}