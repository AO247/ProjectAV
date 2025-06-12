
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

    Button(ID3D11Device* device, ID3D11DeviceContext* context, int x, int y, int width, int height, const std::wstring& text, const wchar_t* fontFilePath);
    ~Button();

    void Draw(ID3D11DeviceContext* context, float screenWidth, float screenHeight);
    bool IsClicked(int mouseX, int mouseY);

    void SetPosition(int x, int y);
    void SetSize(int width, int height);
    void SetText(const std::wstring& text);
    void SetColor(float r, float g, float b, float a);
    void SetTextColor(const DirectX::XMFLOAT4& color);


    int GetX() const { return x; }
    int GetY() const { return y; }
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }

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
    DirectX::XMFLOAT4 buttonColor; 
    DirectX::XMFLOAT4 textColor;   

    DirectX::XMFLOAT4X4 projectionMatrixStorage; 
    DirectX::XMFLOAT4X4 worldMatrixStorage;      

    std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
    std::unique_ptr<DirectX::SpriteFont> spriteFont;

    ID3D11BlendState* blendState;

    float textScale = 1.0f;

};