#include "Button.h"
#include <d3dcompiler.h>
#include <iostream>
#include <WICTextureLoader.h>
#include <SpriteFont.h>
#include <SpriteBatch.h>

// Helper function to compile shaders from memory
HRESULT CompileShaderFromMemory(const char* shaderSource, const char* profile, const char* entryPoint, ID3DBlob** shaderBlob) {
    UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    compileFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* errorBlob = nullptr;
    HRESULT hr = D3DCompile(
        shaderSource,
        strlen(shaderSource),
        nullptr,
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        entryPoint,
        profile,
        compileFlags,
        0,
        shaderBlob,
        &errorBlob
    );

    if (FAILED(hr)) {
        if (errorBlob) {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }
        return hr;
    }

    return S_OK;
}

Button::Button(ID3D11Device* device, int x, int y, int width, int height, const std::wstring& text)
    : device(device), x(x), y(y), width(width), height(height), text(text), color({ 1.0f, 1.0f, 1.0f, 1.0f }) {
    HRESULT hr;

    // Vertex data for a rectangle (button)
    Vertex vertices[] = {
        //Position              Color                  UV
        {{0.0f, 0.0f, 0.0f}, {0.5f, 0.5f, 0.5f, 1.0f}, {0.0f, 1.0f}}, // Top-Left (Gray)
        {{(float)width, 0.0f, 0.0f}, {0.5f, 0.5f, 0.5f, 1.0f}, {1.0f, 1.0f}}, // Top-Right (Gray)
        {{(float)width, (float)height, 0.0f}, {0.5f, 0.5f, 0.5f, 1.0f}, {1.0f, 0.0f}}, // Bottom-Right (Gray)
        {{0.0f, (float)height, 0.0f}, {0.5f, 0.5f, 0.5f, 1.0f}, {0.0f, 0.0f}}  // Bottom-Left (Gray)
    };

    //Index buffer data
    WORD indices[] = {
        0,  1,  2,
        0,  2,  3
    };

    // Create Vertex Buffer
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(vertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices;
    hr = device->CreateBuffer(&bd, &initData, &vertexBuffer);
    if (FAILED(hr)) {
        std::cerr << "Failed to create vertex buffer!" << std::endl;
        return;
    }

    // Create Index Buffer
    D3D11_BUFFER_DESC ibd = {};
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = sizeof(indices);
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA iinitData = {};
    iinitData.pSysMem = indices;
    hr = device->CreateBuffer(&ibd, &iinitData, &indexBuffer);
    if (FAILED(hr)) {
        std::cerr << "Failed to create index buffer!" << std::endl;
        return;
    }

    // Vertex Shader Source (HLSL inlined as a string)
    const char* vertexShaderSource =
        "cbuffer MatrixBuffer : register(b0) {\n"  // Constant buffer for the projection matrix
        "    matrix projectionMatrix;\n"
        "    matrix worldMatrix; // Add world matrix here\n"
        "};\n"
        "struct VS_INPUT {\n"
        "  float3 Position : POSITION;\n"
        "  float4 Color : COLOR;\n"
        "  float2 TexCoord : TEXCOORD;\n"
        "};\n"
        "struct PS_INPUT {\n"
        "  float4 Position : SV_POSITION;\n"
        "  float4 Color : COLOR;\n"
        "  float2 TexCoord : TEXCOORD;\n"
        "};\n"
        "PS_INPUT main(VS_INPUT input) {\n"
        "  PS_INPUT output;\n"
        "  float4 worldPosition = mul(float4(input.Position, 1.0f), worldMatrix);\n" // Apply world matrix here
        "  output.Position = mul(worldPosition, projectionMatrix);\n" // Apply projection matrix
        "  output.Color = input.Color;\n"
        "  output.TexCoord = input.TexCoord;\n"
        "  return output;\n"
        "}\n";

    // Pixel Shader Source (HLSL inlined as a string)
    const char* pixelShaderSource =
        "struct PS_INPUT {\n"
        "  float4 Position : SV_POSITION;\n"
        "  float4 Color : COLOR;\n"
        "  float2 TexCoord : TEXCOORD;\n"
        "};\n"
        "float4 main(PS_INPUT input) : SV_TARGET {\n"
        "  return input.Color;\n"
        "}\n";

    // Compile Vertex Shader
    ID3DBlob* vsBlob = nullptr;
    hr = CompileShaderFromMemory(vertexShaderSource, "vs_4_0", "main", &vsBlob);
    if (FAILED(hr)) {
        std::cerr << "Failed to compile vertex shader!" << std::endl;
        return;
    }

    // Create Vertex Shader
    hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader);
    if (FAILED(hr)) {
        std::cerr << "Failed to create vertex shader!" << std::endl;
        vsBlob->Release();
        return;
    }

    // Compile Pixel Shader
    ID3DBlob* psBlob = nullptr;
    hr = CompileShaderFromMemory(pixelShaderSource, "ps_4_0", "main", &psBlob);
    if (FAILED(hr)) {
        std::cerr << "Failed to compile pixel shader!" << std::endl;
        vsBlob->Release();
        return;
    }

    // Create Pixel Shader
    hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader);
    if (FAILED(hr)) {
        std::cerr << "Failed to create pixel shader!" << std::endl;
        psBlob->Release();
        vsBlob->Release();
        return;
    }

    // Define Input Layout
    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    // Create Input Layout
    hr = device->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);
    if (FAILED(hr)) {
        std::cerr << "Failed to create input layout!" << std::endl;
        psBlob->Release();
        vsBlob->Release();
        return;
    }

    // Release shader blobs
    psBlob->Release();
    vsBlob->Release();

    // Create Blend State
    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    hr = device->CreateBlendState(&blendDesc, &blendState);
    if (FAILED(hr)) {
        std::cerr << "Failed to create blend state!" << std::endl;
        return;
    }

    // Create Constant Buffer for Projection Matrix
    D3D11_BUFFER_DESC cbd = {};
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.ByteWidth = sizeof(DirectX::XMFLOAT4X4) * 2;
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hr = device->CreateBuffer(&cbd, nullptr, &constantBuffer);
    if (FAILED(hr)) {
        std::cerr << "Failed to create constant buffer!" << std::endl;
        return;
    }
}

