#pragma once
#include <d3d11.h>
#include <string>

namespace render
{
  namespace texture
  {
    // Types
    enum class ETextureType : uint32_t
    {
      DIFFUSE,
      SPECULAR,
      AMBIENT,
      EMISSIVE,
      HEIGHT,
      NORMAL,
      SHININESS,
      OPACITY,
      DISPLACEMENT,
      LIGHTMAP,
      REFLECTION,
      BASE_COLOR,
      NORMAL_CAMERA,
      EMISSION_COLOR,
      METALNESS,
      DIFFUSE_ROUGHNESS,
      AMBIENT_OCCLUSSION,
      COUNT
    };

    class CTexture
    {
    public:
      static constexpr uint32_t s_uChannels = 4;

    public:
      CTexture(const std::string& _sTextureID) : m_sTextureID(_sTextureID) {}
      ~CTexture();

      void BindTexture();
      HRESULT SetTexture(void* _pData, uint32_t _uWidth, uint32_t _uHeight, uint32_t _uChannels);
      ID3D11Texture2D* const GetTexture() { return m_pTexture; }

      ID3D11SamplerState* const GetSamplerState() { return m_pSamplerState; }
      ID3D11ShaderResourceView* const GetShaderResourceView() { return m_pShaderResourceView; }

    private:
      void Clear();

      // Info
      std::string m_sTextureID;

      uint32_t m_uTextureWidth = 0;
      uint32_t m_uTextureHeight = 0;
      uint32_t m_uChannels = 0;

      // DirectX
      ID3D11Texture2D* m_pTexture = nullptr;
      ID3D11ShaderResourceView* m_pShaderResourceView = nullptr;
      ID3D11SamplerState* m_pSamplerState = nullptr;
    };
  }
}


