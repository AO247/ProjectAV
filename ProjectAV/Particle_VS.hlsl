cbuffer CBuf : register(b0)
{
    matrix viewProjection;
    float3 cameraPosition;
};

// Per-vertex data for the quad
struct VSInput
{
    float3 pos : POSITION;
    float2 tc : TEXCOORD;
};

// Per-instance data for each particle
struct InstanceInput
{
    float3 instancePos : INSTANCE_POS;
    float4 instanceColor : INSTANCE_COLOR;
    float2 instanceSize : INSTANCE_SIZE;
    float instanceRot : INSTANCE_ROT;
};

struct PSInput
{
    float4 color : COLOR;
    float2 tc : TEXCOORD;
    float4 pos : SV_POSITION;
};

PSInput main(VSInput vsi, InstanceInput ini)
{
    PSInput pso;
    
    // Billboard calculation
    // CORRECTED: Access instancePos via the 'ini' input parameter
    float3 particleToCamera = normalize(ini.instancePos - cameraPosition);
    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 right = normalize(cross(up, particleToCamera));
    up = cross(particleToCamera, right);

    // Apply rotation around the facing direction (particleToCamera axis)
    float s, c;
    sincos(ini.instanceRot, s, c);
    
    float3x3 rotMat;
    rotMat[0] = right * c - up * s;
    rotMat[1] = right * s + up * c;
    rotMat[2] = particleToCamera;

    // Apply rotation and scale to the quad's vertices
    float3 vertexPos = vsi.pos.x * rotMat[0] * ini.instanceSize.x +
                       vsi.pos.y * rotMat[1] * ini.instanceSize.y;

    // Add the instance's world position
    // CORRECTED: Access instancePos via the 'ini' input parameter
    vertexPos += ini.instancePos;
    
    // Transform to clip space
    pso.pos = mul(float4(vertexPos, 1.0f), viewProjection);
    
    // Pass-through data
    pso.color = ini.instanceColor;
    pso.tc = vsi.tc;
    
    return pso;
}