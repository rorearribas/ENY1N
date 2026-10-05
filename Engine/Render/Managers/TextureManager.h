#pragma once
#include "Engine/Render/Resources/Texture2D.h"
#include <unordered_map>
#include <string>

namespace render
{
  class CTextureManager
  {
  public:
    CTextureManager() = default;
    ~CTextureManager() = default;

    void Initialize(const CRenderDeviceDX11& _rRenderDevice) { m_pRenderDevice = &_rRenderDevice; }

    texture::TSharedTexture Create(const std::string& _sKey, const texture::TTextureDesc& _rTextureDesc, const D3D11_SHADER_RESOURCE_VIEW_DESC& _rViewDesc);
    texture::TSharedTexture Find(const std::string& _sKey) const;

  private:
    const CRenderDeviceDX11* m_pRenderDevice = nullptr;
    std::unordered_map<std::string, texture::TSharedTexture> m_lstTextures;
  };
}
