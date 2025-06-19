// In Particle_VS.hlsl

cbuffer CBuf : register(b0)
{
    matrix viewProjection;
    float3 cameraPosition;
    float padding1; // Corresponds to 'padding' in C++ struct
    bool lockY; // Corresponds to 'lockY' in C++ struct
    // The rest of the padding is handled implicitly by HLSL packing rules
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
    
    // ===================================================================
    // +++ NEW CONDITIONAL BILLBOARDING LOGIC +++
    // ===================================================================
    
    float3 up;
    float3 right;
    float3 particleToCamera = normalize(ini.instancePos - cameraPosition);

    if (lockY)
    {
        // --- Cylindrical Billboarding (Y-axis locked) ---
        // 1. Get the direction to the camera, but only in the XZ plane.
        float3 forward_xz = particleToCamera;
        forward_xz.y = 0.0f;
        forward_xz = normalize(forward_xz);
        
        // 2. The 'up' vector is always the world's up-axis.
        up = float3(0.0f, 1.0f, 0.0f);
        
        // 3. The 'right' vector is perpendicular to the world 'up' and our new 'forward_xz'.
        right = normalize(cross(up, forward_xz));
    }
    else
    {
        // --- Spherical Billboarding (Full, original behavior) ---
        // 1. The 'up' vector is initially the world up.
        up = float3(0.0f, 1.0f, 0.0f);
        
        // 2. The 'right' vector is perpendicular to world 'up' and the direction to the camera.
        // This gives us a horizontal vector in the camera's view plane.
        right = normalize(cross(up, particleToCamera));
        
        // 3. The final 'up' vector is perpendicular to the camera direction and the 'right' vector.
        // This ensures the quad's up-axis is aligned with the camera's up-axis.
        up = cross(particleToCamera, right);
    }
    
    // ===================================================================
    // The rest of the logic remains the same, but now uses the
    // correctly calculated 'up' and 'right' vectors.
    // ===================================================================

    // Apply rotation around the facing direction
    float s, c;
    sincos(ini.instanceRot, s, c);
    
    // We can't build a 3x3 matrix directly, so we build the final vertex position
    // by combining the basis vectors. This is more efficient.
    float3 rotated_right = right * c - up * s;
    float3 rotated_up = right * s + up * c;

    // Apply rotation and scale to the quad's vertices
    float3 vertexPos = rotated_right * vsi.pos.x * ini.instanceSize.x +
                       rotated_up * vsi.pos.y * ini.instanceSize.y;

    // Add the instance's world position
    vertexPos += ini.instancePos;
    
    // Transform to clip space
    pso.pos = mul(float4(vertexPos, 1.0f), viewProjection);
    
    // Pass-through data
    pso.color = ini.instanceColor;
    pso.tc = vsi.tc;
    
    return pso;
}