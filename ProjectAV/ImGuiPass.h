#include "RenderQueuePass.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "App.h"

#pragma once
namespace Rgph {
    class ImGuiPass : public RenderQueuePass {
    public:
        ImGuiPass(std::string name, Graphics& gfx) : RenderQueuePass(std::move(name)) {
            using namespace Bind;
            RegisterSink(DirectBufferSink<RenderTarget>::Make("renderTarget", renderTarget));
            RegisterSink(DirectBufferSink<DepthStencil>::Make("depthStencil", depthStencil));
            RegisterSource(DirectBufferSource<RenderTarget>::Make("renderTarget", renderTarget));
            RegisterSource(DirectBufferSource<DepthStencil>::Make("depthStencil", depthStencil));
            AddBind(Stencil::Resolve(gfx, Stencil::Mode::Off));
        }
        void Execute(Graphics& gfx) const override {
            renderTarget->BindAsBuffer(gfx, depthStencil.get());
            ImGui::Render();
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        }
    };
}