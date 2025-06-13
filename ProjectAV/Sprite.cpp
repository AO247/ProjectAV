#include "Sprite.h"
#include <d3dcompiler.h> 
#include <iostream>      
#include <vector>        
#include <WICTextureLoader.h> 


static HRESULT CompileShaderFromMemory_Sprite(const char* shaderSource, const char* profile, const char* entryPoint, ID3DBlob** shaderBlob) {
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
            OutputDebugStringA(static_cast<char*>(errorBlob->GetBufferPointer()));
            std::cerr << "Shader compilation error: " << static_cast<char*>(errorBlob->GetBufferPointer()) << std::endl;
            errorBlob->Release();
        }
        else {
            std::vector<char> buffer(128);
            sprintf_s(buffer.data(), buffer.size(), "Shader compilation failed with HRESULT: 0x%X\n", hr);
            OutputDebugStringA(buffer.data());
            std::cerr << "Shader compilation failed with HRESULT: " << std::hex << hr << std::endl;
        }
        return hr;
    }
    if (errorBlob) {
        OutputDebugStringA(static_cast<char*>(errorBlob->GetBufferPointer())); 
        errorBlob->Release();
    }
    return S_OK;
}


Sprite::Sprite(ID3D11Device* device, int x, int y, int width, int height,
    const std::wstring& spritePath)
    : device_(device), x_(x), y_(y), width_(width), height_(height) {
    HRESULT hr;

    OutputDebugStringW(L"Sprite Constructor START for: ");
    OutputDebugStringW(spritePath.c_str());
    OutputDebugStringW(L"\n");

    DirectX::XMStoreFloat4x4(&projectionMatrix_, DirectX::XMMatrixIdentity());
    DirectX::XMStoreFloat4x4(&worldMatrix_, DirectX::XMMatrixIdentity());

    hr = DirectX::CreateWICTextureFromFile(device_, spritePath.c_str(), nullptr, &texture_);
    if (FAILED(hr)) {
        std::vector<wchar_t> buffer(256);
        swprintf_s(buffer.data(), buffer.size(), L"Sprite Error: Failed to load sprite texture: %s HRESULT: 0x%X\n", spritePath.c_str(), hr);
        OutputDebugStringW(buffer.data());
        std::wcerr << L"Sprite Error: Failed to load sprite texture: " << spritePath.c_str() << L" HRESULT: 0x" << std::hex << hr << std::endl;
    }
    else {
        std::vector<wchar_t> buffer(256);
        swprintf_s(buffer.data(), buffer.size(), L"Sprite Info: Successfully loaded texture: %s\n", spritePath.c_str());
        OutputDebugStringW(buffer.data());
    }

    Vertex vertices[] = {
        {{0.0f,           0.0f,          0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
        {{(float)width_,  0.0f,          0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
        {{(float)width_, (float)height_, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
        {{0.0f,          (float)height_, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
    };

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(vertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices;
    hr = device_->CreateBuffer(&bd, &initData, &vertexBuffer_);
    if (FAILED(hr)) {
        OutputDebugStringA("Sprite Error: Failed to create vertex buffer!\n");
        std::cerr << "Failed to create vertex buffer for Sprite! HRESULT: " << std::hex << hr << std::endl;
        return;
    }
    else {
        OutputDebugStringA("Sprite Info: Vertex buffer created.\n");
    }


    WORD indices[] = { 0, 1, 2, 0, 2, 3 };

    D3D11_BUFFER_DESC ibd = {};
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = sizeof(indices);
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData = {};
    iinitData.pSysMem = indices;
    hr = device_->CreateBuffer(&ibd, &iinitData, &indexBuffer_);
    if (FAILED(hr)) {
        OutputDebugStringA("Sprite Error: Failed to create index buffer!\n");
        std::cerr << "Failed to create index buffer for Sprite! HRESULT: " << std::hex << hr << std::endl;
        return;
    }
    else {
        OutputDebugStringA("Sprite Info: Index buffer created.\n");
    }


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
        "PS_INPUT main(VS_INPUT input) {\n"
        "  PS_INPUT output;\n"
        "  float4 worldPosition = mul(float4(input.Position, 1.0f), worldMatrix);\n"
        "  output.Position = mul(worldPosition, projectionMatrix);\n"
        "  output.Color = input.Color;\n"
        "  output.TexCoord = input.TexCoord;\n"
        "  return output;\n"
        "}\n";

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

    ID3DBlob* vsBlob = nullptr;
    hr = CompileShaderFromMemory_Sprite(vertexShaderSource, "vs_4_0", "main", &vsBlob);
    if (FAILED(hr)) { OutputDebugStringA("Sprite Error: Failed to compile VS (Texture Version)!\n"); return; }
    else { OutputDebugStringA("Sprite Info: VS (Texture Version) compiled.\n"); }

    hr = device_->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader_);
    if (FAILED(hr)) {
        OutputDebugStringA("Sprite Error: Failed to create VS object (Texture Version)!\n");
        if (vsBlob) vsBlob->Release(); return;
    }
    else { OutputDebugStringA("Sprite Info: VS object (Texture Version) created.\n"); }

    ID3DBlob* psBlob = nullptr;
    hr = CompileShaderFromMemory_Sprite(pixelShaderSource, "ps_4_0", "main", &psBlob);
    if (FAILED(hr)) {
        OutputDebugStringA("Sprite Error: Failed to compile PS (Texture Version)!\n");
        if (vsBlob) vsBlob->Release(); return;
    }
    else { OutputDebugStringA("Sprite Info: PS (Texture Version) compiled.\n"); }

    hr = device_->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader_);
    if (FAILED(hr)) {
        OutputDebugStringA("Sprite Error: Failed to create PS object (Texture Version)!\n");
        if (psBlob) psBlob->Release();
        if (vsBlob) vsBlob->Release();
        return;
    }
    else { OutputDebugStringA("Sprite Info: PS object (Texture Version) created.\n"); }

    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    hr = device_->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout_);
    if (FAILED(hr)) {
        OutputDebugStringA("Sprite Error: Failed to create input layout (Texture Version)!\n");
        if (psBlob) psBlob->Release();
        if (vsBlob) vsBlob->Release();
        return;
    }
    else { OutputDebugStringA("Sprite Info: Input layout (Texture Version) created.\n"); }

    if (psBlob) psBlob->Release();
    if (vsBlob) vsBlob->Release();


    D3D11_BUFFER_DESC cbd = {};
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.ByteWidth = sizeof(DirectX::XMFLOAT4X4) * 2;
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hr = device_->CreateBuffer(&cbd, nullptr, &constantBuffer_);
    if (FAILED(hr)) { OutputDebugStringA("Sprite Error: Failed to create constant buffer!\n"); return; }
    else { OutputDebugStringA("Sprite Info: Constant buffer created.\n"); }


    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = device_->CreateSamplerState(&samplerDesc, &sampler_);
    if (FAILED(hr)) { OutputDebugStringA("Sprite Error: Failed to create sampler state!\n"); return; }
    else { OutputDebugStringA("Sprite Info: Sampler state created.\n"); }


    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    hr = device_->CreateBlendState(&blendDesc, &blendState_);
    if (FAILED(hr)) { OutputDebugStringA("Sprite Error: Failed to create blend state!\n"); return; }
    else { OutputDebugStringA("Sprite Info: Blend state created.\n"); }

    OutputDebugStringW(L"Sprite Constructor END for: ");
    OutputDebugStringW(spritePath.c_str());
    OutputDebugStringW(L"\n");
}

Sprite::~Sprite() {
    if (blendState_) { blendState_->Release(); blendState_ = nullptr; }
    if (sampler_) { sampler_->Release(); sampler_ = nullptr; }
    if (texture_) { texture_->Release(); texture_ = nullptr; }
    if (constantBuffer_) { constantBuffer_->Release(); constantBuffer_ = nullptr; }
    if (inputLayout_) { inputLayout_->Release(); inputLayout_ = nullptr; }
    if (pixelShader_) { pixelShader_->Release(); pixelShader_ = nullptr; }
    if (vertexShader_) { vertexShader_->Release(); vertexShader_ = nullptr; }
    if (indexBuffer_) { indexBuffer_->Release(); indexBuffer_ = nullptr; }
    if (vertexBuffer_) { vertexBuffer_->Release(); vertexBuffer_ = nullptr; }
    OutputDebugStringA("Sprite Destructor called.\n");
}

void Sprite::Draw(ID3D11DeviceContext* context) {


    if (!context || !vertexBuffer_ || !indexBuffer_ || !vertexShader_ || !pixelShader_ || !inputLayout_ || !constantBuffer_ || !texture_ || !sampler_ || !blendState_) {
        OutputDebugStringA("Sprite::Draw() Error: Missing critical D3D resources (Texture Version). Aborting draw.\n");
        if (!context) OutputDebugStringA("Context is NULL\n");
        if (!vertexBuffer_) OutputDebugStringA("vertexBuffer_ is NULL\n");
        if (!indexBuffer_) OutputDebugStringA("indexBuffer_ is NULL\n");
        if (!vertexShader_) OutputDebugStringA("vertexShader_ is NULL\n");
        if (!pixelShader_) OutputDebugStringA("pixelShader_ is NULL\n");
        if (!inputLayout_) OutputDebugStringA("inputLayout_ is NULL\n");
        if (!constantBuffer_) OutputDebugStringA("constantBuffer_ is NULL\n");
        if (!texture_) OutputDebugStringA("texture_ is NULL - Texture likely failed to load or was released!\n");
        if (!sampler_) OutputDebugStringA("sampler_ is NULL!\n");
        if (!blendState_) OutputDebugStringA("blendState_ is NULL!\n");
        return;
    }

    float windowClientWidth = 1280.0f;
    float windowClientHeight = 720.0f;
    HWND activeWindow = GetActiveWindow();
    bool gotDimensionsFromWindow = false;

    if (activeWindow) {
        RECT clientRect;
        if (GetClientRect(activeWindow, &clientRect)) {
            if (clientRect.right - clientRect.left > 0 && clientRect.bottom - clientRect.top > 0) {
                windowClientWidth = static_cast<float>(clientRect.right - clientRect.left);
                windowClientHeight = static_cast<float>(clientRect.bottom - clientRect.top);
                gotDimensionsFromWindow = true;
            }
            else {
                OutputDebugStringA("Sprite::Draw - GetClientRect returned zero/negative dimensions. Using fallback.\n");
            }
        }
        else {
            DWORD error = GetLastError();
            std::vector<char> buffer(256);
            sprintf_s(buffer.data(), buffer.size(), "Sprite::Draw - GetClientRect failed. Error: %lu. Using fallback.\n", error);
            OutputDebugStringA(buffer.data());
        }
    }
    else {
        //OutputDebugStringA("Sprite::Draw - GetActiveWindow() returned NULL. Using fallback.\n");
    }




    DirectX::XMMATRIX projMat = DirectX::XMMatrixOrthographicOffCenterLH(0.0f, windowClientWidth, windowClientHeight, 0.0f, 0.0f, 1.0f);
    DirectX::XMStoreFloat4x4(&this->projectionMatrix_, DirectX::XMMatrixTranspose(projMat));

    DirectX::XMMATRIX worldMat = DirectX::XMMatrixTranslation(static_cast<float>(x_), static_cast<float>(y_), 0.0f);
    DirectX::XMStoreFloat4x4(&this->worldMatrix_, DirectX::XMMatrixTranspose(worldMat));


    Vertex vertices[] = {
        {{0.0f,            0.0f,           0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
        {{(float)width_,   0.0f,           0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
        {{(float)width_,  (float)height_,  0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
        {{0.0f,           (float)height_,  0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
    };
    D3D11_MAPPED_SUBRESOURCE mappedResourceVB;
    HRESULT hrVB = context->Map(vertexBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResourceVB);
    if (SUCCEEDED(hrVB)) {
        memcpy(mappedResourceVB.pData, vertices, sizeof(vertices));
        context->Unmap(vertexBuffer_, 0);
    }
    else {
        OutputDebugStringA("Sprite::Draw - Failed to map vertex buffer for update!\n");
        return;
    }


    D3D11_MAPPED_SUBRESOURCE mappedResourceCB;
    HRESULT hrCB = context->Map(constantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResourceCB);
    if (FAILED(hrCB)) {
        OutputDebugStringA("Sprite::Draw - Failed to map constant buffer!\n");
        return;
    }
    struct MatrixBufferType { DirectX::XMFLOAT4X4 projection; DirectX::XMFLOAT4X4 world; };
    MatrixBufferType* cbPtr = static_cast<MatrixBufferType*>(mappedResourceCB.pData);
    cbPtr->projection = projectionMatrix_;
    cbPtr->world = worldMatrix_;
    context->Unmap(constantBuffer_, 0);


    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &vertexBuffer_, &stride, &offset);
    context->IASetIndexBuffer(indexBuffer_, DXGI_FORMAT_R16_UINT, 0);
    context->IASetInputLayout(inputLayout_);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    context->VSSetShader(vertexShader_, nullptr, 0);
    context->VSSetConstantBuffers(0, 1, &constantBuffer_);

    context->PSSetShader(pixelShader_, nullptr, 0);
    context->PSSetShaderResources(0, 1, &texture_);
    context->PSSetSamplers(0, 1, &sampler_);

    float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    context->OMSetBlendState(blendState_, blendFactor, 0xffffffff);

    context->DrawIndexed(6, 0, 0);

}

void Sprite::SetPosition(int x, int y) {
    x_ = x;
    y_ = y;
}

void Sprite::SetSize(int width, int height) {
    width_ = width;
    height_ = height;
}