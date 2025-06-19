#include "Sprite.h"
#include <d3dcompiler.h> 
#include <iostream>      
#include <vector>        
#include <WICTextureLoader.h> 
#include <algorithm>
Microsoft::WRL::ComPtr<IWICImagingFactory> Sprite::s_wicFactory = nullptr;
bool Sprite::s_wicFactoryInitialized = false;
#include <numeric> // For std::iota if needed, or fill canvas
#include <algorithm>

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

HRESULT Sprite::InitializeWICFactory() {
    if (s_wicFactoryInitialized) {
        return S_OK;
    }
    // Note: CoInitializeEx should be called by the application's main thread.
    // This function assumes COM has been initialized.
    HRESULT hr = CoCreateInstance(
        CLSID_WICImagingFactory,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&s_wicFactory)
    );

    if (SUCCEEDED(hr)) {
        s_wicFactoryInitialized = true;
        OutputDebugStringA("Sprite Info: WIC Imaging Factory initialized successfully.\n");
    }
    else {
        OutputDebugStringA("Sprite Error: Failed to create WIC Imaging Factory. GIF loading will fail.\n");
    }
    return hr;
}

bool Sprite::IsGifExtension(const std::wstring& path) {
    if (path.length() < 4) {
        return false;
    }
    std::wstring ext = path.substr(path.length() - 4);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::towlower); // Convert to lowercase
    return ext == L".gif";
}

