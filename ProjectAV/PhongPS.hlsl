cbuffer LightCBuf : register(b1)
{
    float3 lightDirection;
    float padding_light;

    float3 ambient;
    float3 diffuseColor;
    float diffuseIntensity;
};

cbuffer ObjectCBuf : register(b2)
{
    float specularIntensity;
    float specularPower;
    float padding_obj[2];
};

Texture2D tex;
SamplerState splr;

struct VSOut
{
    float3 viewPos : Position;
    float3 normal : Normal;
    float2 tc : Texcoord;
    float4 pos : SV_Position;
};

float4 main(VSOut vso) : SV_Target
{

    float3 n = normalize(vso.normal);

    const float3 lightVector = -lightDirection;
    
    const float diffuseFactor = max(0.0f, dot(lightVector, n));
    const float3 diffuse = diffuseColor * diffuseIntensity * diffuseFactor;

    const float3 viewDir = normalize(-vso.viewPos);

    const float3 w = n * dot(lightVector, n);
    const float3 r = normalize(w * 2.0f - lightVector);

    const float specFactor = pow(max(0.0f, dot(r, viewDir)), specularPower);
    const float3 specular = (diffuseColor * diffuseIntensity) * specularIntensity * specFactor;


    float3 texColor = tex.Sample(splr, vso.tc).rgb;

    float3 finalColor = saturate((ambient + diffuse) * texColor + specular);

    return float4(finalColor, 1.0f);
}