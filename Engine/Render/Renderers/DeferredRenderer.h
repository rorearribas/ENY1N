#pragma once
#include "Engine/Render/Resources/RenderTarget.h"

namespace render
{
  class CDeferredRenderer
  {
  public:
    CDeferredRenderer() = default;
    ~CDeferredRenderer() {}

    HRESULT Init(uint32_t _uWidth, uint32_t _uHeight);

    void DrawOpaques(void) {}
    void ComputeGraphicsBuffer(void) {}

    void AttachRenderTargets(ID3D11DepthStencilView* _pDepthStencilView);
    void DetachRenderTargets();

    const CRenderTarget* GetDiffuseRT() const { return m_pDiffuseRT.get(); }
    const CRenderTarget* GetNormalRT() const { return m_pNormalRT.get(); }
    const CRenderTarget* GetSpecularRT() const { return m_pSpecularRT.get(); }

    void ClearRenderTargets(const float _v4ClearColor[4]);

  private:
    HRESULT SetupRenderTargets(uint32_t _uWidth, uint32_t _uHeight);
    void Release();

  private:
    std::unique_ptr<CRenderTarget> m_pDiffuseRT;
    std::unique_ptr<CRenderTarget> m_pNormalRT;
    std::unique_ptr<CRenderTarget> m_pSpecularRT;
  };
}


