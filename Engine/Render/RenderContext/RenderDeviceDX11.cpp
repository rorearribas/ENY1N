#include "RenderDeviceDX11.h"
#include "RenderCommandsDX11.h"

namespace render
{
  HRESULT CRenderDeviceDX11::Init(const TRenderContextDesc& _rRenderContextDesc, CRenderCommandsDX11& _rCommands)
  {
    DXGI_MODE_DESC rBufferDesc = DXGI_MODE_DESC();
    {
      rBufferDesc.Width = _rRenderContextDesc.uWidth;
      rBufferDesc.Height = _rRenderContextDesc.uHeight;
      rBufferDesc.RefreshRate.Numerator = _rRenderContextDesc.uRefreshRate;
      rBufferDesc.RefreshRate.Denominator = 1u;
      rBufferDesc.Format = _rRenderContextDesc.eFormat;
      rBufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
      rBufferDesc.Scaling = DXGI_MODE_SCALING_CENTERED;
    }

    DXGI_SWAP_CHAIN_DESC rSwapChainDesc = DXGI_SWAP_CHAIN_DESC();
    {
      rSwapChainDesc.OutputWindow = _rRenderContextDesc.pOutputWindow;
      rSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
      rSwapChainDesc.BufferCount = _rRenderContextDesc.uBufferCount;
      rSwapChainDesc.BufferDesc = rBufferDesc;
      rSwapChainDesc.SampleDesc.Count = 1u;
      rSwapChainDesc.SampleDesc.Quality = 0u;
      rSwapChainDesc.Windowed = !_rRenderContextDesc.bFullscreen;
    }

    D3D_FEATURE_LEVEL lstFeatureLevels[] =
    {
      D3D_FEATURE_LEVEL_11_0,
      D3D_FEATURE_LEVEL_10_1,
      D3D_FEATURE_LEVEL_10_0,
      D3D_FEATURE_LEVEL_9_3,
      D3D_FEATURE_LEVEL_9_2,
      D3D_FEATURE_LEVEL_9_1
    };
    uint32_t uNumFeatureLevels = ARRAYSIZE(lstFeatureLevels);
    D3D_FEATURE_LEVEL rFeatureLevel = D3D_FEATURE_LEVEL();
    uint32_t uFlags = 0;
    ID3D11DeviceContext* pContext = nullptr;

    Clean();
    _rCommands.Clean();

    HRESULT hResult = D3D11CreateDeviceAndSwapChain
    (
      nullptr,
      D3D_DRIVER_TYPE_HARDWARE,
      nullptr,
      uFlags,
      lstFeatureLevels,
      uNumFeatureLevels,
      D3D11_SDK_VERSION,
      &rSwapChainDesc,
      &m_pSwapChain,
      &m_pDevice,
      &rFeatureLevel,
      &pContext
    );

    if (SUCCEEDED(hResult))
    {
      _rCommands.SetContext(pContext);
    }
    else
    {
      if (pContext)
      {
        pContext->Release();
      }
    }

    return hResult;
  }
  // ------------------------------------
  void CRenderDeviceDX11::Clean()
  {
    if (m_pSwapChain)
    {
      m_pSwapChain->Release();
      m_pSwapChain = nullptr;
    }

    if (m_pDevice)
    {
      m_pDevice->Release();
      m_pDevice = nullptr;
    }
  }
}
