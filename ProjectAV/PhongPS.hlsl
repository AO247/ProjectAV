cbuffer LightCBuf
{
    float3 lightPosView; // Pozycja œwiat³a w przestrzeni widoku
    float3 lightDirView; // Kierunek œwiat³a w przestrzeni widoku (znormalizowany)
    float3 ambientColor;

    float3 diffuseColor;
    float diffuseIntensity;

    float attConst;
    float attLin;
    float attQuad;

    float spotCutoffCos;
    float spotExponent;
};

cbuffer ObjectCBuf
{
    float specularIntensity;
    float specularPower;
    float padding[2];
};

Texture2D tex;

SamplerState splr;


float4 main(float3 fragPosView : Position, float3 normalView : Normal, float2 texCoord : Texcoord) : SV_Target
{
    float3 N = normalize(normalView);
    float3 V = normalize(-fragPosView); // Kierunek do kamery (0,0,0 w view space)
    
    // Kierunek do œwiat³a i dystans
    float3 L = lightPosView - fragPosView;
    float dist = length(L);
    float3 dirToLight = L / dist;

    // Spot factor
    float spotCos = dot(-lightDirView, dirToLight);
    float spotEffect = (spotCos > spotCutoffCos) ? pow(spotCos, spotExponent) : 0.0f;

    // Attenuation
    float attenuation = 1.0f / (attConst + attLin * dist + attQuad * dist * dist);

    // Diffuse
    float diffuseFactor = max(dot(N, dirToLight), 0.0f);
    float3 diffuse = diffuseColor * diffuseIntensity * diffuseFactor * attenuation * spotEffect;

    // Specular (Blinn)
    float3 H = normalize(dirToLight + V); // Halfway vector
    float specFactor = pow(max(dot(N, H), 0.0f), specularPower);
    float3 specular = diffuseColor * specularIntensity * specFactor * attenuation * spotEffect;

    // Final color
    float3 texColor = tex.Sample(splr, texCoord).rgb;
    float3 finalColor = saturate((ambientColor + diffuse) * texColor + specular);

    return float4(finalColor, 1.0f);
}