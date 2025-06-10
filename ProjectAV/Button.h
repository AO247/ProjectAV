// --- START OF FILE Button.h ---
#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <memory>
 
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <CommonStates.h> 

class Button {
public:
    // Constructor now needs device context for SpriteBatch and font path
    Button(ID3D11Device* device, ID3D11DeviceContext* context, int x, int y, int width, int height, const std::wstring& text, const wchar_t* fontFilePath);
    ~Button();

    void Draw(ID3D11DeviceContext* context, float screenWidth, float screenHeight);
    bool IsClicked(int mouseX, int mouseY);

    void SetPosition(int x, int y);
    void SetSize(int width, int height);
    void SetText(const std::wstring& text);
    void SetColor(float r, float g, float b, float a);
    void SetTextColor(const DirectX::XMFLOAT4& color); // Method to set text color


    int GetX() const { return x; }
    int GetY() const { return y; }
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }

private:
    struct Vertex {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color; // Color for the button background
        DirectX::XMFLOAT2 uv;
    };

    ID3D11Device* device = nullptr;
    ID3D11Buffer* vertexBuffer = nullptr;
    ID3D11Buffer* indexBuffer = nullptr;
    ID3D11VertexShader* vertexShader = nullptr;
    ID3D11PixelShader* pixelShader = nullptr;
    ID3D11InputLayout* inputLayout = nullptr;
    ID3D11Buffer* constantBuffer = nullptr; // For VS projection/world matrices
    // ID3D11Buffer* colorConstantBuffer = nullptr; // For PS button color (Alternative to vertex color)


    int x, y, width, height;
    std::wstring text;
    DirectX::XMFLOAT4 buttonColor; // Color for the button background vertices
    DirectX::XMFLOAT4 textColor;   // Color for the text

    DirectX::XMFLOAT4X4 projectionMatrixStorage; // To store projection matrix for VS
    DirectX::XMFLOAT4X4 worldMatrixStorage;      // To store world matrix for VS

    // DirectX Tool Kit members
    std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
    std::unique_ptr<DirectX::SpriteFont> spriteFont;
    // std::unique_ptr<DirectX::CommonStates> commonStates; // Optional, if you need specific states

    ID3D11BlendState* blendState; // For button quad transparency

    // Removed unused texture/sampler as the current button is solid color
    // ID3D11ShaderResourceView* texture;
    // ID3D11SamplerState* sampler;

    // Text rendering properties (can be expanded)
    float textScale = 1.0f;
    // textXOffset and textYOffset can be used for fine-tuning, or calculated dynamically
};
// --- END OF FILE Button.h ---