Sprite::Sprite(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int x, int y, int width, int height,
    const std::wstring& spritePath)
    : device_(device), x_(x), y_(y), width_(width), height_(height), debugSpritePath_(spritePath) {
    HRESULT hr;

    OutputDebugStringW(L"Sprite Constructor START for: ");
    OutputDebugStringW(spritePath.c_str());
    OutputDebugStringW(L"\n");

    DirectX::XMStoreFloat4x4(&projectionMatrix_, DirectX::XMMatrixIdentity());
    DirectX::XMStoreFloat4x4(&worldMatrix_, DirectX::XMMatrixIdentity());

    // Initialize WIC Factory if it hasn't been already
    InitializeWICFactory();

    if (IsGifExtension(spritePath) && s_wicFactoryInitialized) {
        OutputDebugStringW(L"Sprite Info: Detected .gif, attempting to load as animation.\n");
        hr = LoadGifAnimation(device, spritePath);
        if (SUCCEEDED(hr) && !gifFrameSRVs_.empty()) {
            isAnimatedGif_ = true;
            texture_ = gifFrameSRVs_[0].Get(); // Set current texture to the first frame
            // If width/height were passed as 0, LoadGifAnimation might have updated them
            // The vertex buffer created later will use these potentially updated dimensions
            OutputDebugStringW(L"Sprite Info: GIF animation loaded successfully.\n");
        }
        else {
            OutputDebugStringW(L"Sprite Warning: Failed to load GIF as animation. Falling back to static image load.\n");
            // Fallback: Try to load as a static image (first frame)
            hr = DirectX::CreateWICTextureFromFile(device, deviceContext, spritePath.c_str(), nullptr, &texture_);
            if (FAILED(hr)) {
                // Log failure for static load
                std::vector<wchar_t> buffer(256);
                swprintf_s(buffer.data(), buffer.size(), L"Sprite Error: Fallback static texture load failed for %s. HRESULT: 0x%X\n", spritePath.c_str(), hr);
                OutputDebugStringW(buffer.data());
            }
        }
    }
    else {
        OutputDebugStringW(L"Sprite Info: Loading as static texture.\n");
        hr = DirectX::CreateWICTextureFromFile(device, deviceContext, spritePath.c_str(), nullptr, &texture_);
        if (FAILED(hr)) {
            std::vector<wchar_t> buffer(256);
            swprintf_s(buffer.data(), buffer.size(), L"Sprite Error: Failed to load sprite texture: %s HRESULT: 0x%X\n", spritePath.c_str(), hr);
            OutputDebugStringW(buffer.data());
            // texture_ will be nullptr, Draw call should handle this
        }
    }

    if (texture_ != nullptr) {
        OutputDebugStringW(L"Sprite Info: Texture resource view successfully obtained.\n");
    }
    else {
        OutputDebugStringW(L"Sprite Error: Texture resource view is NULL after loading attempts.\n");
    }


    // Vertex Buffer (uses width_ and height_ which might have been updated by GIF loading if they were 0)
    // Note: Your Draw() method re-creates vertices and updates the buffer every frame,
    // so the initial vertex values here are somewhat less critical if width_/height_ are correct by Draw time.
    Vertex vertices[] = {
        {{0.0f,           0.0f,          0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
        {{(float)width_,  0.0f,          0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
        {{(float)width_, (float)height_, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
        {{0.0f,          (float)height_, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
    };

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DYNAMIC; // Keep as DYNAMIC since Draw() updates it
    bd.ByteWidth = sizeof(vertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices;
    hr = device_->CreateBuffer(&bd, &initData, &vertexBuffer_);
    if (FAILED(hr)) { /* ... error handling ... */ return; }
    else { OutputDebugStringA("Sprite Info: Vertex buffer created.\n"); }

    // Index Buffer (remains the same)
    WORD indices[] = { 0, 1, 2, 0, 2, 3 };
    D3D11_BUFFER_DESC ibd = {};
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = sizeof(indices);
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData = {};
    iinitData.pSysMem = indices;
    hr = device_->CreateBuffer(&ibd, &iinitData, &indexBuffer_);
    if (FAILED(hr)) { /* ... error handling ... */ return; }
    else { OutputDebugStringA("Sprite Info: Index buffer created.\n"); }

    // Shaders, Input Layout, Constant Buffer, Sampler, BlendState (remain the same)
    // ... (copy the rest of your original constructor code for these D3D resources) ...
    // Make sure to use the CompileShaderFromMemory_Sprite static function.
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
    gifFrameTextures_.clear();
    gifFrameSRVs_.clear();
    frameDelays_.clear(); 
    if (!isAnimatedGif_ && texture_) {
        texture_->Release();
    }
    texture_ = nullptr; 
     
    if (blendState_) { blendState_->Release(); blendState_ = nullptr; }
    if (sampler_) { sampler_->Release(); sampler_ = nullptr; } 
    if (constantBuffer_) { constantBuffer_->Release(); constantBuffer_ = nullptr; }
    if (inputLayout_) { inputLayout_->Release(); inputLayout_ = nullptr; }
    if (pixelShader_) { pixelShader_->Release(); pixelShader_ = nullptr; }
    if (vertexShader_) { vertexShader_->Release(); vertexShader_ = nullptr; }
    if (indexBuffer_) { indexBuffer_->Release(); indexBuffer_ = nullptr; }
    if (vertexBuffer_) { vertexBuffer_->Release(); vertexBuffer_ = nullptr; }
    OutputDebugStringA("Sprite Destructor called.\n");
}
 

 
struct ColorBGRA {
    BYTE b, g, r, a;
};

HRESULT Sprite::LoadGifAnimation(ID3D11Device* device, const std::wstring& filePath) {
    if (!s_wicFactory) {
        OutputDebugStringA("LoadGifAnimation Error: WIC Factory not available.\n");
        return E_FAIL;
    }

    Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;
    HRESULT hr = s_wicFactory->CreateDecoderFromFilename(
        filePath.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder);
    if (FAILED(hr)) {
        wchar_t errorMsg[512];
        swprintf_s(errorMsg, L"LoadGifAnimation Error: CreateDecoderFromFilename failed for '%s' with HRESULT: 0x%X\n", filePath.c_str(), hr);
        OutputDebugStringW(errorMsg);
        return hr;
    }

    // --- Get GIF Global Metadata ---
    UINT logicalScreenWidth = 0, logicalScreenHeight = 0;
    ColorBGRA gifBackgroundColor = { 0, 0, 0, 0 }; // Default: transparent black
    bool hasGlobalColorTable = false;
    std::vector<WICColor> globalColorTableData; // Renamed to avoid conflict

    Microsoft::WRL::ComPtr<IWICMetadataQueryReader> gifMetadataReader;
    if (SUCCEEDED(decoder->GetMetadataQueryReader(&gifMetadataReader))) {
        PROPVARIANT propValue;
        PropVariantInit(&propValue);

        if (SUCCEEDED(gifMetadataReader->GetMetadataByName(L"/logscrdesc/Width", &propValue)) && propValue.vt == VT_UI2) {
            logicalScreenWidth = propValue.uiVal;
        } PropVariantClear(&propValue);
        if (SUCCEEDED(gifMetadataReader->GetMetadataByName(L"/logscrdesc/Height", &propValue)) && propValue.vt == VT_UI2) {
            logicalScreenHeight = propValue.uiVal;
        } PropVariantClear(&propValue);

        BYTE backgroundColorIndex = 0;
        if (SUCCEEDED(gifMetadataReader->GetMetadataByName(L"/logscrdesc/BackgroundColorIndex", &propValue)) && propValue.vt == VT_UI1) {
            backgroundColorIndex = propValue.bVal;
        } PropVariantClear(&propValue);

        if (SUCCEEDED(gifMetadataReader->GetMetadataByName(L"/logscrdesc/GlobalColorTableFlag", &propValue)) && propValue.vt == VT_BOOL) {
            hasGlobalColorTable = (propValue.boolVal == VARIANT_TRUE);
        } PropVariantClear(&propValue);

        if (hasGlobalColorTable) {
            Microsoft::WRL::ComPtr<IWICPalette> palette;
            if (SUCCEEDED(s_wicFactory->CreatePalette(&palette)) && SUCCEEDED(decoder->CopyPalette(palette.Get()))) {
                UINT colorCount = 0;
                palette->GetColorCount(&colorCount);
                if (colorCount > 0) {
                    globalColorTableData.resize(colorCount);
                    palette->GetColors(colorCount, globalColorTableData.data(), &colorCount); // Corrected variable name
                    if (backgroundColorIndex < colorCount) {
                        WICColor bgWicColor = globalColorTableData[backgroundColorIndex]; // Corrected variable name
                        gifBackgroundColor.r = (bgWicColor >> 16) & 0xFF;
                        gifBackgroundColor.g = (bgWicColor >> 8) & 0xFF;
                        gifBackgroundColor.b = bgWicColor & 0xFF;
                        gifBackgroundColor.a = (bgWicColor >> 24) & 0xFF;
                    }
                }
            }
            else {
                OutputDebugStringA("LoadGifAnimation Warning: Failed to copy global palette.\n");
            }
        }
    }
    else {
        OutputDebugStringA("LoadGifAnimation Warning: Could not get GIF global metadata reader. Using defaults.\n");
    }

    UINT frameCount = 0;
    hr = decoder->GetFrameCount(&frameCount);
    if (FAILED(hr) || frameCount == 0) {
        OutputDebugStringA("LoadGifAnimation Error: GetFrameCount failed or GIF has 0 frames.\n");
        return FAILED(hr) ? hr : E_FAIL;
    }

    wchar_t w_msg[512];
    char a_msg[256];

    swprintf_s(w_msg, L"Sprite Info: GIF '%s' has %u frames. Initial Logical Size: %ux%u\n",
        filePath.c_str(), frameCount, logicalScreenWidth, logicalScreenHeight);
    OutputDebugStringW(w_msg);

    std::vector<ColorBGRA> canvas;
    if (logicalScreenWidth > 0 && logicalScreenHeight > 0) {
        canvas.resize(static_cast<size_t>(logicalScreenWidth) * logicalScreenHeight, gifBackgroundColor);
    }

    UINT prevFrameDisposal = 0;
    UINT prevFrameLeft = 0, prevFrameTop = 0, prevFrameWidth = 0, prevFrameHeight = 0;

    gifFrameTextures_.reserve(frameCount);
    gifFrameSRVs_.reserve(frameCount);
    frameDelays_.reserve(frameCount);

    for (UINT i = 0; i < frameCount; ++i) {
        Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> wicFrame;
        hr = decoder->GetFrame(i, &wicFrame);
        if (FAILED(hr)) {
            sprintf_s(a_msg, "LoadGifAnimation Error: GetFrame failed for frame %u with HRESULT: 0x%X. Skipping frame.\n", i, hr);
            OutputDebugStringA(a_msg);
            continue;
        }

        float delaySeconds = 0.1f;
        BOOL transparencyFlag = FALSE;
        BYTE transparentColorIndex = 0;
        UINT disposalMethod = 0;
        UINT frameLeft = 0, frameTop = 0, frameWidth = 0, frameHeight = 0;

        hr = wicFrame->GetSize(&frameWidth, &frameHeight);
        if (FAILED(hr) || frameWidth == 0 || frameHeight == 0) {
            sprintf_s(a_msg, "LoadGifAnimation Error: GetSize for frame %u failed or returned zero dimensions. HRESULT: 0x%X. Skipping frame.\n", i, hr);
            OutputDebugStringA(a_msg);
            continue;
        }


        Microsoft::WRL::ComPtr<IWICMetadataQueryReader> frameMetadataReader;
        if (SUCCEEDED(wicFrame->GetMetadataQueryReader(&frameMetadataReader))) {
            PROPVARIANT propValue; PropVariantInit(&propValue);
            if (SUCCEEDED(frameMetadataReader->GetMetadataByName(L"/imgdesc/Left", &propValue)) && propValue.vt == VT_UI2) frameLeft = propValue.uiVal; PropVariantClear(&propValue);
            if (SUCCEEDED(frameMetadataReader->GetMetadataByName(L"/imgdesc/Top", &propValue)) && propValue.vt == VT_UI2) frameTop = propValue.uiVal; PropVariantClear(&propValue);

            if (SUCCEEDED(frameMetadataReader->GetMetadataByName(L"/grctlext/Delay", &propValue)) && propValue.vt == VT_UI2) delaySeconds = std::max(0.01f, static_cast<float>(propValue.uiVal) / 100.0f); PropVariantClear(&propValue);            if (SUCCEEDED(frameMetadataReader->GetMetadataByName(L"/grctlext/Disposal", &propValue)) && propValue.vt == VT_UI1) disposalMethod = propValue.bVal; PropVariantClear(&propValue);
            if (SUCCEEDED(frameMetadataReader->GetMetadataByName(L"/grctlext/TransparencyFlag", &propValue)) && propValue.vt == VT_BOOL) transparencyFlag = propValue.boolVal; PropVariantClear(&propValue);
            if (transparencyFlag && SUCCEEDED(frameMetadataReader->GetMetadataByName(L"/grctlext/TransparentColorIndex", &propValue)) && propValue.vt == VT_UI1) transparentColorIndex = propValue.bVal; PropVariantClear(&propValue);
        }
        else {
            sprintf_s(a_msg, "LoadGifAnimation Warning: Could not get metadata reader for frame %u.\n", i);
            OutputDebugStringA(a_msg);
        }

        frameDelays_.push_back(delaySeconds);

        if (canvas.empty()) {
            if (logicalScreenWidth == 0 || logicalScreenHeight == 0) {
                logicalScreenWidth = frameLeft + frameWidth;
                logicalScreenHeight = frameTop + frameHeight;
                if (logicalScreenWidth == 0 || logicalScreenHeight == 0) {
                    OutputDebugStringA("LoadGifAnimation Error: Cannot determine canvas size from first valid frame. Aborting.\n"); return E_FAIL;
                }
                swprintf_s(w_msg, L"Canvas size dynamically set from frame %d: %ux%u\n", i, logicalScreenWidth, logicalScreenHeight);
                OutputDebugStringW(w_msg);
            }
            canvas.resize(static_cast<size_t>(logicalScreenWidth) * logicalScreenHeight, gifBackgroundColor);
        }

        if (i > 0) {
            if (prevFrameDisposal == 2) {
                sprintf_s(a_msg, "Frame %d: Prev frame disposal was 2 (Restore BG). Area: L%u T%u W%u H%u\n", i, prevFrameLeft, prevFrameTop, prevFrameWidth, prevFrameHeight);
                OutputDebugStringA(a_msg);
                for (UINT y_disp = 0; y_disp < prevFrameHeight; ++y_disp) { // Renamed loop variables
                    for (UINT x_disp = 0; x_disp < prevFrameWidth; ++x_disp) {
                        UINT canvasX = prevFrameLeft + x_disp;
                        UINT canvasY = prevFrameTop + y_disp;
                        if (canvasX < logicalScreenWidth && canvasY < logicalScreenHeight) {
                            canvas[static_cast<size_t>(canvasY) * logicalScreenWidth + canvasX] = gifBackgroundColor;
                        }
                    }
                }
            }
            else if (prevFrameDisposal == 3) {
                OutputDebugStringA("LoadGifAnimation Warning: Disposal method 3 (Restore Previous) not fully implemented. Treating as Do Not Dispose.\n");
            }
        }

        Microsoft::WRL::ComPtr<IWICFormatConverter> converter;
        hr = s_wicFactory->CreateFormatConverter(&converter);
        if (FAILED(hr)) {
            sprintf_s(a_msg, "LoadGifAnimation Error: CreateFormatConverter for frame %u failed with HRESULT: 0x%X. Skipping frame.\n", i, hr);
            OutputDebugStringA(a_msg);
            continue;
        }

        hr = converter->Initialize(wicFrame.Get(), GUID_WICPixelFormat32bppBGRA, WICBitmapDitherTypeNone, nullptr, 0.f, WICBitmapPaletteTypeMedianCut);
        if (FAILED(hr)) {
            sprintf_s(a_msg, "LoadGifAnimation Warning: Initialize to BGRA for frame %u failed. Trying PBGRA. HRESULT: 0x%X.\n", i, hr);
            OutputDebugStringA(a_msg);
            hr = converter->Initialize(wicFrame.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.f, WICBitmapPaletteTypeMedianCut);
            if (FAILED(hr)) {
                sprintf_s(a_msg, "LoadGifAnimation Error: Initialize to PBGRA for frame %u also failed. HRESULT: 0x%X. Skipping frame.\n", i, hr);
                OutputDebugStringA(a_msg);
                continue;
            }
        }

        std::vector<BYTE> framePixelBuffer(static_cast<size_t>(frameWidth) * frameHeight * 4);
        hr = converter->CopyPixels(nullptr, frameWidth * 4, static_cast<UINT>(framePixelBuffer.size()), framePixelBuffer.data());
        if (FAILED(hr)) {
            sprintf_s(a_msg, "LoadGifAnimation Error: CopyPixels for frame %u failed with HRESULT: 0x%X. Skipping frame.\n", i, hr);
            OutputDebugStringA(a_msg);
            continue;
        }

        sprintf_s(a_msg, "Frame %d: Compositing. TransFlag: %s, TransIdx: %u. Frame L%u T%u W%u H%u\n",
            i, transparencyFlag ? "TRUE" : "FALSE", transparentColorIndex, frameLeft, frameTop, frameWidth, frameHeight);
        OutputDebugStringA(a_msg);

        for (UINT y_comp = 0; y_comp < frameHeight; ++y_comp) { // Renamed loop variables
            for (UINT x_comp = 0; x_comp < frameWidth; ++x_comp) {
                UINT canvasX = frameLeft + x_comp;
                UINT canvasY = frameTop + y_comp;

                if (canvasX < logicalScreenWidth && canvasY < logicalScreenHeight) {
                    size_t framePixelIdx = (static_cast<size_t>(y_comp) * frameWidth + x_comp) * 4;
                    BYTE b = framePixelBuffer[framePixelIdx + 0];
                    BYTE g = framePixelBuffer[framePixelIdx + 1];
                    BYTE r = framePixelBuffer[framePixelIdx + 2];
                    BYTE a_from_wic = framePixelBuffer[framePixelIdx + 3];

                    if (!transparencyFlag || a_from_wic != 0) {
                        size_t canvasIdx = static_cast<size_t>(canvasY) * logicalScreenWidth + canvasX;
                        canvas[canvasIdx].b = b;
                        canvas[canvasIdx].g = g;
                        canvas[canvasIdx].r = r;
                        canvas[canvasIdx].a = 255;
                    }
                }
            }
        }

        std::vector<BYTE> d3dPixelBuffer(static_cast<size_t>(logicalScreenWidth) * logicalScreenHeight * 4);
        for (size_t k = 0; k < canvas.size(); ++k) {
            d3dPixelBuffer[k * 4 + 0] = canvas[k].b;
            d3dPixelBuffer[k * 4 + 1] = canvas[k].g;
            d3dPixelBuffer[k * 4 + 2] = canvas[k].r;
            d3dPixelBuffer[k * 4 + 3] = 0xFF;
        }

        if (i == 0 && (width_ == 0 || height_ == 0) && logicalScreenWidth > 0 && logicalScreenHeight > 0) {
            width_ = logicalScreenWidth;
            height_ = logicalScreenHeight;
            sprintf_s(a_msg, "Sprite dimensions set to logical screen: %d x %d\n", width_, height_);
            OutputDebugStringA(a_msg);
        }

        D3D11_TEXTURE2D_DESC texDesc = {};
        texDesc.Width = logicalScreenWidth;
        texDesc.Height = logicalScreenHeight;
        texDesc.MipLevels = 1; texDesc.ArraySize = 1; texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        texDesc.SampleDesc.Count = 1; texDesc.SampleDesc.Quality = 0; texDesc.Usage = D3D11_USAGE_IMMUTABLE;
        texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        D3D11_SUBRESOURCE_DATA subresourceData = {};
        subresourceData.pSysMem = d3dPixelBuffer.data();
        subresourceData.SysMemPitch = logicalScreenWidth * 4;

        Microsoft::WRL::ComPtr<ID3D11Texture2D> d3dFrameTexture;
        hr = device->CreateTexture2D(&texDesc, &subresourceData, &d3dFrameTexture);
        if (FAILED(hr)) {
            sprintf_s(a_msg, "LoadGifAnimation Error: CreateTexture2D for frame %u failed with HRESULT: 0x%X. Skipping frame.\n", i, hr);
            OutputDebugStringA(a_msg);
            continue;
        }
        gifFrameTextures_.push_back(d3dFrameTexture);

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = texDesc.Format; srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0; srvDesc.Texture2D.MipLevels = 1;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> frameSRV;
        hr = device->CreateShaderResourceView(d3dFrameTexture.Get(), &srvDesc, &frameSRV);
        if (FAILED(hr)) {
            sprintf_s(a_msg, "LoadGifAnimation Error: CreateShaderResourceView for frame %u failed with HRESULT: 0x%X. Skipping frame.\n", i, hr);
            OutputDebugStringA(a_msg);
            gifFrameTextures_.pop_back(); // Remove the texture if SRV creation failed
            continue;
        }
        gifFrameSRVs_.push_back(frameSRV);

        if (i == 0 && !canvas.empty() && d3dPixelBuffer.size() >= 8) {
            if (canvas.size() > 1 && d3dPixelBuffer.size() > 8) { // Check canvas has at least 2 pixels
                sprintf_s(a_msg, "Canvas Frame 0, Pixel 0 (Final D3D BGRA): %02X %02X %02X %02X || P1: %02X %02X %02X %02X\n",
                    d3dPixelBuffer[0], d3dPixelBuffer[1], d3dPixelBuffer[2], d3dPixelBuffer[3],
                    d3dPixelBuffer[4], d3dPixelBuffer[5], d3dPixelBuffer[6], d3dPixelBuffer[7]);
                OutputDebugStringA(a_msg);
            }
        }

        prevFrameDisposal = disposalMethod;
        prevFrameLeft = frameLeft;
        prevFrameTop = frameTop;
        prevFrameWidth = frameWidth;
        prevFrameHeight = frameHeight;
    }

    if (gifFrameSRVs_.empty()) {
        OutputDebugStringA("LoadGifAnimation Error: No frames were successfully loaded into SRVs after processing all frames.\n");
        return E_FAIL;
    }
    return S_OK;
}
void Sprite::Update(float deltaTime) {
    if (isAnimatedGif_ && !gifFrameSRVs_.empty() && !frameDelays_.empty() && gifFrameSRVs_.size() == frameDelays_.size()) {
        animationTimer_ += deltaTime;
        if (currentFrameIndex_ < frameDelays_.size() && animationTimer_ >= frameDelays_[currentFrameIndex_]) {
            animationTimer_ -= frameDelays_[currentFrameIndex_]; // Consume time for this frame
            currentFrameIndex_ = (currentFrameIndex_ + 1) % gifFrameSRVs_.size();
            texture_ = gifFrameSRVs_[currentFrameIndex_].Get(); // Switch to next frame's texture
        }
    }
}

void Sprite::Draw(ID3D11DeviceContext* context) {

    if (!texture_) {
        OutputDebugStringW(L"Sprite::Draw() Error: texture_ is NULL. Cannot draw sprite: ");
        OutputDebugStringW(debugSpritePath_.c_str());
        OutputDebugStringW(L"\n");
        return; // Critical resource missing
    }

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
            }
        }
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