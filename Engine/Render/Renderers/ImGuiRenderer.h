#pragma once
#include "Renderer.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <cstdint>
#include <d3d11.h>

namespace render
{
  class CImGuiRenderer : public IRenderer
  {
  public:
    CImGuiRenderer(CRender* _pRender) : IRenderer(_pRender) {}
    ~CImGuiRenderer() = default;

    HRESULT Init(const HWND& _hWnd);

    void PrepareFrame() override;
    void Execute(scene::CRenderScene& /*_rRenderScene*/) override;
  };
}

