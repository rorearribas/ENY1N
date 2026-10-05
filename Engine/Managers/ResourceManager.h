#pragma once
#include "Libs/Utils/Singleton.h"
#include "Engine/Render/RenderTypes.h"
#include "Engine/Render/Graphics/Model.h"
#include "Engine/Render/RenderContext/RenderDeviceDX11.h"
#include "Engine/Render/Managers/TextureManager.h"
#include <filesystem>

class CResourceManager : public utils::CSingleton<CResourceManager>
{
public:
  static std::string s_sRelativeTexturesPath;

public:
  CResourceManager() {}
  ~CResourceManager() {}

  [[nodiscard]] unsigned char* LoadImage(const char* _sPath, int& _iWidth_, int& _iHeight_, int& _iChannels_);
  [[nodiscard]] render::gfx::TModelData LoadModel(const char* _sPath);
  void Initialize(const render::CRenderDeviceDX11& _rRenderDevice, render::CTextureManager* _pTextureManager = nullptr) { m_pRenderDevice = &_rRenderDevice; m_pTextureManager = _pTextureManager; }

private:
  void RegisterTexture(std::unique_ptr<render::mat::CMaterial>& _pMaterial_, render::ETexture _eType, const std::filesystem::path& _sPath);
  std::unordered_map<std::string, render::texture::TSharedTexture> m_lstCachedTextures;
  const render::CRenderDeviceDX11* m_pRenderDevice = nullptr;
  render::CTextureManager* m_pTextureManager = nullptr;
};