Button::~Button() {
    if (vertexBuffer) vertexBuffer->Release();
    if (indexBuffer) indexBuffer->Release();
    if (vertexShader) vertexShader->Release();
    if (pixelShader) pixelShader->Release();
    if (inputLayout) inputLayout->Release();
    if (constantBuffer) constantBuffer->Release();
}

void Button::Draw(ID3D11DeviceContext* context) {
    // Get window dimensions
    RECT rect;
    GetClientRect(GetActiveWindow(), &rect);
    float width = rect.right - rect.left;
    float height = rect.bottom - rect.top;

    // Calculate the projection matrix
    DirectX::XMMATRIX projectionMatrix = DirectX::XMMatrixOrthographicOffCenterLH(
        0.0f, width,
        height, 0.0f,
        0.0f, 1.0f
    );
    DirectX::XMStoreFloat4x4(&this->projectionMatrix, DirectX::XMMatrixTranspose(projectionMatrix));

    // Calculate the world matrix
    DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixTranslation((float)x, (float)y, 0.0f);
    DirectX::XMStoreFloat4x4(&this->worldMatrix, DirectX::XMMatrixTranspose(worldMatrix));

    // Map constant buffer
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = context->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(hr)) {
        std::cerr << "Failed to map constant buffer!" << std::endl;
        return;
    }

    // Copy the projection matrix to the mapped memory
    memcpy(mappedResource.pData, &this->projectionMatrix, sizeof(DirectX::XMFLOAT4X4));
    // Offset the pointer and copy the world matrix
    memcpy((char*)mappedResource.pData + sizeof(DirectX::XMFLOAT4X4), &this->worldMatrix, sizeof(DirectX::XMFLOAT4X4));

    // Unmap the constant buffer
    context->Unmap(constantBuffer, 0);

    // Set constant buffer to vertex shader
    context->VSSetConstantBuffers(0, 1, &constantBuffer);

    // Set Vertex Buffer
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

    // Set Index Buffer
    context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);

    // Set Input Layout
    context->IASetInputLayout(inputLayout);

    // Set Primitive Topology
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Set Shaders
    context->VSSetShader(vertexShader, nullptr, 0);
    context->PSSetShader(pixelShader, nullptr, 0);

    //Set Blend State
    float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    context->OMSetBlendState(blendState, blendFactor, 0xffffffff);

    // Draw
    context->DrawIndexed(6, 0, 0);
}

bool Button::IsClicked(int mouseX, int mouseY) {
    return (mouseX >= x && mouseX <= x + width && mouseY >= y && mouseY <= y + height);
}

void Button::SetPosition(int x, int y) {
    this->x = x;
    this->y = y;
}

void Button::SetSize(int width, int height) {
    this->width = width;
    this->height = height;
}

void Button::SetText(const std::wstring& text) {
    this->text = text;
}

void Button::SetColor(float r, float g, float b, float a)
{
    color.x = r;
    color.y = g;
    color.z = b;
    color.w = a;
}