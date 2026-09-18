#pragma once
#include "Renderer.h"

namespace render
{
  class CLightingRenderer : public IRenderer
  {
  public:
    CLightingRenderer(CRender* _pRender) : IRenderer(_pRender) {}
    ~CLightingRenderer() = default;

    void PrepareFrame() override {}
    void Execute(scene::CRenderScene& _rRenderScene) override;
    HRESULT Init(uint32_t _uWidth, uint32_t _uHeight);

    const CCamera& GetShadowCamera() const { return *m_pShadowCamera; }

  private:
    ID3D11RasterizerState* m_pShadowsRasterizer = nullptr;
    CCamera* m_pShadowCamera = nullptr;
  };
}


