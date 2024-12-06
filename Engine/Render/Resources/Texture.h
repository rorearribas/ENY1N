#pragma once
#include <d3d11.h>

namespace render
{
  namespace texture
  {
    class CTexture 
    {
    public:
      explicit CTexture(const char* _sTexturePath);
      ~CTexture();

      void BindTexture();
      void ClearTexture();
      HRESULT LoadTexture(const char* _sTexturePath);

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


