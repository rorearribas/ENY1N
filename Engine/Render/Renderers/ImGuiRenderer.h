#pragma once
#include "Renderer.h"
#include "Engine/Render/RenderContext/RenderDeviceDX11.h"
#include "Engine/Render/RenderContext/RenderCommandsDX11.h"

namespace render
{
  class CImGuiRenderer : public IRenderer
  {
  public:
    CImGuiRenderer(CRender* _pRender) : IRenderer(_pRender) {}
    ~CImGuiRenderer() = default;

    HRESULT Init(const render::CRenderDeviceDX11& _rRenderDevice, const render::CRenderCommandsDX11& _rCommands, const HWND& _hWnd);

    void PrepareFrame() override;
    void Draw(scene::CRenderScene& /*_rRenderScene*/) override;
  };
}

