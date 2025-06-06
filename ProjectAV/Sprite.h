#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <WICTextureLoader.h>


class Sprite {
public:
    Sprite(ID3D11Device* device, int x, int y, int width, int height,
        const std::wstring& spritePath);
    ~Sprite();

    void Draw(ID3D11DeviceContext* context);

    void SetPosition(int x, int y);
    void SetSize(int width, int height); 

private:
    struct Vertex {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
        DirectX::XMFLOAT2 uv;
    };

    ID3D11Device* device_ = nullptr;
    ID3D11Buffer* vertexBuffer_ = nullptr;
    ID3D11Buffer* indexBuffer_ = nullptr;
    ID3D11VertexShader* vertexShader_ = nullptr;
    ID3D11PixelShader* pixelShader_ = nullptr;
    ID3D11InputLayout* inputLayout_ = nullptr;
    ID3D11Buffer* constantBuffer_ = nullptr;

    int x_ = 0;
    int y_ = 0;
    int width_ = 0;
    int height_ = 0;

    DirectX::XMFLOAT4X4 projectionMatrix_{}; // Initialized
    DirectX::XMFLOAT4X4 worldMatrix_{};       // Initialized

    ID3D11ShaderResourceView* texture_ = nullptr;
    ID3D11SamplerState* sampler_ = nullptr;
    ID3D11BlendState* blendState_ = nullptr;
};