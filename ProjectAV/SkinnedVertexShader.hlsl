// Max number of bones, must match C++ definition
#define MAX_BONES 32

cbuffer PerFrame : register(b0) {
    matrix World;
    matrix View;
    matrix Projection;
};

cbuffer SkinningData : register(b1) {
    matrix BoneTransforms[MAX_BONES]; // Array of final skinning matrices
};

struct VertexInputType {
    float3 position : POSITION;
    float3 normal   : NORMAL;
    float2 texCoord : TEXCOORD0;
    int4 boneIDs    : BONEINDICES; // Semantic depends on your Input Layout
    float4 weights  : BONEWEIGHTS; // Semantic depends on your Input Layout
};

struct PixelInputType {
    float4 position : SV_POSITION;
    float3 normal   : NORMAL;
    float2 texCoord : TEXCOORD0;
    float3 worldPos : WORLDPOS; // Optional for lighting
};

PixelInputType main(VertexInputType input) {
    PixelInputType output = (PixelInputType)0;

    float4 totalLocalPos = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 totalNormal = float4(0.0f, 0.0f, 0.0f, 0.0f); // Normals are directions, w = 0

    // Skin the vertex position and normal
    // Note: Assumes BoneTransforms are already (GlobalPose * InverseBindPose)
    // And usually transposed for HLSL if C++ DirectXMath was row-major by default.
    // If BoneTransforms are column-major (transposed in C++):
    for (int i = 0; i < 4; ++i) {
        if (input.weights[i] > 0.0f && input.boneIDs[i] >= 0 && input.boneIDs[i] < MAX_BONES) {
            matrix boneTransform = BoneTransforms[input.boneIDs[i]];
            totalLocalPos += mul(float4(input.position, 1.0f), boneTransform) * input.weights[i];
            totalNormal += mul(float4(input.normal, 0.0f), boneTransform) * input.weights[i]; // For normals, w=0
        }
    }

    // If no weights (or issues), use original position
    if (input.weights.x == 0.0f && input.weights.y == 0.0f && input.weights.z == 0.0f && input.weights.w == 0.0f) {
        totalLocalPos = float4(input.position, 1.0f);
        totalNormal = float4(input.normal, 0.0f);
    }

    output.worldPos = mul(totalLocalPos, World).xyz;
    output.position = mul(totalLocalPos, World);
    output.position = mul(output.position, View);
    output.position = mul(output.position, Projection);

    // Transform normal to world space (remove translation part of World matrix)
    output.normal = normalize(mul(totalNormal.xyz, (float3x3) World));
    // Or if BoneTransforms already handle world transform (less common for skinning)
    // output.normal = normalize(totalNormal.xyz);

    output.texCoord = input.texCoord;

    return output;
}