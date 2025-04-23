// Button.cpp
#include "Button.h"

#include <WICTextureLoader.h>

Button::Button(
    const wchar_t* text,
    DirectX::XMFLOAT2 position,
    DirectX::XMFLOAT2 size,
    std::function<void()> onClick,
    ID3D11Device* device,
    const wchar_t* fontPath
) : text(text), position(position), size(size), onClick(onClick)
{
    // Load font
    font = std::make_unique<DirectX::SpriteFont>(device, fontPath);

    // Create a simple texture for the button
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = 1;
    desc.Height = 1;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    uint32_t pixelData = 0xFFFFFFFF; // White pixel
    D3D11_SUBRESOURCE_DATA initData = { &pixelData, 4, 0 };

    Microsoft::WRL::ComPtr<ID3D11Texture2D> tex;
    device->CreateTexture2D(&desc, &initData, &tex);
    device->CreateShaderResourceView(tex.Get(), nullptr, &texture);
}

void Button::Update(int mouseX, int mouseY, bool leftPressed)
{
    isHovered =
        mouseX >= position.x &&
        mouseX <= position.x + size.x &&
        mouseY >= position.y &&
        mouseY <= position.y + size.y;

    if (isHovered && leftPressed)
    {
        if (!isPressed)
        {
            onClick();
        }
        isPressed = true;
    }
    else
    {
        isPressed = false;
    }
}

void Button::Draw(DirectX::SpriteBatch* spriteBatch)
{
    if (!texture || !font) return;

    // Draw button background
    DirectX::XMVECTOR color = isPressed ?
        DirectX::Colors::Gray :
        (isHovered ? DirectX::Colors::LightGray : DirectX::Colors::White);

    spriteBatch->Draw(
        texture.Get(),
        position,
        nullptr,
        color,
        0.0f,
        DirectX::XMFLOAT2(0, 0),
        size
    );

    // Calculate text position (centered)
    auto textSize = font->MeasureString(text.c_str());
    DirectX::XMFLOAT2 textPos = {
        position.x + (size.x - textSize.m128_f32[0]) / 2,
        position.y + (size.y - textSize.m128_f32[1]) / 2
    };

    // Draw button text
    font->DrawString(
        spriteBatch,
        text.c_str(),
        textPos,
        DirectX::Colors::Black
    );
}