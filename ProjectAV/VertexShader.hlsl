struct VSOut
{
    float4 color : COLOR;
	float4 pos : SV_POSITION;
};

cbuffer CBuf
{
    row_major matrix transform;
};

VSOut main( float2 pos : Position, float4 color : Color )
{
	VSOut vso;
    vso.pos = mul(float4(pos.x, pos.y, 0.0f, 1.0f), transform);
    vso.color = color;
    return vso;
}