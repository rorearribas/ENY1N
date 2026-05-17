#pragma once
namespace render { class CRender; }

namespace render
{
  class IRenderer
  {
  public:
    IRenderer(CRender* _pRender) : m_pRender(_pRender) {}
    virtual ~IRenderer() {}

  protected:
    CRender* m_pRender = nullptr;
  };
};

