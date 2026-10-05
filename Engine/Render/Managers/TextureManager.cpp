#include "TextureManager.h"

namespace render
{
  texture::TSharedTexture CTextureManager::Create(const std::string& _sKey, const texture::TTextureData& _rTextureData, const D3D11_SHADER_RESOURCE_VIEW_DESC& _rViewDesc)
  {
    auto it = m_lstTextures.find(_sKey);
    if (it != m_lstTextures.end())
    {
      return it->second;
    }

    if (!m_pRenderDevice)
    {
      return nullptr;
    }

    auto pTexture = std::make_shared<render::texture::TShaderResource>();
    HRESULT hResult = pTexture->CreateTexture(*m_pRenderDevice, _rTextureData);
    if (FAILED(hResult))
    {
      return nullptr;
    }

    hResult = pTexture->CreateView(*m_pRenderDevice, _rViewDesc);
    if (FAILED(hResult))
    {
      return nullptr;
    }

    m_lstTextures.emplace(_sKey, pTexture);
    return pTexture;
  }
  // ------------------------------------
  texture::TSharedTexture CTextureManager::Find(const std::string& _sKey) const
  {
    auto it = m_lstTextures.find(_sKey);
    return it == m_lstTextures.end() ? nullptr : it->second;
  }
}
