#include "Shader.h"
#include "Engine/Global/GlobalResources.h"

// Compiled shaders
#include "Engine/Shaders/Model/DeferredVertexShader.h"
#include "Engine/Shaders/Model/DeferredPixelShader.h"

namespace render
{
  namespace shader
  {
    void SVertexShader::Release()
    {
      global::dx11::SafeRelease(m_pVertexShader);
    }

    HRESULT SVertexShader::CreateShader()
    {
      // Create shader
      return global::dx11::s_pDevice->CreateVertexShader
      (
        g_DeferredVertexShader,
        sizeof(g_DeferredVertexShader),
        nullptr,
        &m_pVertexShader
      );
    }

    HRESULT SPixelShader::CreateShader()
    {
      // Create shader
      return global::dx11::s_pDevice->CreatePixelShader
      (
        g_DeferredPixelShader,
        sizeof(g_DeferredPixelShader),
        nullptr,
        &m_pPixelShader
      );
    }

    void SPixelShader::Release()
    {
      global::dx11::SafeRelease(m_pPixelShader);
    }
  }
}

