#pragma once
#include "Libs/Utils/Singleton.h"
#include "Engine/Render/Graphics/Model.h"

class CResourceManager : public utils::CSingleton<CResourceManager>
{
public:
  [[nodiscard]] char* LoadResource(const char* _sPath, const char* _sMode);
  [[nodiscard]] render::graphics::CModel::SModelData LoadModel(const char* _sPath, const char* _sBaseModelMtlDir);
};



