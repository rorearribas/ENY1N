#pragma once

namespace render { class CRender; }
namespace scene { class CRenderScene; }

namespace render
{
  class IRenderer
  {
  public:
    IRenderer(CRender* _pRender) : m_pRender(_pRender) {}
    virtual ~IRenderer() {}

    virtual void Execute(scene::CRenderScene* _pRenderScene) = 0;

  protected:
    CRender* m_pRender = nullptr;
  };
};

