#pragma once
#include "Renderer.h"

namespace render
{
  class CForwardRenderer : public IRenderer
  {
  public:
    CForwardRenderer(CRender* _pRender) : IRenderer(_pRender) {}
    ~CForwardRenderer() {}

  private:
  };

}