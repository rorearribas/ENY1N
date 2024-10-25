#include "RenderSystem.h"

namespace render
{
  void CRenderSystem::InitDevice()
  {
    D3D_FEATURE_LEVEL featureLevels[] =
    {
      D3D_FEATURE_LEVEL_11_0,
      D3D_FEATURE_LEVEL_10_1,
      D3D_FEATURE_LEVEL_10_0,
      D3D_FEATURE_LEVEL_9_3,
      D3D_FEATURE_LEVEL_9_2,
      D3D_FEATURE_LEVEL_9_1
    };
    UINT uNumFeatureLevels = ARRAYSIZE(featureLevels);

    HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
    D3D11_CREATE_DEVICE_BGRA_SUPPORT, featureLevels, uNumFeatureLevels, D3D11_SDK_VERSION,
    &m_pDevice, nullptr, &m_pDeviceContext);
  }

  void CRenderSystem::InitWindow(const UINT32& _uWidth, const UINT32& _uHeight)
  {
    m_pRenderWindow = new CRenderWindow();
    m_pRenderWindow->CreateRenderWindow(_uWidth, _uHeight);

    m_pRenderWindow->ShowRenderWindow();
    m_pRenderWindow->UpdateWindow();
  }

}