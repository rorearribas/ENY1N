#pragma once
#include "Renderer.h"

namespace render
{
  class CLightingRenderer : public IRenderer
  {
  public:
    CLightingRenderer(CRender* _pRender) : IRenderer(_pRender) {}
    ~CLightingRenderer() {}

  };
}


