 
#include "Button.h"
#include <d3dcompiler.h>
#include <iostream> 
#include "DirectXColors.h" 

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
    if (errorBlob) errorBlob->Release(); 
    return S_OK;
}

Button::Button(ID3D11Device* device, ID3D11DeviceContext* context, int x, int y, int width, int height, const std::wstring& text, const wchar_t* fontFilePath)
    : device(device), x(x), y(y), width(width), height(height), text(text),
    buttonColor({ 0.5f, 0.5f, 0.5f, 1.0f }), 
    textColor(DirectX::Colors::Black.f)     
{
    HRESULT hr;

    Vertex vertices[] = {
        {{0.0f,           0.0f,            0.0f}, buttonColor, {0.0f, 1.0f}}, 
        {{(float)width,   0.0f,            0.0f}, buttonColor, {1.0f, 1.0f}}, 
        {{(float)width,   (float)height,   0.0f}, buttonColor, {1.0f, 0.0f}}, 
        {{0.0f,           (float)height,   0.0f}, buttonColor, {0.0f, 0.0f}}  
    };


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

    WORD indices[] = { 0, 1, 2, 0, 2, 3 };
    D3D11_BUFFER_DESC ibd = {};
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = sizeof(indices);
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData = {};
    iinitData.pSysMem = indices;
    hr = device->CreateBuffer(&ibd, &iinitData, &indexBuffer);
    if (FAILED(hr)) {  return; }


    const char* vertexShaderSource =
        "cbuffer MatrixBuffer : register(b0) {\n"
        "    matrix worldMatrix;\n"      
        "    matrix projectionMatrix;\n"
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
        "  float4 worldPos = mul(float4(input.Position, 1.0f), worldMatrix);\n"
        "  output.Position = mul(worldPos, projectionMatrix);\n"
        "  output.Color = input.Color;\n" 
        "  output.TexCoord = input.TexCoord;\n"
        "  return output;\n"
        "}\n";

    const char* pixelShaderSource =
        "struct PS_INPUT {\n"
        "  float4 Position : SV_POSITION;\n"
        "  float4 Color : COLOR;\n"
        "  float2 TexCoord : TEXCOORD;\n"
        "};\n"
        "float4 main(PS_INPUT input) : SV_TARGET {\n"
        "  return input.Color;\n" 
        "}\n";

    ID3DBlob* vsBlob = nullptr;
    hr = CompileShaderFromMemory(vertexShaderSource, "vs_4_0", "main", &vsBlob);
    if (FAILED(hr)) {  return; }
    hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader);
    if (FAILED(hr)) { vsBlob->Release();  return; }

    ID3DBlob* psBlob = nullptr;
    hr = CompileShaderFromMemory(pixelShaderSource, "ps_4_0", "main", &psBlob);
    if (FAILED(hr)) { vsBlob->Release();  return; }
    hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader);
    if (FAILED(hr)) { psBlob->Release(); vsBlob->Release();  return; }


    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    hr = device->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);
    vsBlob->Release();
    psBlob->Release();
    if (FAILED(hr)) {  return; }

    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    hr = device->CreateBlendState(&blendDesc, &this->blendState); 
    if (FAILED(hr)) {  return; }


    D3D11_BUFFER_DESC cbd = {};
    cbd.Usage = D3D11_USAGE_DYNAMIC;

    cbd.ByteWidth = sizeof(DirectX::XMFLOAT4X4) * 2; 
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hr = device->CreateBuffer(&cbd, nullptr, &constantBuffer);
    if (FAILED(hr)) {  return; }


    try {
        spriteBatch = std::make_unique<DirectX::SpriteBatch>(context);
        spriteFont = std::make_unique<DirectX::SpriteFont>(device, fontFilePath);

    }
    catch (const std::exception& e) {
        std::cerr << "Failed to initialize DXTK objects: " << e.what() << std::endl;

        OutputDebugStringA("DXTK Init Error: ");
        OutputDebugStringA(e.what());
        OutputDebugStringA("\n");
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
    if (blendState) blendState->Release();

}

