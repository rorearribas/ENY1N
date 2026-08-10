#pragma once
#include "Engine/Render/Resources/RenderTarget.h"
#include "Renderer.h"

namespace scene { class CRenderScene; }

namespace render
{
  class CDeferredRenderer : public IRenderer
  {
  public:
    CDeferredRenderer(CRender* _pRender) : IRenderer(_pRender) {}
    ~CDeferredRenderer() {}

    HRESULT Init(uint32_t _uWidth, uint32_t _uHeight);
    void Execute(scene::CRenderScene& _rRenderScene) override;

    void DrawOpaques(void) {}
    void ComputeGraphicsBuffer(void) {}

    void AttachRenderTargets(render::texture::TDepthStencil& _rDepthStencil);
    void DetachRenderTargets();

    void ClearRenderTargets(const float _v4ClearColor[4]);
    CRenderTarget& GetDiffuseRT() const { return *m_pDiffuseRT; }
    CRenderTarget& GetNormalRT() const { return *m_pNormalRT; }
    CRenderTarget& GetSpecularRT() const { return *m_pSpecularRT; }

  private:
    HRESULT SetupRenderTargets(uint32_t _uWidth, uint32_t _uHeight);
    void Release();

  private:
    std::unique_ptr<CRenderTarget> m_pDiffuseRT;
    std::unique_ptr<CRenderTarget> m_pNormalRT;
    std::unique_ptr<CRenderTarget> m_pSpecularRT;
  };
}


