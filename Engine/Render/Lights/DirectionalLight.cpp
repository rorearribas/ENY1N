#include "DirectionalLight.h"
#include <cassert>
#include "Libs/Macros/GlobalMacros.h"

namespace render
{
  namespace lights
  {
    CDirectionalLight::CDirectionalLight() : CLight(DIRECTIONAL_LIGHT)
    {
      InitDefaultLight();
      HRESULT hr = m_oConstantBuffer.Init(global::dx11::s_pDevice, global::dx11::s_pDeviceContext);
      UNUSED_VARIABLE(hr);
      assert(!FAILED(hr));
    }
    // ------------------------------------
    void CDirectionalLight::UpdateLight()
    {
      // Update buffer
      m_oConstantBuffer.GetData().Direction = m_v3Direction;
      m_oConstantBuffer.GetData().Color = m_v3LightColor;
      m_oConstantBuffer.GetData().Intensity = m_fIntensity;
      m_oConstantBuffer.UpdateBuffer();

      // Set constant buffer
      ID3D11Buffer* pConstantBuffer = m_oConstantBuffer.GetBuffer();
      global::dx11::s_pDeviceContext->PSSetConstantBuffers(1, 1, &pConstantBuffer);
    }
    // ------------------------------------
    void CDirectionalLight::InitDefaultLight()
    {
      SetColor(maths::CVector3(1.0f, 0.9f, 0.6f));
    }
  }
}
