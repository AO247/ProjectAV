//#pragma once
//
//// Jolt Physics Library includes
//#include <Jolt/Renderer/DebugRenderer.h> // For JPH::DebugRenderer and its nested types
//#include <Jolt/Core/Color.h>           // For JPH::Color
//#include <Jolt/Math/RMat44.h>          // For JPH::RMat44Arg
//#include <Jolt/Math/ma>
//#include <Jolt/Math/RVec3.h>           // For JPH::RVec3Arg
//#include <Jolt/Geometry/AABox.h>       // For JPH::AABox
//#include <Jolt/Core/RefTarget.h>       // **** For JPH::RefTarget ****
//#include <Jolt/Core/RTTI.h>            // **** For JPH_DECLARE_RTTI... ****
//#include <Jolt/ObjectStream/SerializableObject.h> // **** For JPH_DECLARE_RTTI... (often needed by RTTI macros) ****
//
//// Your Engine includes
//#include "Graphics.h" // Your engine's Graphics class
//
//// Standard Library includes
//#include <DirectXMath.h>
//#include <wrl.h>      // For Microsoft::WRL::ComPtr
//#include <vector>
//#include <mutex>
//#include <memory>     // For std::unique_ptr (though m_createdBatches is removed for Ref system)
//#include <string_view>
//
//class PhysicsDebugRenderer : public JPH::DebugRenderer
//{
//public:
//    // DirectX Vertex structure for rendering
//    struct Vertex
//    {
//        DirectX::XMFLOAT3 pos;
//        DirectX::XMFLOAT4 color;
//    };
//
//    // Structure to hold D3D11 resources for a Jolt Batch, now ref-counted by Jolt
//    struct MyRenderBatchData : public JPH::RefTarget<MyRenderBatchData>
//    {
//        JPH_DECLARE_RTTI_FOR_CLASS_NOSERIALIZE(MyRenderBatchData) // Jolt RTTI macro
//            // JPH_OVERRIDE_NEW_DELETE // Optional: If Jolt's allocator should manage these objects
//
//            Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer = nullptr;
//        Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer = nullptr;
//        UINT vertexCount = 0;
//        UINT indexCount = 0;
//        UINT stride = sizeof(PhysicsDebugRenderer::Vertex);
//        D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//        bool isIndexed = false;
//
//        MyRenderBatchData() = default;
//        MyRenderBatchData(const MyRenderBatchData&) = delete;
//        MyRenderBatchData& operator=(const MyRenderBatchData&) = delete;
//    };
//
//
//    PhysicsDebugRenderer(Graphics& gfx);
//    ~PhysicsDebugRenderer() override; // Keep override, can be defaulted if no specific cleanup needed
//
//    void Initialize(); // Your D3D resource init
//
//    // --- Jolt DebugRenderer Overrides ---
//    void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override;
//    void DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, ECastShadow inCastShadow) override;
//
//    Batch CreateTriangleBatch(const Triangle* inTriangles, int inTriangleCount) override;
//    Batch CreateTriangleBatch(const JPH::DebugRenderer::Vertex* inVertices, int inVertexCount, const JPH::uint32* inIndices, int inIndexCount) override;
//
//    void DrawGeometry(
//        JPH::RMat44Arg inModelMatrix,
//        const JPH::AABox& inWorldSpaceBounds,
//        float inLODScaleSq,
//        JPH::ColorArg inModelColor,
//        const GeometryRef& inGeometry,
//        ECullMode inCullMode = ECullMode::CullBackFace,
//        ECastShadow inCastShadow = ECastShadow::On,
//        EDrawMode inDrawMode = EDrawMode::Solid
//    ) override;
//
//    void DrawText3D(JPH::RVec3Arg inPosition, const std::string_view& inString, JPH::ColorArg inColor = JPH::Color(255, 255, 255, 255), float inHeight = 0.5f) override;
//
//    // Engine-side method to render immediate-mode lines/triangles
//    void Render();
//
//private:
//    // Constant buffer structure
//    struct TransformConstantBuffer
//    {
//        DirectX::XMMATRIX mvp;
//    };
//
//    void CreateResources();
//
//    Graphics& m_gfx;
//    bool m_initialized = false;
//
//    // For immediate mode DrawLine/DrawTriangle
//    std::vector<PhysicsDebugRenderer::Vertex> m_lineVertices;
//    std::vector<PhysicsDebugRenderer::Vertex> m_triangleVertices;
//    std::mutex m_mutex;
//
//    // D3D Resources for immediate mode and general use by DrawGeometry
//    Microsoft::WRL::ComPtr<ID3D11Buffer> m_pLineVertexBuffer;
//    Microsoft::WRL::ComPtr<ID3D11Buffer> m_pTriangleVertexBuffer;
//    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout;
//    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pVertexShader;
//    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pPixelShader;
//    Microsoft::WRL::ComPtr<ID3D11Buffer> m_pConstantBuffer;
//    Microsoft::WRL::ComPtr<ID3D11BlendState> m_pBlendState;
//    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_pRasterizerState;
//
//    static constexpr UINT MAX_LINE_VERTICES = 65536;
//    static constexpr UINT MAX_TRIANGLE_VERTICES = 65536 * 3;
//
//    // m_createdBatches is NO LONGER NEEDED here if Jolt's Ref system manages lifetime.
//    // std::vector<std::unique_ptr<MyRenderBatchData>> m_createdBatches; 
//};