// Button.h
#pragma once
#include <functional>
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <CommonStates.h>
#include "Mouse.h"  
#include "Window.h"

class Button
{
public:
    Button(
        const wchar_t* text,
        DirectX::XMFLOAT2 position,
        DirectX::XMFLOAT2 size,
        std::function<void()> onClick,
        ID3D11Device* device,
        const wchar_t* fontPath = L"myfile.spritefont"
    );

    DirectX::XMFLOAT2 GetPosition() const { return position; }
    DirectX::XMFLOAT2 GetSize() const { return size; }
    void Update(int mouseX, int mouseY, bool leftPressed);
    void Draw(DirectX::SpriteBatch* spriteBatch);

private:
    std::wstring text;
    DirectX::XMFLOAT2 position;
    DirectX::XMFLOAT2 size;
    std::function<void()> onClick;
    bool isHovered = false;
    bool isPressed = false;

    std::unique_ptr<DirectX::SpriteFont> font;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
};