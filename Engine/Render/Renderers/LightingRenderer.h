#pragma once
#include "Renderer.h"

namespace scene { class CRenderScene; }

namespace render
{
  class CLightingRenderer : public IRenderer
  {
  public:
    CLightingRenderer(CRender* _pRender) : IRenderer(_pRender) {}
    ~CLightingRenderer() {}

    void Execute(scene::CRenderScene* _pRenderScene) override;

  private:
  };
}


