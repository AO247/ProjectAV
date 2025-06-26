#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <WICTextureLoader.h>

#include <vector>
#include <wrl/client.h> // For Microsoft::WRL::ComPtr

// WIC specific includes
#include <wincodec.h>
#pragma comment(lib, "windowscodecs.lib") 

class Sprite {
public:
    Sprite(ID3D11Device* device, ID3D11DeviceContext* deviceContext,
        int x, int y, int width, int height,
        const std::wstring& spritePath);
    ~Sprite();

    void Draw(ID3D11DeviceContext* context, float scale = 1.0f, DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f });

    void Update(float deltaTime);

    void SetPosition(int x, int y);
    void SetSize(int width, int height); 
    int x_ = 0;
    int y_ = 0;
    int width_ = 0;
    int height_ = 0;
private:
    struct Vertex {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
        DirectX::XMFLOAT2 uv;
    };

    static HRESULT InitializeWICFactory();  
    HRESULT LoadGifAnimation(ID3D11Device* device, const std::wstring& filePath);
    static bool IsGifExtension(const std::wstring& path);

    ID3D11Device* device_ = nullptr;
    ID3D11Buffer* vertexBuffer_ = nullptr;
    ID3D11Buffer* indexBuffer_ = nullptr;
    ID3D11VertexShader* vertexShader_ = nullptr;
    ID3D11PixelShader* pixelShader_ = nullptr;
    ID3D11InputLayout* inputLayout_ = nullptr;
    ID3D11Buffer* constantBuffer_ = nullptr;

   

    DirectX::XMFLOAT4X4 projectionMatrix_{}; 
    DirectX::XMFLOAT4X4 worldMatrix_{};       

    ID3D11ShaderResourceView* texture_ = nullptr;
    ID3D11SamplerState* sampler_ = nullptr;
    ID3D11BlendState* blendState_ = nullptr;


    bool isAnimatedGif_ = false;
    std::vector<Microsoft::WRL::ComPtr<ID3D11Texture2D>> gifFrameTextures_; // Keep textures alive
    std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> gifFrameSRVs_;
    std::vector<float> frameDelays_; // Frame delays in seconds
    unsigned int currentFrameIndex_ = 0;
    float animationTimer_ = 0.0f;

    static Microsoft::WRL::ComPtr<IWICImagingFactory> s_wicFactory;
    static bool s_wicFactoryInitialized;
    std::wstring debugSpritePath_;
};