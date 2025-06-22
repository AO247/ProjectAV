// =================================================================================
// Particle_VS.hlsl
//
// This vertex shader handles instanced, billboarding particles.
// It supports:
// - Spherical billboarding (always facing the camera).
// - Cylindrical billboarding (Y-axis locked, for effects like smoke).
// - Per-particle rotation.
// - Texture atlas for randomized particle appearance.
// =================================================================================


// Constant buffer matching the C++ ParticleCbuf struct.
// It's updated once per particle system, per frame.
cbuffer CBuf : register(b0)
{
    matrix viewProjection; // Combined view and projection matrix from the camera.
    float3 cameraPosition; // World-space position of the camera.
    float padding1; // Ensures alignment.
    bool lockY; // If true, use cylindrical billboarding.
    float2 atlasSize; // Stores { 1.0f/numAtlasCols, 1.0f/numAtlasRows }.
    float padding2; // Ensures alignment.
};

// Input from the static vertex buffer (the quad).
// This is the same for every single particle.
struct VSInput
{
    float3 pos : POSITION; // Local position of a quad vertex (e.g., -0.5, -0.5, 0).
    float2 tc : TEXCOORD; // Local texture coordinate (e.g., 0, 1).
};

// Input from the instance buffer.
// This is unique for each active particle.
struct InstanceInput
{
    float3 instancePos : INSTANCE_POS; // World position of the particle.
    float4 instanceColor : INSTANCE_COLOR; // Color/alpha, interpolated over the particle's lifetime.
    float2 instanceSize : INSTANCE_SIZE; // Size, interpolated over the particle's lifetime.
    float instanceRot : INSTANCE_ROT; // Rotation, interpolated over the particle's lifetime.
    float2 atlasOffset : ATLAS_OFFSET; // UV offset for the top-left corner of this particle's sub-texture.
};

// Data passed from the vertex shader to the pixel shader.
struct PSInput
{
    float4 color : COLOR; // Final interpolated particle color.
    float2 tc : TEXCOORD; // Final calculated texture coordinates for the atlas.
    float4 pos : SV_POSITION; // Final position in clip space (required output).
};


PSInput main(VSInput vsi, InstanceInput ini)
{
    PSInput pso;
    
    // 1. DETERMINE BILLBOARDING BASIS VECTORS (up, right, forward)
    // ----------------------------------------------------------------
    float3 up;
    float3 right;
    // Calculate the normalized direction from the particle to the camera.
    float3 particleToCamera = normalize(ini.instancePos - cameraPosition);

    if (lockY)
    {
        // --- Cylindrical Billboarding (Y-axis locked) ---
        // The particle stands upright but rotates to face the camera.
        
        // The "forward" vector is the direction to the camera, flattened onto the XZ plane.
        float3 forward_xz = particleToCamera;
        forward_xz.y = 0.0f;
        forward_xz = normalize(forward_xz);
        
        // The "up" vector is always the world's up-axis.
        up = float3(0.0f, 1.0f, 0.0f);
        
        // The "right" vector is perpendicular to the world 'up' and our new 'forward_xz'.
        right = normalize(cross(up, forward_xz));
    }
    else
    {
        // --- Spherical Billboarding (Full, camera-facing) ---
        // The particle is a flat plane that always perfectly faces the camera.

        // Start with the world's up vector.
        up = float3(0.0f, 1.0f, 0.0f);
        
        // Create a 'right' vector that is horizontal in the camera's view.
        right = normalize(cross(up, particleToCamera));
        
        // Create the final 'up' vector that is perpendicular to the 'right' vector and the
        // direction to the camera, making it aligned with the camera's vertical axis.
        up = cross(particleToCamera, right);
    }
    
    // 2. CALCULATE FINAL VERTEX POSITION
    // ----------------------------------------------------------------
    
    // Apply the per-particle 2D rotation to the basis vectors.
    float s, c;
    sincos(ini.instanceRot, s, c);
    
    float3 rotated_right = right * c - up * s;
    float3 rotated_up = right * s + up * c;

    // Calculate the final world position of the quad's vertex by scaling the
    // rotated basis vectors and adding the particle's instance position.
    float3 vertexPos = rotated_right * vsi.pos.x * ini.instanceSize.x +
                       rotated_up * vsi.pos.y * ini.instanceSize.y;
    vertexPos += ini.instancePos;
    
    // Transform the final world position into clip space for the rasterizer.
    pso.pos = mul(float4(vertexPos, 1.0f), viewProjection);
    
    // 3. CALCULATE FINAL TEXTURE COORDINATES
    // ----------------------------------------------------------------
    
    // Scale the quad's base UVs (0-1) to the size of a single atlas cell,
    // then add the offset to select the correct cell for this particle.
    pso.tc = (vsi.tc * atlasSize) + ini.atlasOffset;
    
    // 4. PASS-THROUGH DATA
    // ----------------------------------------------------------------
    
    // The color is already calculated on the CPU, so just pass it along.
    pso.color = ini.instanceColor;
    
    return pso;
}