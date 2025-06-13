struct PS_INPUT
{
    float4 clipPosition : SV_Position;
    //float3 viewNormal : NORMAL;
    float2 texCoord : TEXCOORD0;
    float3 viewPosition : TEXCOORD1;
};

float4 main(PS_INPUT input) : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}