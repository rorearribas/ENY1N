#include "RenderSystem.h"
#include <d3dcompiler.h>
#include <array>

namespace render
{
  namespace internal_rendersystem
  {
    static const float s_fMinDepth(0.0f);
    static const float s_fMaxDepth(1.0f);
  }

  CRenderSystem::CRenderSystem(const UINT32& _uWidth, const UINT32& _uHeight)
  {
    // Create render window
    m_pRenderWindow = new CRenderWindow(_uWidth, _uHeight);
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

    HRESULT hr = m_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pRenderTargetView);
    pBackBuffer->Release();
    m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, nullptr);

    return hr;
  }
  // ------------------------------------
  void CRenderSystem::ConfigureViewport()
  {
    if (m_pDeviceContext && m_pRenderWindow)
    {
      RECT rWinRect;
      GetClientRect(m_pRenderWindow->GetHwnd(), &rWinRect);

      D3D11_VIEWPORT oD3D11Viewport = {};
      oD3D11Viewport.Width = (FLOAT)(rWinRect.right - rWinRect.left);
      oD3D11Viewport.Height = (FLOAT)(rWinRect.bottom - rWinRect.top);
      oD3D11Viewport.MinDepth = internal_rendersystem::s_fMinDepth;
      oD3D11Viewport.MaxDepth = internal_rendersystem::s_fMaxDepth;

      m_pDeviceContext->RSSetViewports(1, &oD3D11Viewport);
    }
  }
  // ------------------------------------
  void CRenderSystem::UpdateRender()
  {
    /* clear the back buffer to cornflower blue for the new frame */
    float background_color[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
    m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, background_color);

    /***** Input Assembler (map how the vertex shader inputs should be read from vertex buffer) ******/
    for (auto& oPrimitiveItem : m_vctPrimitiveItems)
    {
      UINT vertex_stride = 3 * sizeof(float);
      UINT vertex_offset = 0;

      m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
      m_pDeviceContext->IASetInputLayout(oPrimitiveItem->GetInputLayout());

      ID3D11Buffer* pBuffer = oPrimitiveItem->GetBuffer();
      m_pDeviceContext->IASetVertexBuffers(0, 1, &pBuffer, &vertex_stride, &vertex_offset);

      /*** set vertex shader to use and pixel shader to use, and constant buffers for each ***/
      m_pDeviceContext->VSSetShader(oPrimitiveItem->GetVertexShader(), NULL, 0);
      m_pDeviceContext->PSSetShader(oPrimitiveItem->GetPixelShader(), NULL, 0);

      /*** draw the vertex buffer with the shaders ****/
      m_pDeviceContext->Draw(oPrimitiveItem->GetIndexCount(), 0);
    }

    /**** swap the back and front buffers (show the frame we just drew) ****/
    m_pDXGISwapChain->Present(1, 0);
  }
  // ------------------------------------
  items::CPrimitiveItem* CRenderSystem::CreatePrimitiveItem(std::vector<float>& _vctVertexData)
  {
    return m_vctPrimitiveItems.emplace_back(new items::CPrimitiveItem(m_pDevice, _vctVertexData));
  }
}