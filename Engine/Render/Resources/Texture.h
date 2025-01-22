#pragma once
#include <d3d11.h>
#include <string>

namespace render
{
  namespace texture
  {
    class CTexture 
    {
    public:
      explicit CTexture(const std::string& _sTextureName) : m_sTextureName(_sTextureName) {}
      ~CTexture();

      HRESULT SetTexture(unsigned char* _pTexture, uint32_t _uWidth, uint32_t _uHeight);
      void BindTexture();
      void ClearTexture();

      ID3D11Texture2D* const GetTexture() { return m_pTexture; }
      ID3D11SamplerState* const GetSamplerState() { return m_pSamplerState; }
      ID3D11ShaderResourceView* const GetShaderResourceView() { return m_pShaderResourceView; }

    private:
      // Info
      std::string m_sTextureName;
      uint32_t m_uTextureWidth = 0;
      uint32_t m_uTextureHeight = 0;

      // DirectX
      ID3D11Texture2D* m_pTexture = nullptr;
      ID3D11ShaderResourceView* m_pShaderResourceView = nullptr;
      ID3D11SamplerState* m_pSamplerState = nullptr;
    };
  }
}


