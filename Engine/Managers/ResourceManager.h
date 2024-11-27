#pragma once
#include "Libs/Utils/Singleton.h"
#include "Engine/Render/Graphics/Model.h"
#include <vector>

class CResourceManager : public utils::CSingleton<CResourceManager>
{
public:
  struct SModelData
  {
    std::vector<render::graphics::CModel::SVertexInfo> m_vctVertexInfo = {};
    std::vector<uint32_t> m_vctIndexes = {};
  };
public:
  [[nodiscard]] char* LoadResource(const char* _sPath, const char* _sMode);
  [[nodiscard]] SModelData LoadModelData(const char* _sPath, const char* _sMode);
};



