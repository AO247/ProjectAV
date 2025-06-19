cbuffer DirectionalLightCBuf : register(b0)
{
    float3 viewLightDirection;
    float padding1;
    float3 dir_ambient;
    float padding2;
    float3 dir_diffuseColor;
    float dir_diffuseIntensity;

};