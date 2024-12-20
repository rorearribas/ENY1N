#pragma once
#include <d3d11.h>

namespace render
{
  namespace texture
  {
    class CTexture 
    {
    public:
      CTexture() {}
      ~CTexture();

      HRESULT SetTexture(unsigned char* _cTexture, int _iWidth, int _iHeight);
      void BindTexture();
      void ClearTexture();

      ID3D11Texture2D* const GetTexture() { return m_pTexture; }
      ID3D11SamplerState* const GetSamplerState() { return m_pSamplerState; }
      ID3D11ShaderResourceView* const GetShaderResourceView() { return m_pShaderResourceView; }

    private:
      ID3D11Texture2D* m_pTexture = nullptr;
      ID3D11ShaderResourceView* m_pShaderResourceView = nullptr;
      ID3D11SamplerState* m_pSamplerState = nullptr;
    };
  }
}


