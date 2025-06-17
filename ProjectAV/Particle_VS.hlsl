cbuffer CBuf
{
    matrix view;
};

struct VSIn
{
    float3 pos : Position;
    float2 tex : TexCoord;
    float3 instancePos : InstancePos;
    float2 instanceSize : InstanceSize;
    float4 instanceColor : InstanceColor;
};

struct VSOut
{
    float4 pos : SV_Position;
    float4 color : Color;
    float2 tex : TexCoord;
};

VSOut main(VSIn input)
{
    VSOut output;
    
    // Billboarding logic
    // Zero out the rotation part of the view matrix
    matrix billboardView = view;
    billboardView[0][0] = 1.0f;
    billboardView[0][1] = 0.0f;
    billboardView[0][2] = 0.0f;
    billboardView[1][0] = 0.0f;
    billboardView[1][1] = 1.0f;
    billboardView[1][2] = 0.0f;
    billboardView[2][0] = 0.0f;
    billboardView[2][1] = 0.0f;
    billboardView[2][2] = 1.0f;
    
    float3 worldPos = mul(float4(input.pos, 1.0f), billboardView).xyz;
    worldPos.xy *= input.instanceSize;
    worldPos += input.instancePos;
    
    output.pos = float4(worldPos, 1.0f);
    output.color = input.instanceColor;
    output.tex = input.tex;
    
    return output;
}