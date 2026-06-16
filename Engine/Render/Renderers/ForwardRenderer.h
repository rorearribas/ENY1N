#pragma once
#include "Renderer.h"

namespace render
{
  class CForwardRenderer : public IRenderer
  {
  public:
    CForwardRenderer(CRender* _pRender) : IRenderer(_pRender) {}
    ~CForwardRenderer() {}

    void Execute(scene::CRenderScene* _pRenderScene) override;

  private:
  };

}