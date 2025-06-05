//#include "PhysicsDebugRenderer.h"
//#include "GraphicsThrowMacros.h"
//#include <d3dcompiler.h>
//#include <Jolt/Core/Color.h>
//#include <Jolt/Geometry/AABox.h>
//#include <Jolt/Math/Mat44.h>
//#include <Jolt/Math/Vec3.h>
//// RTTI includes are not strictly necessary if MyRenderBatchData is not used by Jolt's Ref system here
//// #include <Jolt/Core/RTTI.h>
//// #include <Jolt/ObjectStream/SerializableObject.h>
//
//// HLSL Shaders (namespace)
//// ... (as before) ...
//
//PhysicsDebugRenderer::PhysicsDebugRenderer(Graphics& gfx)
//    : m_gfx(gfx), m_initialized(false)
//{
//    m_lineVertices.reserve(MAX_LINE_VERTICES / 2);
//    m_triangleVertices.reserve(MAX_TRIANGLE_VERTICES / 2);
//}
//
//PhysicsDebugRenderer::~PhysicsDebugRenderer()
//{
//    // m_createdBatches will clean itself up if it had any elements (it won't with this approach)
//}
//
//void PhysicsDebugRenderer::Initialize()
//{
//    if (!m_initialized)
//    {
//        CreateResources();
//        m_initialized = true;
//    }
//}
//
//// CreateResources() method (as before, no changes needed for this strategy)
//// ... (full implementation of CreateResources as you have it) ...
//void PhysicsDebugRenderer::CreateResources()
//{
//    INFOMAN(m_gfx);
//    ID3D11Device* pDevice = m_gfx.GetD3DDevice();
//    HRESULT hr; // Ensure hr is declared if INFOMAN doesn't bring it. Your INFOMAN does.
//
//    // Compile Vertex Shader
//    Microsoft::WRL::ComPtr<ID3DBlob> pVSBlob;
//    Microsoft::WRL::ComPtr<ID3DBlob> pErrorBlob = nullptr;
//    UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;
//#ifndef NDEBUG
//    compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
//#else
//    compileFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
//#endif
//
//    hr = D3DCompile(g_vertexShaderHLSL, strlen(g_vertexShaderHLSL), "DebugVS_Physics", nullptr, nullptr, "main", "vs_5_0", compileFlags, 0, &pVSBlob, &pErrorBlob);
//    if (pErrorBlob) {
//        OutputDebugStringA("PhysicsDebugRenderer - Vertex Shader Compilation Log:\n");
//        OutputDebugStringA(static_cast<const char*>(pErrorBlob->GetBufferPointer()));
//        OutputDebugStringA("\n");
//    }
//    if (FAILED(hr)) {
//        infoManager.Set();
//        throw GFX_EXCEPT(hr);
//    }
//    GFX_THROW_INFO(pDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &m_pVertexShader));
//
//    D3D11_INPUT_ELEMENT_DESC ied[] = {
//        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
//        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
//    };
//    GFX_THROW_INFO(pDevice->CreateInputLayout(ied, ARRAYSIZE(ied), pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_pInputLayout));
//
//    Microsoft::WRL::ComPtr<ID3DBlob> pPSBlob;
//    pErrorBlob.Reset();
//    hr = D3DCompile(g_pixelShaderHLSL, strlen(g_pixelShaderHLSL), "DebugPS_Physics", nullptr, nullptr, "main", "ps_5_0", compileFlags, 0, &pPSBlob, &pErrorBlob);
//    if (pErrorBlob) {
//        OutputDebugStringA("PhysicsDebugRenderer - Pixel Shader Compilation Log:\n");
//        OutputDebugStringA(static_cast<const char*>(pErrorBlob->GetBufferPointer()));
//        OutputDebugStringA("\n");
//    }
//    if (FAILED(hr)) {
//        infoManager.Set();
//        throw GFX_EXCEPT(hr);
//    }
//    GFX_THROW_INFO(pDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &m_pPixelShader));
//
//    D3D11_BUFFER_DESC bd_dynamic = {};
//    bd_dynamic.Usage = D3D11_USAGE_DYNAMIC;
//    bd_dynamic.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//    bd_dynamic.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//    bd_dynamic.MiscFlags = 0;
//    bd_dynamic.StructureByteStride = sizeof(PhysicsDebugRenderer::Vertex);
//
//    bd_dynamic.ByteWidth = MAX_LINE_VERTICES * sizeof(PhysicsDebugRenderer::Vertex);
//    GFX_THROW_INFO(pDevice->CreateBuffer(&bd_dynamic, nullptr, &m_pLineVertexBuffer));
//
//    bd_dynamic.ByteWidth = MAX_TRIANGLE_VERTICES * sizeof(PhysicsDebugRenderer::Vertex);
//    GFX_THROW_INFO(pDevice->CreateBuffer(&bd_dynamic, nullptr, &m_pTriangleVertexBuffer));
//
//    D3D11_BUFFER_DESC cbd = {};
//    cbd.Usage = D3D11_USAGE_DYNAMIC;
//    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//    cbd.ByteWidth = sizeof(TransformConstantBuffer);
//    cbd.StructureByteStride = 0;
//    GFX_THROW_INFO(pDevice->CreateBuffer(&cbd, nullptr, &m_pConstantBuffer));
//
//    D3D11_BLEND_DESC blendDesc = {};
//    blendDesc.RenderTarget[0].BlendEnable = TRUE;
//    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
//    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
//    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
//    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
//    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
//    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
//    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
//    GFX_THROW_INFO(pDevice->CreateBlendState(&blendDesc, &m_pBlendState));
//
//    D3D11_RASTERIZER_DESC rasterDesc = {};
//    rasterDesc.FillMode = D3D11_FILL_SOLID;
//    rasterDesc.CullMode = D3D11_CULL_NONE;
//    rasterDesc.FrontCounterClockwise = FALSE;
//    rasterDesc.DepthClipEnable = TRUE;
//    GFX_THROW_INFO(pDevice->CreateRasterizerState(&rasterDesc, &m_pRasterizerState));
//}
//
//
//// --- "Do Nothing Safely" Implementations for Batching ---
//JPH::DebugRenderer::Batch PhysicsDebugRenderer::CreateTriangleBatch(const JPH::DebugRenderer::Triangle* /*inTriangles*/, int /*inTriangleCount*/)
//{
//    // We are not creating renderer-specific batches.
//    // Return a null batch handle.
//    // If Batch is void*, this is nullptr.
//    // If Batch is Ref<SomeBase>, this is a default-constructed (null) Ref.
//    return JPH::DebugRenderer::Batch();
//}
//
//JPH::DebugRenderer::Batch PhysicsDebugRenderer::CreateTriangleBatch(const JPH::DebugRenderer::Vertex* /*inJoltVertices*/, int /*inVertexCount*/, const JPH::uint32* /*inIndices*/, int /*inIndexCount*/)
//{
//    // We are not creating renderer-specific batches.
//    return JPH::DebugRenderer::Batch();
//}
//
//void PhysicsDebugRenderer::DrawGeometry(
//    JPH::RMat44Arg inModelMatrix,
//    const JPH::AABox& inWorldSpaceBounds,
//    float inLODScaleSq,
//    JPH::ColorArg inModelColor,
//    const JPH::DebugRenderer::GeometryRef& inGeometry,
//    JPH::DebugRenderer::ECullMode /*inCullModeParam*/,
//    JPH::DebugRenderer::ECastShadow /*inCastShadow*/,
//    JPH::DebugRenderer::EDrawMode /*inDrawModeParam*/
//) {
//    if (!m_initialized || !inGeometry) return;
//
//    const JPH::DebugRenderer::Geometry* geom = inGeometry.GetPtr();
//    if (!geom || geom->mLODs.empty()) return;
//
//    const JPH::DebugRenderer::LOD& lod = geom->mLODs[0]; // Simplified LOD selection
//
//    JPH::DebugRenderer::Batch batch_handle = lod.mBatch;
//
//    // If CreateTriangleBatch returned null (or default Batch()), then batch_handle will be null here.
//    // If batch_handle is null, we have nothing to draw for this Geometry.
//    if (!batch_handle)
//    {
//        // Optional: For debugging, draw the AABB of the geometry that we *would* have drawn.
//        // This helps confirm that DrawGeometry is being called and matrices are okay.
//        /*
//        INFOMAN(m_gfx); // For GFX_THROW_INFO
//        JPH::Vec3 corners[8];
//        inWorldSpaceBounds.GetCorners(corners);
//
//        DrawLine(JPH::RVec3(corners[0]), JPH::RVec3(corners[1]), inModelColor);
//        DrawLine(JPH::RVec3(corners[1]), JPH::RVec3(corners[3]), inModelColor);
//        DrawLine(JPH::RVec3(corners[3]), JPH::RVec3(corners[2]), inModelColor);
//        DrawLine(JPH::RVec3(corners[2]), JPH::RVec3(corners[0]), inModelColor);
//        DrawLine(JPH::RVec3(corners[4]), JPH::RVec3(corners[5]), inModelColor);
//        DrawLine(JPH::RVec3(corners[5]), JPH::RVec3(corners[7]), inModelColor);
//        DrawLine(JPH::RVec3(corners[7]), JPH::RVec3(corners[6]), inModelColor);
//        DrawLine(JPH::RVec3(corners[6]), JPH::RVec3(corners[4]), inModelColor);
//        DrawLine(JPH::RVec3(corners[0]), JPH::RVec3(corners[4]), inModelColor);
//        DrawLine(JPH::RVec3(corners[1]), JPH::RVec3(corners[5]), inModelColor);
//        DrawLine(JPH::RVec3(corners[2]), JPH::RVec3(corners[6]), inModelColor);
//        DrawLine(JPH::RVec3(corners[3]), JPH::RVec3(corners[7]), inModelColor);
//        */
//        return;
//    }
//
//    // If we had successfully created a batch and batch_handle was NOT null,
//    // here's where we would cast batch_handle to MyRenderBatchData* and draw it.
//    // But since CreateTriangleBatch returns null, this part will not be reached.
//    // MyRenderBatchData* my_batch = static_cast<MyRenderBatchData*>(batch_handle);
//    // ... then draw my_batch ...
//}
//
//void PhysicsDebugRenderer::DrawText3D(JPH::RVec3Arg /*inPosition*/, const std::string_view& /*inString*/, JPH::ColorArg /*inColor*/, float /*inHeight*/)
//{
//    // Stub - Requires a full font rendering system.
//}
//
//// DrawLine method (as before, no changes)
//// ...
//void PhysicsDebugRenderer::DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor)
//{
//    if (!m_initialized) return;
//    PhysicsDebugRenderer::Vertex v_from, v_to;
//    v_from.pos = { inFrom.GetX(), inFrom.GetY(), inFrom.GetZ() };
//    JPH::Color c = inColor;
//    if (c.a == 0 && c.r == 0 && c.g == 0 && c.b == 0) c = JPH::Color(255, 255, 255, 255); // If fully transparent black, use white
//    else if (c.a == 0) c.a = 255; // If transparent, make opaque
//    v_from.color = {
//        static_cast<float>(c.r) / 255.0f, static_cast<float>(c.g) / 255.0f,
//        static_cast<float>(c.b) / 255.0f, static_cast<float>(c.a) / 255.0f
//    };
//    v_to.pos = { inTo.GetX(), inTo.GetY(), inTo.GetZ() };
//    v_to.color = v_from.color;
//    std::lock_guard<std::mutex> lock(m_mutex);
//    if (m_lineVertices.size() + 2 <= MAX_LINE_VERTICES) {
//        m_lineVertices.push_back(v_from);
//        m_lineVertices.push_back(v_to);
//    }
//}
//
//
//// DrawTriangle method (as before, no changes)
//// ...
//void PhysicsDebugRenderer::DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, JPH::DebugRenderer::ECastShadow /*inCastShadow*/)
//{
//    if (!m_initialized) return;
//    PhysicsDebugRenderer::Vertex v1_dx, v2_dx, v3_dx;
//    JPH::Color c = inColor;
//    if (c.a == 0 && c.r == 0 && c.g == 0 && c.b == 0) c = JPH::Color(255, 255, 255, 255);
//    else if (c.a == 0) c.a = 255;
//    DirectX::XMFLOAT4 dx_color = {
//        static_cast<float>(c.r) / 255.0f, static_cast<float>(c.g) / 255.0f,
//        static_cast<float>(c.b) / 255.0f, static_cast<float>(c.a) / 255.0f
//    };
//    v1_dx.pos = { inV1.GetX(), inV1.GetY(), inV1.GetZ() }; v1_dx.color = dx_color;
//    v2_dx.pos = { inV2.GetX(), inV2.GetY(), inV2.GetZ() }; v2_dx.color = dx_color;
//    v3_dx.pos = { inV3.GetX(), inV3.GetY(), inV3.GetZ() }; v3_dx.color = dx_color;
//    std::lock_guard<std::mutex> lock(m_mutex);
//    if (m_triangleVertices.size() + 3 <= MAX_TRIANGLE_VERTICES) {
//        m_triangleVertices.push_back(v1_dx);
//        m_triangleVertices.push_back(v2_dx);
//        m_triangleVertices.push_back(v3_dx);
//    }
//}
//
//
//// Render method (as before, no changes, handles only immediate mode lines/triangles)
//// ...
//void PhysicsDebugRenderer::Render()
//{
//    if (!m_initialized) return;
//    std::vector<PhysicsDebugRenderer::Vertex> lines_to_render;
//    std::vector<PhysicsDebugRenderer::Vertex> triangles_to_render;
//    {
//        std::lock_guard<std::mutex> lock(m_mutex);
//        if (!m_lineVertices.empty()) {
//            lines_to_render.swap(m_lineVertices);
//            m_lineVertices.reserve(MAX_LINE_VERTICES / 2);
//        }
//        if (!m_triangleVertices.empty()) {
//            triangles_to_render.swap(m_triangleVertices);
//            m_triangleVertices.reserve(MAX_TRIANGLE_VERTICES / 2);
//        }
//    }
//    if (lines_to_render.empty() && triangles_to_render.empty()) {
//        return;
//    }
//    INFOMAN(m_gfx);
//    ID3D11DeviceContext* pContext = m_gfx.GetD3DContext();
//    HRESULT hr; // Ensure hr is declared for GFX_EXCEPT/GFX_THROW_INFO_ONLY if INFOMAN doesn't. Your INFOMAN does.
//    TransformConstantBuffer cbData;
//    cbData.mvp = DirectX::XMMatrixTranspose(m_gfx.GetCamera() * m_gfx.GetProjection());
//    D3D11_MAPPED_SUBRESOURCE msr_cb;
//    infoManager.Set();
//    if (FAILED(hr = pContext->Map(m_pConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr_cb))) {
//        throw GFX_EXCEPT(hr);
//    }
//    memcpy(msr_cb.pData, &cbData, sizeof(TransformConstantBuffer));
//    pContext->Unmap(m_pConstantBuffer.Get(), 0);
//    pContext->IASetInputLayout(m_pInputLayout.Get());
//    pContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
//    pContext->VSSetConstantBuffers(0, 1, m_pConstantBuffer.GetAddressOf());
//    pContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);
//    pContext->OMSetBlendState(m_pBlendState.Get(), nullptr, 0xffffffff);
//    pContext->RSSetState(m_pRasterizerState.Get());
//    pContext->OMSetDepthStencilState(nullptr, 0);
//    UINT stride = sizeof(PhysicsDebugRenderer::Vertex);
//    UINT offset = 0;
//    if (!lines_to_render.empty()) {
//        D3D11_MAPPED_SUBRESOURCE msr_vb_lines;
//        infoManager.Set();
//        if (FAILED(hr = pContext->Map(m_pLineVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr_vb_lines))) {
//            throw GFX_EXCEPT(hr);
//        }
//        memcpy(msr_vb_lines.pData, lines_to_render.data(), lines_to_render.size() * sizeof(PhysicsDebugRenderer::Vertex));
//        pContext->Unmap(m_pLineVertexBuffer.Get(), 0);
//        pContext->IASetVertexBuffers(0, 1, m_pLineVertexBuffer.GetAddressOf(), &stride, &offset);
//        pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
//        GFX_THROW_INFO_ONLY(pContext->Draw(static_cast<UINT>(lines_to_render.size()), 0));
//    }
//    if (!triangles_to_render.empty()) {
//        D3D11_MAPPED_SUBRESOURCE msr_vb_tris;
//        infoManager.Set();
//        if (FAILED(hr = pContext->Map(m_pTriangleVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &msr_vb_tris))) {
//            throw GFX_EXCEPT(hr);
//        }
//        memcpy(msr_vb_tris.pData, triangles_to_render.data(), triangles_to_render.size() * sizeof(PhysicsDebugRenderer::Vertex));
//        pContext->Unmap(m_pTriangleVertexBuffer.Get(), 0);
//        pContext->IASetVertexBuffers(0, 1, m_pTriangleVertexBuffer.GetAddressOf(), &stride, &offset);
//        pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//        GFX_THROW_INFO_ONLY(pContext->Draw(static_cast<UINT>(triangles_to_render.size()), 0));
//    }
//}