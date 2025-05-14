#include "AnimatedSprite.h"
#include <d3dcompiler.h>
#include <iostream>

// Helper function to compile shaders from memory (same as in Button.cpp)
HRESULT CompileShaderFromMemory2(const char* shaderSource, const char* profile, const char* entryPoint, ID3DBlob** shaderBlob);

AnimatedSprite::AnimatedSprite(ID3D11Device* device, int x, int y, int width, int height,
    const std::wstring& spriteSheetPath, int frameCountX, int frameCountY)
    : device(device), x(x), y(y), width(width), height(height),
    frameCountX(frameCountX), frameCountY(frameCountY) {
    HRESULT hr;

    // Calculate animation frame dimensions
    animationWidth = 1.0f / frameCountX;
    animationHeight = 1.0f / frameCountY;

    // Load sprite sheet texture
    hr = DirectX::CreateWICTextureFromFile(device, spriteSheetPath.c_str(), nullptr, &spriteSheetTexture);
    if (FAILED(hr)) {
        std::cerr << "Failed to load sprite sheet texture!" << std::endl;
        return;
    }

    // Define vertices
    Vertex vertices[] = {
        // Position, Color, UV
        {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, animationHeight}},
        {{(float)width, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {animationWidth, animationHeight}},
        {{(float)width, (float)height, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {animationWidth, 0.0f}},
        {{0.0f, (float)height, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}}
    };

    // Index Buffer
    WORD indices[] = {
        0, 1, 2,
        0, 2, 3
    };

    // Create Vertex Buffer
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(vertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

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

    // Vertex Shader Source (with world matrix)
    const char* vertexShaderSource =
        "cbuffer MatrixBuffer : register(b0) {\n"
        "    matrix projectionMatrix;\n"
        "    matrix worldMatrix;\n"
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
        "PS_INPUT main(VS_INPUT input) : SV_TARGET {\n"
        "  PS_INPUT output;\n"
        "  float4 worldPosition = mul(float4(input.Position, 1.0f), worldMatrix);\n"
        "  output.Position = mul(worldPosition, projectionMatrix);\n"
        "  output.Color = input.Color;\n"
        "  output.TexCoord = input.TexCoord;\n"
        "  return output;\n"
        "}\n";
    // Pixel Shader Source (with texture sampling)
    const char* pixelShaderSource =
        "Texture2D spriteTexture : register(t0);\n"
        "SamplerState spriteSampler : register(s0);\n"
        "struct PS_INPUT {\n"
        "  float4 Position : SV_POSITION;\n"
        "  float4 Color : COLOR;\n"
        "  float2 TexCoord : TEXCOORD;\n"
        "};\n"
        "float4 main(PS_INPUT input) : SV_TARGET {\n"
        "  return input.Color * spriteTexture.Sample(spriteSampler, input.TexCoord);\n"
        "}\n";
    // Compile Vertex Shader
    ID3DBlob* vsBlob = nullptr;
    hr = CompileShaderFromMemory2(vertexShaderSource, "vs_4_0", "main", &vsBlob);
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
    hr = CompileShaderFromMemory2(pixelShaderSource, "ps_4_0", "main", &psBlob);
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

    // Create Constant Buffer
    D3D11_BUFFER_DESC cbd = {};
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.ByteWidth = sizeof(DirectX::XMFLOAT4X4) * 2; // Projection + World
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hr = device->CreateBuffer(&cbd, nullptr, &constantBuffer);
    if (FAILED(hr)) {
        std::cerr << "Failed to create constant buffer!" << std::endl;
        return;
    }
    // Sampler State
    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    hr = device->CreateSamplerState(&samplerDesc, &sampler);
    if (FAILED(hr)) {
        std::cerr << "Failed to create sampler state!" << std::endl;
        return;
    }

    // Blend State
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
}

AnimatedSprite::~AnimatedSprite() {
    if (vertexBuffer) vertexBuffer->Release();
    if (indexBuffer) indexBuffer->Release();
    if (vertexShader) vertexShader->Release();
    if (pixelShader) pixelShader->Release();
    if (inputLayout) inputLayout->Release();
    if (constantBuffer) constantBuffer->Release();
    if (spriteSheetTexture) spriteSheetTexture->Release();
    if (sampler) sampler->Release();
    if (blendState) blendState->Release();
}

void AnimatedSprite::Update(float dt) {
    // Update animation timer
    timeElapsed += dt;

    // Check if it's time to switch frames
    if (timeElapsed >= frameTime) {
        currentFrame++; // Increment current frame
        timeElapsed -= frameTime;   // Reset timer
        if (currentFrame >= frameCountX * frameCountY) {
            currentFrame = 0;  // Loop back to the first frame
        }
    }
}

void AnimatedSprite::Draw(ID3D11DeviceContext* context) {
    // Get window dimensions
    RECT rect;
    GetClientRect(GetActiveWindow(), &rect);
    float width = rect.right - rect.left;
    float height = rect.bottom - rect.top;

    // Calculate matrices
    DirectX::XMMATRIX projectionMatrix = DirectX::XMMatrixOrthographicOffCenterLH(0.0f, width, height, 0.0f, 0.0f, 1.0f);
    DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixTranslation((float)x, (float)y, 0.0f);

    // Transpose matrices for HLSL
    DirectX::XMStoreFloat4x4(&this->projectionMatrix, DirectX::XMMatrixTranspose(projectionMatrix));
    DirectX::XMStoreFloat4x4(&this->worldMatrix, DirectX::XMMatrixTranspose(worldMatrix));

    // Update UV coordinates based on the current frame
    float uStart = (currentFrame % frameCountX) * animationWidth;
    float vStart = (currentFrame / frameCountX) * animationHeight;

    Vertex vertices[] = {
        // Position, Color, UV
        {{0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {uStart, vStart + animationHeight}},
        {{(float)width, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {uStart + animationWidth, vStart + animationHeight}},
        {{(float)width, (float)height, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {uStart + animationWidth, vStart}},
        {{0.0f, (float)height, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {uStart, vStart}}
    };

    // Map constant buffer
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = context->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(hr)) {
        std::cerr << "Failed to map constant buffer!" << std::endl;
        return;
    }
    char* dataPtr = (char*)mappedResource.pData;
    memcpy(dataPtr, &this->projectionMatrix, sizeof(DirectX::XMFLOAT4X4));
    dataPtr += sizeof(DirectX::XMFLOAT4X4);
    memcpy(dataPtr, &this->worldMatrix, sizeof(DirectX::XMFLOAT4X4));
    context->Unmap(constantBuffer, 0);

    // Update Vertex Buffer
    D3D11_MAPPED_SUBRESOURCE mappedResource2;
    HRESULT hr2 = context->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource2);
    if (FAILED(hr2)) {
        std::cerr << "Failed to map vertex buffer for UV update!" << std::endl;
        return;
    }
    memcpy(mappedResource2.pData, vertices, sizeof(vertices));
    context->Unmap(vertexBuffer, 0);

    // Set resources and states
    context->PSSetShaderResources(0, 1, &spriteSheetTexture);
    context->PSSetSamplers(0, 1, &sampler);
    context->OMSetBlendState(blendState, nullptr, 0xffffffff);

    // Set buffers and layout
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);
    context->IASetInputLayout(inputLayout);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Set shaders and constant buffer
    context->VSSetShader(vertexShader, nullptr, 0);
    context->VSSetConstantBuffers(0, 1, &constantBuffer);
    context->PSSetShader(pixelShader, nullptr, 0);

    // Draw
    context->DrawIndexed(6, 0, 0);
}

void AnimatedSprite::SetPosition(int x, int y) {
    this->x = x;
    this->y = y;
}

void AnimatedSprite::SetSize(int width, int height) {
    this->width = width;
    this->height = height;
}

void AnimatedSprite::SetFrameTime(float frameTime) {
    this->frameTime = frameTime;
}

HRESULT CompileShaderFromMemory2(const char* shaderSource, const char* profile, const char* entryPoint, ID3DBlob** shaderBlob) {
    UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    compileFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* errorBlob = nullptr;
    HRESULT hr = D3DCompile(
        shaderSource,
        strlen(shaderSource),
        nullptr,  // Optional source name for error reporting
        nullptr,  // Optional macros
        D3D_COMPILE_STANDARD_FILE_INCLUDE, // Optional include handler
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