#include "DeferredRenderer.h"

namespace render
{
  namespace internal
  {
    static constexpr uint32_t uRenderTargets(3);
  }
  // ------------------------------------
  void CDeferredRenderer::Release()
  {
    m_pDiffuseRT.reset();
    m_pNormalRT.reset();
    m_pSpecularRT.reset();
  }
  // ------------------------------------
  HRESULT CDeferredRenderer::Init(uint32_t _uWidth, uint32_t _uHeight)
  {
    return SetupRenderTargets(_uWidth, _uHeight);
  }
  // ------------------------------------
  void CDeferredRenderer::AttachRenderTargets(ID3D11DepthStencilView* _pDepthStencilView)
  {
    ID3D11RenderTargetView* lstGBufferRTV[internal::uRenderTargets] =
    { 
      m_pDiffuseRT->GetRenderTargetView(), 
      m_pNormalRT->GetRenderTargetView(), 
      m_pSpecularRT->GetRenderTargetView()
    };

    // Set render targets
    global::api::DeviceContext->OMSetRenderTargets(internal::uRenderTargets, lstGBufferRTV, _pDepthStencilView);
  }
  // ------------------------------------
  void CDeferredRenderer::DetachRenderTargets()
  {
    // Remove render targets
    static constexpr uint32_t uRenderTargets(3);
    ID3D11RenderTargetView* lstEmptyRTs[uRenderTargets] = { nullptr, nullptr, nullptr };
    global::api::DeviceContext->OMSetRenderTargets(uRenderTargets, lstEmptyRTs, nullptr);
  }
  // ------------------------------------
  void CDeferredRenderer::ClearRenderTargets(const float _v4ClearColor[4])
  {
    m_pDiffuseRT->SetClearColor(_v4ClearColor);
    m_pNormalRT->SetClearColor(_v4ClearColor);
    m_pSpecularRT->SetClearColor(_v4ClearColor);
  }
  // ------------------------------------
  HRESULT CDeferredRenderer::SetupRenderTargets(uint32_t _uWidth, uint32_t _uHeight)
  {
    // Diffuse
    std::unique_ptr<CRenderTarget> pDiffuseRT = std::make_unique<CRenderTarget>();
    HRESULT hResult = pDiffuseRT->Init(_uWidth, _uHeight, DXGI_FORMAT_R8G8B8A8_UNORM);
    if (FAILED(hResult))
    {
      return hResult;
    }

    // Normal
    std::unique_ptr<CRenderTarget> pNormalRT = std::make_unique<CRenderTarget>();
    hResult = pNormalRT->Init(_uWidth, _uHeight, DXGI_FORMAT_R16G16B16A16_FLOAT);
    if (FAILED(hResult))
    {
      return hResult;
    }

    // Specular
    std::unique_ptr<CRenderTarget> pSpecularRT = std::make_unique<CRenderTarget>();
    hResult = pSpecularRT->Init(_uWidth, _uHeight, DXGI_FORMAT_R8G8B8A8_UNORM);
    if (FAILED(hResult))
    {
      return hResult;
    }

    // Clean
    Release();

    // Set 
    m_pDiffuseRT = std::move(pDiffuseRT);
    m_pNormalRT = std::move(pNormalRT);
    m_pSpecularRT = std::move(pSpecularRT);

    return hResult;
  }
}

