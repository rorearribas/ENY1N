#pragma once
#include "Renderer.h"

namespace render
{
  class CShadowRenderer : public IRenderer
  {
  public:
    CShadowRenderer(CRender* _pRender) : IRenderer(_pRender) {}
    ~CShadowRenderer() = default;

    HRESULT Init(uint32_t _uWidth, uint32_t _uHeight);

    void PrepareFrame() override {}
    void Draw(scene::CRenderScene& _rRenderScene) override;

  private:
    ID3D11RasterizerState* m_pShadowsRasterizer = nullptr;
  };
}