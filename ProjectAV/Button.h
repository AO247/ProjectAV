#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <memory>

class Button {
public:
    Button(ID3D11Device* device, int x, int y, int width, int height, const std::wstring& text);
    ~Button();

    void Draw(ID3D11DeviceContext* context);
    bool IsClicked(int mouseX, int mouseY);

    void SetPosition(int x, int y);
    void SetSize(int width, int height);
    void SetText(const std::wstring& text);
    void SetColor(float r, float g, float b, float a);

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
    ID3D11Buffer* constantBuffer = nullptr;

    int x, y, width, height;
    std::wstring text;
    DirectX::XMFLOAT4 color;
    DirectX::XMFLOAT4X4 projectionMatrix; // Projection matrix
    DirectX::XMFLOAT4X4 worldMatrix; // World matrix
    ID3D11ShaderResourceView* texture;
    ID3D11SamplerState* sampler;
    ID3D11BlendState* blendState;

    float textScale = 1.0f;
    float textXOffset = 0.0f;
    float textYOffset = 0.0f;
};