Texture2D skyboxTexture : register(t0);
SamplerState smp : register(s0);

#define PI 3.1415926535f

struct PS_INPUT
{
    float4 pos : SV_Position;
    float3 viewDir : TEXCOORD0;
};

float4 main(PS_INPUT pin) : SV_Target
{
    float3 dir = normalize(pin.viewDir);

float theta = atan2(dir.z, dir.x);
float phi = acos(dir.y);

float2 uv = float2(
    (theta / (2.0f * PI)) + 0.5f,
    phi / PI
);

return skyboxTexture.Sample(smp, uv);
}