void Button::Draw(ID3D11DeviceContext* context, float screenWidth, float screenHeight) {
    if (!vertexBuffer || !indexBuffer || !vertexShader || !pixelShader || !inputLayout || !constantBuffer || !spriteBatch || !spriteFont) {

        return;
    }


    context->IASetInputLayout(inputLayout);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);


    DirectX::XMMATRIX projection = DirectX::XMMatrixOrthographicOffCenterLH(
        0.0f, screenWidth,
        screenHeight, 0.0f, 
        0.0f, 1.0f
    );
    DirectX::XMStoreFloat4x4(&this->projectionMatrixStorage, DirectX::XMMatrixTranspose(projection));


    DirectX::XMMATRIX world = DirectX::XMMatrixTranslation((float)x, (float)y, 0.0f);
    DirectX::XMStoreFloat4x4(&this->worldMatrixStorage, DirectX::XMMatrixTranspose(world));

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = context->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (SUCCEEDED(hr)) {

        memcpy(mappedResource.pData, &this->worldMatrixStorage, sizeof(DirectX::XMFLOAT4X4));
        memcpy((char*)mappedResource.pData + sizeof(DirectX::XMFLOAT4X4), &this->projectionMatrixStorage, sizeof(DirectX::XMFLOAT4X4));
        context->Unmap(constantBuffer, 0);
    }
    else {
        std::cerr << "Failed to map constant buffer for button!" << std::endl;
        return;
    }
    context->VSSetConstantBuffers(0, 1, &constantBuffer);

    context->VSSetShader(vertexShader, nullptr, 0);
    context->PSSetShader(pixelShader, nullptr, 0);


    float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    context->OMSetBlendState(this->blendState, blendFactor, 0xffffffff);

    context->DrawIndexed(6, 0, 0);


    spriteBatch->Begin(DirectX::SpriteSortMode_Deferred, nullptr, nullptr, nullptr, nullptr, [=]() {
        });

    DirectX::XMVECTOR textSizeVec = spriteFont->MeasureString(text.c_str());
    float textWidth = DirectX::XMVectorGetX(textSizeVec);
    float textHeight = DirectX::XMVectorGetY(textSizeVec);

    DirectX::XMFLOAT2 textPosition;
    textPosition.x = (float)x + ((float)this->width - textWidth * textScale) / 2.0f;
    textPosition.y = (float)y + ((float)this->height - textHeight * textScale) / 2.0f;

    spriteFont->DrawString(
        spriteBatch.get(),
        text.c_str(),
        textPosition,
        XMLoadFloat4(&this->textColor), 
        0.0f,                     
        DirectX::XMFLOAT2{ 0,0 },     
        textScale                
    );

    spriteBatch->End();

}


bool Button::IsClicked(int mouseX, int mouseY) {
    return (mouseX >= x && mouseX <= x + this->width && mouseY >= y && mouseY <= y + this->height);
}

bool Button::IsHovered(int mouseX, int mouseY) {
    return (mouseX >= x && mouseX <= x + this->width && mouseY >= y && mouseY <= y + this->height);
}

void Button::SetPosition(int x, int y) {
    this->x = x;
    this->y = y;
}

void Button::SetSize(int newWidth, int newHeight) {
    if (this->width == newWidth && this->height == newHeight) return;

    this->width = newWidth;
    this->height = newHeight;

   
    if (vertexBuffer) {
        ID3D11DeviceContext* context = nullptr; 
        device->GetImmediateContext(&context);

        if (context) {
            D3D11_MAPPED_SUBRESOURCE mappedResource;
            if (SUCCEEDED(context->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))) {
                Vertex vertices[] = {
                    {{0.0f,             0.0f,              0.0f}, buttonColor, {0.0f, 1.0f}},
                    {{(float)this->width, 0.0f,              0.0f}, buttonColor, {1.0f, 1.0f}},
                    {{(float)this->width, (float)this->height, 0.0f}, buttonColor, {1.0f, 0.0f}},
                    {{0.0f,             (float)this->height, 0.0f}, buttonColor, {0.0f, 0.0f}}
                };
                memcpy(mappedResource.pData, vertices, sizeof(vertices));
                context->Unmap(vertexBuffer, 0);
            }
            context->Release();
        }
    }
}

void Button::SetText(const std::wstring& newText) {
    this->text = newText;
}

void Button::SetColor(float r, float g, float b, float a) {
    buttonColor = { r, g, b, a };

    if (vertexBuffer) {
        ID3D11DeviceContext* pContext = nullptr;
        device->GetImmediateContext(&pContext);
        if (pContext) {
            D3D11_MAPPED_SUBRESOURCE mappedResource;
            if (SUCCEEDED(pContext->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource))) {
                Vertex vertices[] = {
                   {{0.0f,           0.0f,            0.0f}, buttonColor, {0.0f, 1.0f}},
                   {{(float)width,   0.0f,            0.0f}, buttonColor, {1.0f, 1.0f}},
                   {{(float)width,   (float)height,   0.0f}, buttonColor, {1.0f, 0.0f}},
                   {{0.0f,           (float)height,   0.0f}, buttonColor, {0.0f, 0.0f}}
                };
                memcpy(mappedResource.pData, vertices, sizeof(vertices));
                pContext->Unmap(vertexBuffer, 0);
            }
            pContext->Release(); 
        }
    }
}

void Button::SetTextColor(const DirectX::XMFLOAT4& color) {
    this->textColor = color;
}
 