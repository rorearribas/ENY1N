#pragma once
#include "Libs/Utils/Singleton.h"
#include "Engine/Render/RenderTypes.h"
#include "Engine/Render/Graphics/Model.h"
#include <filesystem>

class CResourceManager : public utils::CSingleton<CResourceManager>
{
public:
  static std::string s_sRelativeTexturesPath;

public:
  CResourceManager() {}
  ~CResourceManager() {}

  [[nodiscard]] char* LoadFile(const char* _sPath, const char* _sMode);
  [[nodiscard]] unsigned char* LoadImage(const char* _sPath, int& _iWidth_, int& _iHeight_, int& _iChannels_);
  [[nodiscard]] std::unique_ptr<render::gfx::CModel> LoadModel(const char* _sPath);

private:
  void RegisterTexture(std::unique_ptr<render::mat::CMaterial>& _pMaterial_, render::ETexture _eType, const std::filesystem::path& _sPath);
  std::unordered_map<std::string, render::texture::TSharedTexture> m_lstCachedTextures;
};


