#pragma once
#include "Libs/Utils/Singleton.h"
#include "Engine/Render/RenderTypes.h"
#include "Engine/Render/Graphics/Model.h"
#include <filesystem>

class CResourceManager : public utils::CSingleton<CResourceManager>
{
public:
  CResourceManager() {}
  ~CResourceManager();

  [[nodiscard]] char* LoadFile(const char* _sPath, const char* _sMode);
  [[nodiscard]] unsigned char* LoadImage(const char* _sPath, int& _iWidth_, int& _iHeight_, int& _iChannels_);
  [[nodiscard]] render::gfx::CModel* LoadModel(const char* _sPath, bool& _bCached_);

private:
  void RegisterTexture(render::mat::CMaterial*& pMaterial, render::ETextureType _eType,
    const std::filesystem::path& _oBasePath, const std::string& _sTextureID);

private:
  std::unordered_map<std::string, std::shared_ptr<render::gfx::CModel>> m_dctLoadedModels;
};


