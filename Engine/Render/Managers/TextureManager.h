#pragma once
#include "Engine/Render/Resources/Texture2D.h"
#include <unordered_map>
#include <string>

namespace render
{
  class CTextureManager
  {
  public:
    typedef std::unordered_map<std::string, texture::TSharedTexture> TCachedTextures;

    CTextureManager() = default;
    ~CTextureManager() = default;

    void Initialize(const CRenderDeviceDX11& _rRenderDevice) { m_pRenderDevice = &_rRenderDevice; }
    const TCachedTextures& GetChachedTexture() { return m_lstTextures; }

    render::texture::TSharedTexture Create(const std::string& _sKey, const texture::TTextureData& _rTextureData, const D3D11_SHADER_RESOURCE_VIEW_DESC& _rViewDesc);
    render::texture::TSharedTexture Find(const std::string& _sKey) const;

  private:
    const CRenderDeviceDX11* m_pRenderDevice = nullptr;
    TCachedTextures m_lstTextures;
  };
}
