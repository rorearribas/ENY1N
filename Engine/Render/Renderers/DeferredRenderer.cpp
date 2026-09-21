#include "DeferredRenderer.h"
#include "Engine/Render/Render.h"
#include "Engine/Scenes/RenderScene.h"

namespace render
{
  namespace internal
  {
    static constexpr uint32_t uRenderTargets(3);
    static const float s_v4ClearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
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
    // Create depth stencil texture
    D3D11_TEXTURE2D_DESC rTextureDesc = D3D11_TEXTURE2D_DESC();
    rTextureDesc.Width = _uWidth;
    rTextureDesc.Height = _uHeight;
    rTextureDesc.MipLevels = 1;
    rTextureDesc.ArraySize = 1;
    rTextureDesc.SampleDesc.Count = 1;
    rTextureDesc.Format = DXGI_FORMAT_R32_TYPELESS; // Format
    rTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE; // Depth stencil

    m_oDepthStencil.Release();
    HRESULT hResult = m_oDepthStencil.CreateTexture(rTextureDesc);
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating depth stencil texture!");
      return hResult;
    }

    // Set depth stencil view desc
    D3D11_DEPTH_STENCIL_VIEW_DESC rDepthStencilViewDesc = D3D11_DEPTH_STENCIL_VIEW_DESC();
    rDepthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
    rDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

    // Create the depth stencil view
    hResult = m_oDepthStencil.CreateView(rDepthStencilViewDesc);
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating stencil view!");
      return hResult;
    }

    // Creating view from texture
    D3D11_SHADER_RESOURCE_VIEW_DESC rSRVDesc = D3D11_SHADER_RESOURCE_VIEW_DESC();
    rSRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
    rSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    rSRVDesc.Texture2D.MipLevels = 1;

    m_oDepthStencilShader.Release();
    hResult = m_oDepthStencilShader.CreateViewFromTexture(m_oDepthStencil, rSRVDesc);
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating view!");
      return hResult;
    }

    // Create standard depth stencil state for zprepass
    D3D11_DEPTH_STENCIL_DESC rDepthStencilDesc = D3D11_DEPTH_STENCIL_DESC();
    rDepthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    rDepthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
    rDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    rDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    rDepthStencilDesc.DepthEnable = true;
    rDepthStencilDesc.StencilEnable = false;

    // Front-face
    rDepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    rDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    rDepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    rDepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Back-face
    rDepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    rDepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    rDepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    rDepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Depth
    global::api::SafeRelease(m_pDepthStencilState);
    hResult = m_pRender->CreateDepthStencilState(rDepthStencilDesc, &m_pDepthStencilState);
    if (FAILED(hResult))
    {
      ERROR_LOG("Error creating depth stencil state!");
      return hResult;
    }

    return SetupRenderTargets(_uWidth, _uHeight);
  }
  // ------------------------------------
  void CDeferredRenderer::PrepareFrame()
  {
    // Clear render targets
    m_pDiffuseRT->SetClearColor(internal::s_v4ClearColor);
    m_pNormalRT->SetClearColor(internal::s_v4ClearColor);
    m_pSpecularRT->SetClearColor(internal::s_v4ClearColor);

    // Clear depth stencil -> zbuffer
    m_pRender->ClearDepthStencil(GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
  }
  // ------------------------------------
  void CDeferredRenderer::Draw(scene::CRenderScene& _rRenderScene)
  {
    // Set render targets
    ID3D11RenderTargetView* lstGBufferRTs[internal::uRenderTargets] =
    {
      m_pDiffuseRT->GetRenderTargetView(),
      m_pNormalRT->GetRenderTargetView(),
      m_pSpecularRT->GetRenderTargetView()
    };

    m_pRender->SetRenderTargets(lstGBufferRTs, internal::uRenderTargets, GetDepthStencilView()); // Set render targets
    m_pRender->SetDepthStencilState(m_pDepthStencilState, 1u); // Set depth stencil state

    // Cache models
    _rRenderScene.CacheModels(*m_pRenderCamera);

    // Draw models
    m_pRender->DrawModels(_rRenderScene);

    // Detach render targets
    ID3D11RenderTargetView* lstEmptyRTs[internal::uRenderTargets] = { nullptr, nullptr, nullptr };
    m_pRender->SetRenderTargets(lstEmptyRTs, internal::uRenderTargets);
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

