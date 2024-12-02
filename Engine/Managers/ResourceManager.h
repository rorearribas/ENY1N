#pragma once
#include "Libs/Utils/Singleton.h"
#include "Engine/Render/Graphics/Mesh.h"
#include "Engine/Render/Resources/Material.h"
#include <vector>

class CResourceManager : public utils::CSingleton<CResourceManager>
{
public:
  struct SModelData
  {
    std::vector<render::material::CMaterial> m_vctMaterials = {};
    std::vector<render::graphics::SVertexData> m_vctVertexData = {};
    std::vector<uint32_t> m_vctIndexes = {};
  };

  struct SModelData2
  {
    std::vector<render::graphics::CMesh*> m_vctMeshes = {};
    std::vector<render::graphics::SVertexData> m_vctVertexData = {};
    std::vector<uint32_t> m_vctIndexes = {};
  };

public:
  [[nodiscard]] char* LoadResource(const char* _sPath, const char* _sMode);
  //[[nodiscard]] SModelData LoadModelData(const char* _sPath, const char* _sMode);
  [[nodiscard]] std::vector<render::material::CMaterial> LoadMaterials(const char* _sPath, const char* _sMode);


  [[nodiscard]] std::vector<render::graphics::CMesh*> LoadModel(const char* _sPath, const char* _sBaseModelMtlDir);
};



