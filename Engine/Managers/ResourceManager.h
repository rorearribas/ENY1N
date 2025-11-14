#pragma once
#include "Libs/Utils/Singleton.h"
#include "Engine/Render/RenderTypes.h"
#include "Engine/Render/Graphics/Model.h"
#include <filesystem>

class CResourceManager : public utils::CSingleton<CResourceManager>
{
public:
  CResourceManager() {}
  ~CResourceManager() {}

  [[nodiscard]] char* LoadFile(const char* _sPath, const char* _sMode);
  [[nodiscard]] unsigned char* LoadImage(const char* _sPath, int& _iWidth_, int& _iHeight_, int& _iChannels_);
  [[nodiscard]] std::unique_ptr<render::gfx::CModel> LoadModel(const char* _sPath);

private:
  void RegisterTexture(render::mat::CMaterial*& pMaterial, render::ETextureType _eType,
    const std::filesystem::path& _oBasePath, const std::string& _sTextureID);
};


