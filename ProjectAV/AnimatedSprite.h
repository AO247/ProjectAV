#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <WICTextureLoader.h>  // Include for texture loading

class AnimatedSprite {
public:
    AnimatedSprite(ID3D11Device* device, int x, int y, int width, int height,
        const std::wstring& spriteSheetPath, int frameCountX, int frameCountY);
    ~AnimatedSprite();

    void Draw(ID3D11DeviceContext* context);
    void Update(float dt);  // Add an update function

    void SetPosition(int x, int y);
    void SetSize(int width, int height);
    void SetFrameTime(float frameTime); // set how long a frame lasts

private:
    struct Vertex {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
        DirectX::XMFLOAT2 uv;
    };

    ID3D11Device* device = nullptr;
    ID3D11Buffer* vertexBuffer = nullptr;
    ID3D11Buffer* indexBuffer = nullptr;
    ID3D11VertexShader* vertexShader = nullptr;
    ID3D11PixelShader* pixelShader = nullptr;
    ID3D11InputLayout* inputLayout = nullptr;
    ID3D11Buffer* constantBuffer = nullptr; // Constant buffer for both matrices

    int x, y, width, height;
    DirectX::XMFLOAT4 color;
    DirectX::XMFLOAT4X4 projectionMatrix;
    DirectX::XMFLOAT4X4 worldMatrix;

    // Animation
    ID3D11ShaderResourceView* spriteSheetTexture = nullptr;
    int frameCountX = 0;
    int frameCountY = 0;
    int currentFrame = 0;
    float frameTime = 0.1f;
    float timeElapsed = 0.0f;
    float animationWidth;
    float animationHeight;

    ID3D11SamplerState* sampler = nullptr;
    ID3D11BlendState* blendState;
};