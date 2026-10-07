#pragma once
#include "Engine/Scenes/RenderScene.h"

namespace render { class CCamera; }
namespace render { class CRender; }

namespace render
{
  class IRenderer
  {
  public:
    IRenderer(CRender* _pRender) : m_pRender(_pRender) {}
    virtual ~IRenderer() {}

    virtual void PrepareFrame() = 0;
    virtual void Draw(scene::CRenderScene& _rRenderScene) = 0;

    inline void SetRenderCamera(CCamera* _pCamera) { m_pRenderCamera = _pCamera; }
    inline CCamera* GetRenderCamera() { return m_pRenderCamera; }

  protected:
    CRender* m_pRender = nullptr;
    CCamera* m_pRenderCamera = nullptr;
  };
};

