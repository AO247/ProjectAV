// cbuffer for Model-View and Model-View-Projection matrices
// These are column-major in the cbuffer due to Transpose in C++
cbuffer PerObjectTransforms : register(b0)
{
    matrix modelView; // From C++: Transpose(CharacterWorld * CameraView)
    matrix modelViewProj; // From C++: Transpose(CharacterWorld * CameraView * CameraProjection)
};

// cbuffer for Bone Transformation matrices
// These are column-major in the cbuffer due to Transpose in C++
cbuffer BoneTransforms : register(b3) // Assuming you use slot b3 for bones
{
    matrix finalBoneMatrices[128]; // Array size should match/exceed MAX_SHADER_BONES_CB in C++
};

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL; // <<< ADDED for lighting
    float2 texCoord : TEXCOORD0;
    int4 boneIDs : BLENDINDICES0; // <<< ADDED for skinning
    float4 boneWeights : BLENDWEIGHT0; // <<< ADDED for skinning
};

struct PS_INPUT // Output to Pixel Shader
{
    float4 clipPosition : SV_Position; // Vertex position in clip space
    float3 viewNormal : NORMAL; // Normal in view space (for view-space lighting)
    float2 texCoord : TEXCOORD0; // Texture coordinates
    float3 viewPosition : TEXCOORD1; // Vertex position in view space (for view-space lighting/effects)
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;

    float4 originalModelPos = float4(input.position, 1.0f);
    float4 originalModelNormal = float4(input.normal, 0.0f); // w=0 for vectors (normals)

    float4 skinnedModelPos = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 skinnedModelNormal = float4(0.0f, 0.0f, 0.0f, 0.0f);

    float totalWeight = input.boneWeights.x + input.boneWeights.y + input.boneWeights.z + input.boneWeights.w;

    // Perform skinning if weights are significant
    if (totalWeight > 0.0001f)
    {
        for (int i = 0; i < 4; ++i) // Assuming MAX_BONES_PER_VERTEX is 4
        {
            if (input.boneWeights[i] > 0.00001f)
            {
                // finalBoneMatrices[boneID] = Transpose(OffsetMatrix * GlobalAnimatedBoneTransform)
                // This boneTransform is column-major.
                matrix boneTransform = finalBoneMatrices[input.boneIDs[i]];

                // originalModelPos is a row vector for this mul convention
                skinnedModelPos += input.boneWeights[i] * mul(originalModelPos, boneTransform);
                skinnedModelNormal += input.boneWeights[i] * mul(originalModelNormal, boneTransform);
            }
        }
    }
    else
    {
        // Fallback for unweighted vertices (or if totalWeight is ~0)
        // Default to being transformed by bone 0.
        // Ensure bone 0's matrix is valid (e.g., identity for T-pose if this fallback is hit).
        matrix defaultBoneTransform = finalBoneMatrices[0];
        skinnedModelPos = mul(originalModelPos, defaultBoneTransform);
        skinnedModelNormal = mul(originalModelNormal, defaultBoneTransform);
    }

    // skinnedModelPos and skinnedModelNormal are now in animated model space.

    // Transform position to clip space using the pre-combined ModelViewProjection matrix
    // modelViewProj is Transpose(World*View*Projection) (column-major)
    // output.clipPosition = skinnedModelPos_row_vector * modelViewProj_column_major
    output.clipPosition = mul(skinnedModelPos, modelViewProj);

    // For lighting, transform position and normal to view space using modelView matrix
    // modelView is Transpose(World*View) (column-major)
    // output.viewPosition = skinnedModelPos_row_vector * modelView_column_major
    output.viewPosition = mul(skinnedModelPos, modelView).xyz;
    // output.viewNormal = normalize(skinnedModelNormal_row_vector * (float3x3)modelView_column_major).xyz
    output.viewNormal = normalize(mul(skinnedModelNormal.xyz, (float3x3) modelView));

    output.texCoord = input.texCoord;

    return output;
}