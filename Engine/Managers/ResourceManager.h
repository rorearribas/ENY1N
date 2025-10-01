#pragma once
#include "Libs/Utils/Singleton.h"
#include "Engine/Render/Graphics/Model.h"
#include <filesystem>

class CResourceManager : public utils::CSingleton<CResourceManager>
{
public:
  [[nodiscard]] char* LoadFile(const char* _sPath, const char* _sMode);
  [[nodiscard]] unsigned char* LoadTexture(const char* _sPath, int& _iWidth_, int& _iHeight_, int& _iChannels_);

  [[nodiscard]] render::graphics::CModel::SModelData LoadFBX(const char* _sPath);
  [[nodiscard]] render::graphics::CModel::SModelData LoadOBJ(const char* _sPath, const char* _sBaseModelMtlDir);

private:
  void RegisterTexture(render::material::CMaterial*& pMaterial, render::material::CMaterial::EType _eModifierType,
    const std::filesystem::path& _oBasePath, const std::string& _sTextureName);
};



