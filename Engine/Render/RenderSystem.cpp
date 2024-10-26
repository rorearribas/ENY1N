#include "RenderSystem.h"

namespace render
{
  CRenderSystem::CRenderSystem(const UINT32& _uWidth, const UINT32& _uHeight)
  {
    // Create render window
    m_pRenderWindow = new CRenderWindow();
    m_pRenderWindow->CreateRenderWindow(_uWidth, _uHeight);
  }
  // ------------------------------------
  CRenderSystem::~CRenderSystem()
  {
    if (m_pDevice) { m_pDevice->Release(); }
    if (m_pDeviceContext) { m_pDeviceContext->Release(); }
    if (m_pDXGISwapChain) { m_pDXGISwapChain->Release(); }
    if (m_pRenderTargetView) { m_pRenderTargetView->Release(); }
  }
  // ------------------------------------
  HRESULT CRenderSystem::InitDevice()
  {
    DXGI_SWAP_CHAIN_DESC oSwapChainDescriptor = {};
    oSwapChainDescriptor.BufferCount = 1;
    oSwapChainDescriptor.BufferDesc.Width = m_pRenderWindow->GetWidth();
    oSwapChainDescriptor.BufferDesc.Height = m_pRenderWindow->GetHeight();
    oSwapChainDescriptor.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    oSwapChainDescriptor.BufferDesc.RefreshRate.Numerator = 60;
    oSwapChainDescriptor.BufferDesc.RefreshRate.Denominator = 1;
    oSwapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    oSwapChainDescriptor.OutputWindow = m_pRenderWindow->GetHwnd();
    oSwapChainDescriptor.SampleDesc.Count = 1;
    oSwapChainDescriptor.SampleDesc.Quality = 0;
    oSwapChainDescriptor.Windowed = TRUE;

    D3D_FEATURE_LEVEL vctFeatureLevels[] =
    {
      D3D_FEATURE_LEVEL_11_0,
      D3D_FEATURE_LEVEL_10_1,
      D3D_FEATURE_LEVEL_10_0,
      D3D_FEATURE_LEVEL_9_3,
      D3D_FEATURE_LEVEL_9_2,
      D3D_FEATURE_LEVEL_9_1
    };
    D3D_FEATURE_LEVEL oFeatureLevel;
    UINT uNumFeatureLevels = ARRAYSIZE(vctFeatureLevels);

    // Device and swapchain
    HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, vctFeatureLevels, 
    uNumFeatureLevels, D3D11_SDK_VERSION, &oSwapChainDescriptor, &m_pDXGISwapChain,
    &m_pDevice, &oFeatureLevel, &m_pDeviceContext);
    if (FAILED(hr)) return hr;

    // Render target view
    hr = CreateRenderTargetView();
    if (FAILED(hr)) return hr;

    // Viewport
    ConfigureViewport();

    return S_OK;
  }
  // ------------------------------------
  HRESULT CRenderSystem::CreateRenderTargetView()
  {
    ID3D11Texture2D* pBackBuffer = nullptr;
    m_pDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
    if (!pBackBuffer) return -1;

    m_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pRenderTargetView);
    pBackBuffer->Release();
    m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, nullptr);
  }

  // ------------------------------------
  void CRenderSystem::UpdateRender()
  {
    FLOAT vctClearColor[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
    m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, vctClearColor);
    m_pDXGISwapChain->Present(0, 0);
  }

  void CRenderSystem::ConfigureViewport()
  {
    if(m_pDeviceContext && m_pRenderWindow)
    {
      D3D11_VIEWPORT vp = {};
      vp.Width = (FLOAT)m_pRenderWindow->GetWidth();
      vp.Height = (FLOAT)m_pRenderWindow->GetHeight();
      vp.MinDepth = 0.0f;
      vp.MaxDepth = 1.0f;
      m_pDeviceContext->RSSetViewports(1, &vp);
    }
  }
}