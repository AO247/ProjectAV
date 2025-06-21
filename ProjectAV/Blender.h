// In Blender.h

#pragma once
#include "Bindable.h"
#include <array>
#include <optional>

namespace Bind
{
    class Blender : public Bindable
    {
    public:
        // +++ ADD the Mode enum for new, explicit functionality +++
        enum class Mode
        {
            Normal, // Corresponds to blending = false
            Alpha,  // Standard alpha blending
            Additive
        };

        // --- KEEP the old constructor for backward compatibility ---
        Blender(Graphics& gfx, bool blending, std::optional<float> factor = {});

        // +++ ADD the new constructor overload +++
        Blender(Graphics& gfx, Mode mode);

        void Bind(Graphics& gfx) noxnd override;

        // --- KEEP the old factor functions ---
        void SetFactor(float factor) noxnd;
        float GetFactor() const noxnd;

        // --- KEEP the old Resolve overload ---
        static std::shared_ptr<Blender> Resolve(Graphics& gfx, bool blending, std::optional<float> factor = {});

        // +++ ADD the new Resolve overload +++
        static std::shared_ptr<Blender> Resolve(Graphics& gfx, Mode mode);

        // --- KEEP the old GenerateUID overload ---
        static std::string GenerateUID(bool blending, std::optional<float> factor);

        // +++ ADD the new GenerateUID overload +++
        static std::string GenerateUID(Mode mode);

        std::string GetUID() const noexcept override;

    protected:
        Microsoft::WRL::ComPtr<ID3D11BlendState> pBlender;

        // We need to store enough information to regenerate the UID
        bool blending;
        std::optional<std::array<float, 4>> factors;
        Mode mode; // Store the mode used
    };
}